/**
**  \file
**  \brief     CPU addressing unit
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.29
*/


#include "rgm_cpua.h"
#include "rgm_mix.h"
#include "rgm_vid.h"
#include "rgm_dma.h"


/* Stack bottom. Always zero; if interrupt handlers were be available, it
** would need other values (see versions before 2014.06.23). */
#define STK_B 0U


/* Pointer op. function type for the pointer op. table */
typedef RRPGE_M_FASTCALL void (rrpge_m_xop_t)(void);


/* Write function pointer (global) - this is accessed to call the write part
** of an R-M-W sequence, set up by the read part. */
rrpge_m_addr_write_t* rrpge_m_addr_write;

/* Module globals for various latches serving the R-M-W accesses */
static auint  rrpge_m_offhi; /* Data offset (high 4 bits), for selecting data page */
static auint  rrpge_m_offpg; /* Data offset page << 12 */
static auint  rrpge_m_offlw; /* Data offset (low 12 bits) / Stack offset (full) */
static auint  rrpge_m_avlt;  /* Video latch */
static auint  rrpge_m_arlt;  /* Data RAM read latch */
static auint  rrpge_m_adtsh; /* Right shift amount for sub-word accesses */
static auint  rrpge_m_adtms; /* Mask for sub-word accesses (read preserve) */



/* Data RAM read operation for assisting Read accesses. Uses the Read / Write
** offsets, returns in rrpge_m_arlt (only low 16 bits may be set in that). */
RRPGE_M_FASTCALL static void rrpge_m_addr_rd_data(void)
{
 rrpge_m_offhi = rrpge_m_offlw >> 12;
 rrpge_m_offlw = rrpge_m_offlw & 0xFFFU;
 rrpge_m_offpg = rrpge_m_info.rbk[rrpge_m_offhi]; /* Page of data ( << 12 ) */

 /* Note: only sanity maskings are used to ensure accessing unused areas won't
 ** crash the library. Unused banks shouldn't occur as the kernel should have
 ** errored out on the first place when the user attempted to set those. */

 if ((rrpge_m_offpg & 0x8000000U) == 0U){ /* 0x4000 - 0x41BF: Data pages
                                          ** 0x41C0: Read Only Process Descriptor
                                          ** 0x7FFF: User peripheral area */

  if (rrpge_m_offpg < 0x41C0000U){        /* Assume Data pages */
   rrpge_m_arlt = (auint)(rrpge_m_edat->stat.dram[(rrpge_m_offpg & 0x1FF000U) + rrpge_m_offlw]);
  }else if ((rrpge_m_offpg & 0x2000000U) == 0U){ /* Assume Read Only Process Descriptor */
   if (rrpge_m_offlw < 0xD40U){
    rrpge_m_arlt = (auint)(rrpge_m_edat->stat.ropd[rrpge_m_offlw]);
   }else{
    rrpge_m_arlt = (auint)(rrpge_m_edat->ropc[rrpge_m_offlw - 0xD40U]);
   }
  }else{                                  /* Assume User peripheral page */
   if (rrpge_m_offlw < 0xE00U){           /* RAM pg. 0 shadow */
    rrpge_m_arlt = (auint)(rrpge_m_edat->stat.dram[rrpge_m_offlw]);
   }else if (rrpge_m_offlw < 0xF00U){     /* Repeating 16 word graphics range */
    if ((rrpge_m_offlw & 0xCU) == 0x00U){ /* Graphics FIFO */
     if ((rrpge_m_offlw & 0xFU) == 0x01U){
      rrpge_m_arlt = rrpge_m_edat->stat.ropd[0xD71U]; /* FIFO busy flag */
     }else{
      rrpge_m_arlt = 0U;                  /* Other registers of the FIFO return zero */
     }
    }else{                                /* Other registers than the FIFO */
     rrpge_m_arlt = (auint)(rrpge_m_edat->stat.ropd[0xD70U + (rrpge_m_offlw & 0xFU)]);
    }
   }else{                                 /* Repeating 32 word audio & dma range */
    rrpge_m_arlt = (auint)(rrpge_m_edat->stat.ropd[0xEC0U + (rrpge_m_offlw & 0x1FU)]);
   }
  }

 }else{                                   /* 0x8000 - 0x807F: Video pages */

  /* Check FIFO non-empty, and set halt if so */
  if ((rrpge_m_edat->stat.ropd[0xEC5U] & 1U) != 0U){
   rrpge_m_info.hlt |= RRPGE_HLT_GRAPHICS;
  }
  rrpge_m_info.ocy += 1U;                 /* +1 cycle for graphics access */
  /* Process read */
  rrpge_m_avlt = (auint)(rrpge_m_edat->stat.vram[((rrpge_m_offpg & 0x7F000U) + rrpge_m_offlw) >> 1]);
  rrpge_m_arlt = (rrpge_m_avlt << ((rrpge_m_offlw & 1U) << 4)) >> 16;

 }
}



/* Data RAM write operation for assisting Write accesses. Uses the Read /
** Write offsets. */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_data(auint val)
{
 auint  t0;
 auint  t1;
 rrpge_m_offpg = rrpge_m_info.wbk[rrpge_m_offhi]; /* Page of data ( << 12 ) */

 /* Note: only sanity maskings are used to ensure accessing unused areas won't
 ** crash the library. Unused banks shouldn't occur as the kernel should have
 ** errored out on the first place when the user attempted to set those. */

 if ((rrpge_m_offpg & 0x8000000U) == 0U){ /* 0x4000 - 0x41BF: Data pages
                                          ** 0x41C0: Read Only Process Descriptor
                                          ** 0x7FFF: Audio peripheral area */
  if (rrpge_m_offpg < 0x41C0000U){        /* Assume Data pages */
   rrpge_m_edat->stat.dram[(rrpge_m_offpg & 0x1FF000U) + rrpge_m_offlw] = (uint16)(val);
  }else if ((rrpge_m_offpg & 0x2000000U) != 0U){ /* User peripheral also accepts writes */
   if (rrpge_m_offlw < 0xE00U){           /* RAM pg. 0 shadow */
    rrpge_m_edat->stat.dram[rrpge_m_offlw] = (uint16)(val);
   }else if (rrpge_m_offlw < 0xF00U){     /* Repeating 16 word graphics range */
    if ((rrpge_m_offlw & 0xEU) != 0x0U){  /* Graphics FIFO 0xE00 and 0xE01 does not accept written data. */
     rrpge_m_edat->stat.ropd[0xD70U + (rrpge_m_offlw & 0xFU)] = (uint16)(val);
    }
    switch (rrpge_m_offlw & 0xFU){        /* Process triggers */
     case 0x01U: rrpge_m_vidfifost();                     break;
     case 0x03U: rrpge_m_info.ocy += rrpge_m_vidfifoop(); break;
     default:    break;
    }
   }else{                                 /* Repeating 32 word audio & dma range */
    if ((rrpge_m_offlw & 0x1EU) != 0xCU){ /* Neither Audio DMA clocks at 0xF0C and 0xF0D do not accept writes. */
     rrpge_m_edat->stat.ropd[0xEC0U + (rrpge_m_offlw & 0x1FU)] = (uint16)(val);
    }
    switch (rrpge_m_offlw & 0x1FU){       /* Process triggers */
     case 0x01U: rrpge_m_info.ocy += rrpge_m_dma_fill();  break;
     case 0x02U: rrpge_m_info.ocy += rrpge_m_dma_copy();  break;
     case 0x03U: rrpge_m_info.ocy += rrpge_m_dma_vram();  break;
     case 0x0BU: rrpge_m_edat->stat.ropd[0xECDU] = 0U;    break;
     case 0x1FU: rrpge_m_info.ocy += rrpge_m_mixerop();   break;
     default:    break;
    }
   }
  }                                       /* Other pages here (ROPD) can't accept writes */

 }else{                                   /* 0x8000 - 0x807F: Video pages */
  rrpge_m_info.ocy += 1U;                 /* +1 cycle for graphics access */
  t0 = (rrpge_m_offlw & 1U) ^ 1U;
  t1 = rrpge_m_edat->stat.ropd[0xEE4U + t0];
  t0 = t0 << 4;
  rrpge_m_edat->stat.vram[((rrpge_m_offpg & 0x7F000U) + rrpge_m_offlw) >> 1] =
    (rrpge_m_avlt & (~(t1 << t0))) | ((val & t1) << t0);
 }

}



/* Pre/Post operations on address registers by opcode. There are 15 routines
** contained within a table, the appropriate one has to be selected by the
** contents of the XM register (rrpge_m_info.xmh[0]). The lowest bits of the
** opcode (rrpge_m_info.opc) are used to select the register. Fills only
** rrpge_m_offlw, if page is needed (data RAM), it must be done after the
** return of this. Increments rrpge_m_info.ocy for pre-decrements. */

/* 0000: 8bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_0(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 t0 = (rrpge_m_info.xr[t0 + 4U] & 0xFFFFU) +               /* Offset base */
      (((rrpge_m_info.xmh[1] >> (t0 << 2)) & 0x1U) << 16); /* Offset high */
 rrpge_m_adtsh = ((t0 & 0x1U) ^ 0x1U) << 3;
 rrpge_m_adtms = 0x00FFU;
 rrpge_m_offlw = t0 >> 1;
}

/* 0001: 4bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_1(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 t0 = (rrpge_m_info.xr[t0 + 4U] & 0xFFFFU) +               /* Offset base */
      (((rrpge_m_info.xmh[1] >> (t0 << 2)) & 0x3U) << 16); /* Offset high */
 rrpge_m_adtsh = ((t0 & 0x3U) ^ 0x3U) << 2;
 rrpge_m_adtms = 0x000FU;
 rrpge_m_offlw = t0 >> 2;
}

/* 0010: 2bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_2(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 t0 = (rrpge_m_info.xr[t0 + 4U] & 0xFFFFU) +               /* Offset base */
      (((rrpge_m_info.xmh[1] >> (t0 << 2)) & 0x7U) << 16); /* Offset high */
 rrpge_m_adtsh = ((t0 & 0x7U) ^ 0x7U) << 1;
 rrpge_m_adtms = 0x0003U;
 rrpge_m_offlw = t0 >> 3;
}

/* 0011: 1bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_3(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 t0 = (rrpge_m_info.xr[t0 + 4U] & 0xFFFFU) +               /* Offset base */
      (((rrpge_m_info.xmh[1] >> (t0 << 2)) & 0xFU) << 16); /* Offset high */
 rrpge_m_adtsh = ((t0 & 0xFU) ^ 0xFU);
 rrpge_m_adtms = 0x0001U;
 rrpge_m_offlw = t0 >> 4;
}

/* 010-: 16bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_4(void)
{
 rrpge_m_adtsh = 0U;
 rrpge_m_adtms = 0xFFFFU;
 rrpge_m_offlw = rrpge_m_info.xr[(rrpge_m_info.opc & 0x3U) + 4U] & 0xFFFFU;
}

/* 0110: 16bit post-incrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_6(void)
{
 auint t0 = (rrpge_m_info.opc & 0x3U) + 4U;
 rrpge_m_adtsh = 0U;
 rrpge_m_adtms = 0xFFFFU;
 rrpge_m_offlw = rrpge_m_info.xr[t0] & 0xFFFFU;
 rrpge_m_info.xr[t0]++;
}

/* 0111: 16bit pre-decrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_7(void)
{
 auint t0 = (rrpge_m_info.opc & 0x3U) + 4U;
 rrpge_m_adtsh = 0U;
 rrpge_m_adtms = 0xFFFFU;
 rrpge_m_info.xr[t0]--;
 rrpge_m_offlw = rrpge_m_info.xr[t0] & 0xFFFFU;
 rrpge_m_info.ocy++;
}

/* 1000: 8bit post-incrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_8(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 auint t1 = t0 << 2; /* 0, 4, 8 or 12, shift amount for xmh */
 auint t2 = t0 + 4U; /* 3 - 7 selecting X0 - X3 */
 t0 = (rrpge_m_info.xr[t2] & 0xFFFFU) +             /* Offset base */
      (((rrpge_m_info.xmh[1] >> t1) & 0x1U) << 16); /* Offset high */
 rrpge_m_adtsh = ((t0 & 1U) ^ 1U) << 3;
 rrpge_m_adtms = 0x00FFU;
 rrpge_m_offlw = t0 >> 1;
 t0++;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x0001U << t1))) |
                       ((t0 & 0x10000U) >> (16 - t1));
}

/* 1001: 4bit post-incrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_9(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 auint t1 = t0 << 2; /* 0, 4, 8 or 12, shift amount for xmh */
 auint t2 = t0 + 4U; /* 3 - 7 selecting X0 - X3 */
 t0 = (rrpge_m_info.xr[t2] & 0xFFFFU) +             /* Offset base */
      (((rrpge_m_info.xmh[1] >> t1) & 0x3U) << 16); /* Offset high */
 rrpge_m_adtsh = ((t0 & 0x3U) ^ 0x3U) << 2;
 rrpge_m_adtms = 0x000FU;
 rrpge_m_offlw = t0 >> 2;
 t0++;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x0003U << t1))) |
                       ((t0 & 0x30000U) >> (16 - t1));
}

/* 1010: 2bit post-incrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_a(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 auint t1 = t0 << 2; /* 0, 4, 8 or 12, shift amount for xmh */
 auint t2 = t0 + 4U; /* 3 - 7 selecting X0 - X3 */
 t0 = (rrpge_m_info.xr[t2] & 0xFFFFU) +             /* Offset base */
      (((rrpge_m_info.xmh[1] >> t1) & 0x7U) << 16); /* Offset high */
 rrpge_m_adtsh = ((t0 & 0x7U) ^ 0x7U) << 1;
 rrpge_m_adtms = 0x0003U;
 rrpge_m_offlw = t0 >> 3;
 t0++;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x0007U << t1))) |
                       ((t0 & 0x70000U) >> (16 - t1));
}

/* 1011: 1bit post-incrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_b(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 auint t1 = t0 << 2; /* 0, 4, 8 or 12, shift amount for xmh */
 auint t2 = t0 + 4U; /* 3 - 7 selecting X0 - X3 */
 t0 = (rrpge_m_info.xr[t2] & 0xFFFFU) +             /* Offset base */
      (((rrpge_m_info.xmh[1] >> t1) & 0xFU) << 16); /* Offset high */
 rrpge_m_adtsh = ((t0 & 0xFU) ^ 0xFU);
 rrpge_m_adtms = 0x0001U;
 rrpge_m_offlw = t0 >> 4;
 t0++;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x000FU << t1))) |
                       ((t0 & 0xF0000U) >> (16 - t1));
}

/* 1100: 8bit pre-decrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_c(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 auint t1 = t0 << 2; /* 0, 4, 8 or 12, shift amount for xmh */
 auint t2 = t0 + 4U; /* 3 - 7 selecting X0 - X3 */
 t0 = (rrpge_m_info.xr[t2] & 0xFFFFU) +             /* Offset base */
      (((rrpge_m_info.xmh[1] >> t1) & 0x1U) << 16); /* Offset high */
 t0--;
 rrpge_m_adtsh = ((t0 & 1U) ^ 1U) << 3;
 rrpge_m_adtms = 0x00FFU;
 rrpge_m_offlw = t0 >> 1;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x0001U << t1))) |
                       ((t0 & 0x10000U) >> (16 - t1));
 rrpge_m_info.ocy++;
}

/* 1101: 4bit pre-decrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_d(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 auint t1 = t0 << 2; /* 0, 4, 8 or 12, shift amount for xmh */
 auint t2 = t0 + 4U; /* 3 - 7 selecting X0 - X3 */
 t0 = (rrpge_m_info.xr[t2] & 0xFFFFU) +             /* Offset base */
      (((rrpge_m_info.xmh[1] >> t1) & 0x3U) << 16); /* Offset high */
 t0--;
 rrpge_m_adtsh = ((t0 & 0x3U) ^ 0x3U) << 2;
 rrpge_m_adtms = 0x000FU;
 rrpge_m_offlw = t0 >> 2;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x0003U << t1))) |
                       ((t0 & 0x30000U) >> (16 - t1));
 rrpge_m_info.ocy++;
}

/* 1110: 2bit pre-decrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_e(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 auint t1 = t0 << 2; /* 0, 4, 8 or 12, shift amount for xmh */
 auint t2 = t0 + 4U; /* 3 - 7 selecting X0 - X3 */
 t0 = (rrpge_m_info.xr[t2] & 0xFFFFU) +             /* Offset base */
      (((rrpge_m_info.xmh[1] >> t1) & 0x7U) << 16); /* Offset high */
 t0--;
 rrpge_m_adtsh = ((t0 & 0x7U) ^ 0x7U) << 1;
 rrpge_m_adtms = 0x0003U;
 rrpge_m_offlw = t0 >> 3;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x0007U << t1))) |
                       ((t0 & 0x70000U) >> (16 - t1));
 rrpge_m_info.ocy++;
}

/* 1111: 1bit pre-decrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_f(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 auint t1 = t0 << 2; /* 0, 4, 8 or 12, shift amount for xmh */
 auint t2 = t0 + 4U; /* 3 - 7 selecting X0 - X3 */
 t0 = (rrpge_m_info.xr[t2] & 0xFFFFU) +             /* Offset base */
      (((rrpge_m_info.xmh[1] >> t1) & 0xFU) << 16); /* Offset high */
 t0--;
 rrpge_m_adtsh = ((t0 & 0xFU) ^ 0xFU);
 rrpge_m_adtms = 0x0001U;
 rrpge_m_offlw = t0 >> 4;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x000FU << t1))) |
                       ((t0 & 0xF0000U) >> (16 - t1));
 rrpge_m_info.ocy++;
}

/* Table of pointer ops */
static rrpge_m_xop_t* const rrpge_m_xop_table[16U]={
 &rrpge_m_addr_xop_0, &rrpge_m_addr_xop_1, &rrpge_m_addr_xop_2, &rrpge_m_addr_xop_3,
 &rrpge_m_addr_xop_4, &rrpge_m_addr_xop_4, &rrpge_m_addr_xop_6, &rrpge_m_addr_xop_7,
 &rrpge_m_addr_xop_8, &rrpge_m_addr_xop_9, &rrpge_m_addr_xop_a, &rrpge_m_addr_xop_b,
 &rrpge_m_addr_xop_c, &rrpge_m_addr_xop_d, &rrpge_m_addr_xop_e, &rrpge_m_addr_xop_f
};




/* Addressing mode process functions. These are selected from a 16 element
** table based on bits 2-5 inclusive of the opcode */

/* Write functions */

/* 00--: imm4 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_i4(auint val){}

/* 01--: Stack: BP + imm4 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_si4(auint val)
{
 if (rrpge_m_info.hlt==0){ /* There was no error before (in read) */
  rrpge_m_edat->stat.sram[rrpge_m_offlw] = (uint16)(val);
 }
}

/* 100-: imm16 */
#define rrpge_m_addr_wr_i16  rrpge_m_addr_wr_i4

/* 1010: Data: imm16 */
#define rrpge_m_addr_wr_di16 rrpge_m_addr_wr_data

/* 1011: Stack: BP + imm16 */
#define rrpge_m_addr_wr_si16 rrpge_m_addr_wr_si4

/* 110-: xr */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_xr(auint val)
{
 rrpge_m_info.xr[rrpge_m_info.opc & 0x7U] = val;
}

/* 1110: Data: x16 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_dx16(auint val)
{
 rrpge_m_addr_wr_data( ((val & rrpge_m_adtms) << rrpge_m_adtsh) |
                       (rrpge_m_arlt & (~(rrpge_m_adtms << rrpge_m_adtsh))) );
}

/* 1111: Stack: BP + x16 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_sx16(auint val)
{
 val = ((val & rrpge_m_adtms) << rrpge_m_adtsh) |
       (rrpge_m_arlt & (~(rrpge_m_adtms << rrpge_m_adtsh)));
 if (rrpge_m_info.hlt==0){ /* There was no error before (in read) */
  rrpge_m_edat->stat.sram[rrpge_m_offlw] = (uint16)(val);
 }
}


/* Read functions */

/* 00--: imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_i4(void)
{
 rrpge_m_addr_write = rrpge_m_addr_wr_i4;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 return (rrpge_m_info.opc & 0xFU);
}

/* 01--: Stack: BP + imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_si4(void)
{
 rrpge_m_addr_write = rrpge_m_addr_wr_si4;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 1U;
 rrpge_m_offlw = ((rrpge_m_info.opc & 0xFU) + rrpge_m_info.bp) & 0xFFFFU;
 if ( (rrpge_m_offlw < 0x8000U) && /* Stack top limit */
      (rrpge_m_offlw >= STK_B) ){  /* Stack bottom limit (may pop a warning since STK_B is zero, and the comparison is unsigned) */
  return (auint)(rrpge_m_edat->stat.sram[rrpge_m_offlw]);
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
  return 0U;
 }
}

/* 100-: imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_i16(void)
{
 rrpge_m_addr_write = rrpge_m_addr_wr_i16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 2U;
 return ( ((rrpge_m_info.opc & 0x3U) << 14) +
          ((auint)(rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU]) & 0x3FFFU) );
}

/* 1010: Data: imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_di16(void)
{
 rrpge_m_addr_write = rrpge_m_addr_wr_di16;
 rrpge_m_info.ocy = 2U;
 rrpge_m_info.oaw = 2U;
 rrpge_m_offlw = ( ((rrpge_m_info.opc & 0x3U) << 14) +
                   ((auint)(rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU]) & 0x3FFFU)
                 ) & 0xFFFFU;
 rrpge_m_addr_rd_data();
 return rrpge_m_arlt;
}

/* 1011: Stack: BP + imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_si16(void)
{
 rrpge_m_addr_write = rrpge_m_addr_wr_si16;
 rrpge_m_info.ocy = 2U;
 rrpge_m_info.oaw = 2U;
 rrpge_m_offlw = ( ((rrpge_m_info.opc & 0x3U) << 14) +
                   ((auint)(rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU]) & 0x3FFFU) +
                   rrpge_m_info.bp ) & 0xFFFFU;
 if ( (rrpge_m_offlw < 0x8000U) && /* Stack top limit */
      (rrpge_m_offlw >= STK_B) ){  /* Stack bottom limit (may pop a warning since STK_B is zero, and the comparison is unsigned) */
  return (auint)(rrpge_m_edat->stat.sram[rrpge_m_offlw]);
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
  return 0U;
 }
}

/* 110-: xr */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_xr(void)
{
 rrpge_m_addr_write = rrpge_m_addr_wr_xr;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 return (rrpge_m_info.xr[rrpge_m_info.opc & 0x7U] & 0xFFFFU);
}

/* 1110: Data: x16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_dx16(void)
{
 rrpge_m_addr_write = rrpge_m_addr_wr_dx16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 1U;
 rrpge_m_xop_table[(rrpge_m_info.xmh[0] >> ((rrpge_m_info.opc & 0x3U) << 2)) & 0xFU]();
 rrpge_m_addr_rd_data();
 return (rrpge_m_arlt >> rrpge_m_adtsh) & rrpge_m_adtms;
}

/* 1111: Stack: BP + x16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_sx16(void)
{
 rrpge_m_addr_write = rrpge_m_addr_wr_sx16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 1U;
 rrpge_m_xop_table[(rrpge_m_info.xmh[0] >> ((rrpge_m_info.opc & 0x3U) << 2)) & 0xFU]();
 rrpge_m_offlw = (rrpge_m_offlw + rrpge_m_info.bp) & 0xFFFFU;
 if ( (rrpge_m_offlw < 0x8000U) && /* Stack top limit */
      (rrpge_m_offlw >= STK_B) ){  /* Stack bottom limit (may pop a warning since STK_B is zero, and the comparison is unsigned) */
  return ((auint)(rrpge_m_edat->stat.sram[rrpge_m_offlw]) >> rrpge_m_adtsh) & rrpge_m_adtms;
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
  return 0U;
 }
}



/* Addressing mode specific read, function table by opcode bits 0-5. Uses
** rrpge_m_info.opc for the addressing mode further on, assumes PC points here
** too for fetching the second opcode word as needed. Sets rrpge_m_info.ocy
** and rrpge_m_info.oaw (this latter to 1 or 2) depending on the requirements
** of the addressing operation. Only low 16 bits of the return value may be
** set. The result is returned in rrpge_m_info.rwt. */
rrpge_m_addr_read_t* const rrpge_m_addr_read_table[64] = {
 &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,
 &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,
 &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,
 &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,   &rrpge_m_addr_rd_i4,
 &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,
 &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,
 &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,
 &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,  &rrpge_m_addr_rd_si4,
 &rrpge_m_addr_rd_i16,  &rrpge_m_addr_rd_i16,  &rrpge_m_addr_rd_i16,  &rrpge_m_addr_rd_i16,
 &rrpge_m_addr_rd_i16,  &rrpge_m_addr_rd_i16,  &rrpge_m_addr_rd_i16,  &rrpge_m_addr_rd_i16,
 &rrpge_m_addr_rd_di16, &rrpge_m_addr_rd_di16, &rrpge_m_addr_rd_di16, &rrpge_m_addr_rd_di16,
 &rrpge_m_addr_rd_si16, &rrpge_m_addr_rd_si16, &rrpge_m_addr_rd_si16, &rrpge_m_addr_rd_si16,
 &rrpge_m_addr_rd_xr,   &rrpge_m_addr_rd_xr,   &rrpge_m_addr_rd_xr,   &rrpge_m_addr_rd_xr,
 &rrpge_m_addr_rd_xr,   &rrpge_m_addr_rd_xr,   &rrpge_m_addr_rd_xr,   &rrpge_m_addr_rd_xr,
 &rrpge_m_addr_rd_dx16, &rrpge_m_addr_rd_dx16, &rrpge_m_addr_rd_dx16, &rrpge_m_addr_rd_dx16,
 &rrpge_m_addr_rd_sx16, &rrpge_m_addr_rd_sx16, &rrpge_m_addr_rd_sx16, &rrpge_m_addr_rd_sx16
};



/* Pops off a value from the stack, sets halt cause if stack pointer is out of
** bounds (low). Returning to supervisor must be handled externally (this
** won't do that). Only low 16 bits of the return value may be set. */
RRPGE_M_FASTCALL auint rrpge_m_stk_pop(void)
{
 auint t0;
 rrpge_m_info.sp--;
 t0 = (rrpge_m_info.sp + rrpge_m_info.bp) & 0xFFFFU;
 if ( (t0 < 0x8000U) &&
      (t0 >= STK_B) ){  /* Stack bottom limit (may pop a warning since STK_B is zero, and the comparison is unsigned) */
  return (auint)(rrpge_m_edat->stat.sram[t0]);
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
  return 0U;
 }
}



/* Pushes a value on the stack, sets halt cause if stack pointer is out of
** bounds (high). */
RRPGE_M_FASTCALL void  rrpge_m_stk_push(auint val)
{
 auint t0;
 t0 = (rrpge_m_info.sp + rrpge_m_info.bp) & 0xFFFFU;
 if ( (t0 < 0x8000U) &&
      (t0 >= STK_B) ){  /* Stack bottom limit (may pop a warning since STK_B is zero, and the comparison is unsigned) */
  rrpge_m_edat->stat.sram[t0] = (uint16)(val);
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
 }
 rrpge_m_info.sp++;
}
