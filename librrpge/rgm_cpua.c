/**
**  \file
**  \brief     CPU addressing unit
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.24
*/


#include "rgm_cpua.h"
#include "rgm_fifo.h"
#include "rgm_pram.h"
#include "rgm_vid.h"



/* Pointer op. function type for the pointer op. table */
typedef RRPGE_M_FASTCALL void (rrpge_m_xop_t)(void);



/* Data RAM read operation for assisting Read accesses. Uses the Read / Write
** offsets, returns in rrpge_m_info.add (only low 16 bits may be set). */
RRPGE_M_FASTCALL static void rrpge_m_addr_rd_data(void)
{
 if (rrpge_m_info.ada >= 0x0040U){ /* Normal RAM access */

  rrpge_m_info.add = (auint)(rrpge_m_edat->st.dram[rrpge_m_info.ada]);

 }else{                            /* User Peripheral Area */

  if ((rrpge_m_info.ada & 0x20U) == 0U){
   rrpge_m_info.add = (auint)(rrpge_m_edat->st.stat[RRPGE_STA_UPA + rrpge_m_info.ada]);
  }else{
   rrpge_m_info.add = rrpge_m_pramread(rrpge_m_info.ada);
  }

 }
}



/* Data RAM write operation for assisting Write accesses. Uses the Read /
** Write offsets. */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_data(auint val)
{
 if (rrpge_m_info.ada >= 0x0040U){ /* Normal RAM access */

  rrpge_m_edat->st.dram[rrpge_m_info.ada] = (uint16)(val);

 }else{                            /* User Peripheral Area */

  switch (rrpge_m_info.ada & 0x3CU){

   case 0x00U:                     /* Writes ignored */
    break;

   case 0x04U:                     /* Audio writable regs: Writes proceed */
    rrpge_m_edat->st.stat[RRPGE_STA_UPA + rrpge_m_info.ada] = (uint16)(val);
    break;

   case 0x08U:
   case 0x0CU:                     /* FIFO */
    rrpge_m_fifowrite(rrpge_m_info.ada, val);
    break;

   case 0x10U:
   case 0x14U:
   case 0x18U:
   case 0x1CU:                     /* Graphics */
    rrpge_m_vidwrite(rrpge_m_info.ada, val);
    break;

   default:                        /* PRAM interface */
    rrpge_m_pramwrite(rrpge_m_info.ada, val);
    break;

  }

 }
}



/* Pre/Post operations on address registers by opcode. There are 15 routines
** contained within a table, the appropriate one has to be selected by the
** contents of the XM register (rrpge_m_info.xmh[0]). The lowest bits of the
** opcode (rrpge_m_info.opc) are used to select the register. Fills up address
** (rrpge_m_info.ada) and shift / mask (rrpge_m_info.ads, rrpge_m_info.adm),
** along with performing the necessary addressing operations. The address will
** only have bits of the low 16 set. Sets rrpge_m_info.ocy one for
** pre-decrements. */

/* 0000: 8bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_0(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 t0 = (rrpge_m_info.xr[t0 + 4U] & 0xFFFFU) +               /* Offset base */
      (((rrpge_m_info.xmh[1] >> (t0 << 2)) & 0x1U) << 16); /* Offset high */
 rrpge_m_info.ads = ((t0 & 0x1U) ^ 0x1U) << 3;
 rrpge_m_info.adm = 0x00FFU << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 1;
}

/* 0001: 4bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_1(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 t0 = (rrpge_m_info.xr[t0 + 4U] & 0xFFFFU) +               /* Offset base */
      (((rrpge_m_info.xmh[1] >> (t0 << 2)) & 0x3U) << 16); /* Offset high */
 rrpge_m_info.ads = ((t0 & 0x3U) ^ 0x3U) << 2;
 rrpge_m_info.adm = 0x000FU << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 2;
}

/* 0010: 2bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_2(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 t0 = (rrpge_m_info.xr[t0 + 4U] & 0xFFFFU) +               /* Offset base */
      (((rrpge_m_info.xmh[1] >> (t0 << 2)) & 0x7U) << 16); /* Offset high */
 rrpge_m_info.ads = ((t0 & 0x7U) ^ 0x7U) << 1;
 rrpge_m_info.adm = 0x0003U << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 3;
}

/* 0011: 1bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_3(void)
{
 auint t0 = rrpge_m_info.opc & 0x3U;
 t0 = (rrpge_m_info.xr[t0 + 4U] & 0xFFFFU) +               /* Offset base */
      (((rrpge_m_info.xmh[1] >> (t0 << 2)) & 0xFU) << 16); /* Offset high */
 rrpge_m_info.ads = ((t0 & 0xFU) ^ 0xFU);
 rrpge_m_info.adm = 0x0001U << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 4;
}

/* 010-: 16bit stationary */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_4(void)
{
 rrpge_m_info.ads = 0U;
 rrpge_m_info.adm = 0xFFFFU;
 rrpge_m_info.ada = rrpge_m_info.xr[(rrpge_m_info.opc & 0x3U) + 4U] & 0xFFFFU;
}

/* 0110: 16bit post-incrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_6(void)
{
 auint t0 = (rrpge_m_info.opc & 0x3U) + 4U;
 rrpge_m_info.ads = 0U;
 rrpge_m_info.adm = 0xFFFFU;
 rrpge_m_info.ada = rrpge_m_info.xr[t0] & 0xFFFFU;
 rrpge_m_info.xr[t0]++;
}

/* 0111: 16bit pre-decrementing */
RRPGE_M_FASTCALL static void rrpge_m_addr_xop_7(void)
{
 auint t0 = (rrpge_m_info.opc & 0x3U) + 4U;
 rrpge_m_info.ads = 0U;
 rrpge_m_info.adm = 0xFFFFU;
 rrpge_m_info.xr[t0]--;
 rrpge_m_info.ada = rrpge_m_info.xr[t0] & 0xFFFFU;
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
 rrpge_m_info.ads = ((t0 & 1U) ^ 1U) << 3;
 rrpge_m_info.adm = 0x00FFU << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 1;
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
 rrpge_m_info.ads = ((t0 & 0x3U) ^ 0x3U) << 2;
 rrpge_m_info.adm = 0x000FU << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 2;
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
 rrpge_m_info.ads = ((t0 & 0x7U) ^ 0x7U) << 1;
 rrpge_m_info.adm = 0x0003U << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 3;
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
 rrpge_m_info.ads = ((t0 & 0xFU) ^ 0xFU);
 rrpge_m_info.adm = 0x0001U << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 4;
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
 rrpge_m_info.ads = ((t0 & 1U) ^ 1U) << 3;
 rrpge_m_info.adm = 0x00FFU << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 1;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x0001U << t1))) |
                       ((t0 & 0x10000U) >> (16 - t1));
 rrpge_m_info.ocy = 1U;
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
 rrpge_m_info.ads = ((t0 & 0x3U) ^ 0x3U) << 2;
 rrpge_m_info.adm = 0x000FU << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 2;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x0003U << t1))) |
                       ((t0 & 0x30000U) >> (16 - t1));
 rrpge_m_info.ocy = 1U;
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
 rrpge_m_info.ads = ((t0 & 0x7U) ^ 0x7U) << 1;
 rrpge_m_info.adm = 0x0003U << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 3;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x0007U << t1))) |
                       ((t0 & 0x70000U) >> (16 - t1));
 rrpge_m_info.ocy = 1U;
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
 rrpge_m_info.ads = ((t0 & 0xFU) ^ 0xFU);
 rrpge_m_info.adm = 0x0001U << rrpge_m_info.ads;
 rrpge_m_info.ada = t0 >> 4;
 rrpge_m_info.xr[t2] = t0;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (~(0x000FU << t1))) |
                       ((t0 & 0xF0000U) >> (16 - t1));
 rrpge_m_info.ocy = 1U;
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
 if ((rrpge_m_info.hlt & RRPGE_HLT_STACK) == 0U){ /* There was no error before (in read) */
  rrpge_m_edat->st.dram[rrpge_m_info.ada] = (uint16)(val);
 }
}

/* 1000: imm16 */
#define rrpge_m_addr_wr_i16  rrpge_m_addr_wr_i4

/* 1001: BP + imm16 */
#define rrpge_m_addr_wr_bi16 rrpge_m_addr_wr_i4

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
 rrpge_m_addr_wr_data( ((val & << rrpge_m_info.ads) & rrpge_m_info.adm) |
                       (rrpge_m_info.add & (~rrpge_m_info.adm)) );
}

/* 1111: Stack: BP + x16 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_sx16(auint val)
{
 val = ((val & << rrpge_m_info.ads) & rrpge_m_info.adm) |
       (rrpge_m_info.add & (~rrpge_m_info.adm));
 if ((rrpge_m_info.hlt & RRPGE_HLT_STACK) == 0U){ /* There was no error before (in read) */
  rrpge_m_edat->st.dram[rrpge_m_info.ada] = (uint16)(val);
 }
}


/* Read functions */

/* 00--: imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_i4(void)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_i4;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 return (rrpge_m_info.opc & 0xFU);
}

/* 01--: Stack: BP + imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_si4(void)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_si4;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 rrpge_m_info.ada = (((rrpge_m_info.opc & 0xFU) + rrpge_m_info.bp) & 0xFFFFU) |
                    (rrpge_m_info.sbt & (~0xFFFFU));
 if ( (rrpge_m_info.ada <  rrpge_m_info.stp) &&
      (rrpge_m_info.ada >= rrpge_m_info.sbt) ){
  return (auint)(rrpge_m_edat->st.dram[rrpge_m_info.ada]);
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
  return 0U;
 }
}

/* 1000: imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_i16(void)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_i16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 2U;
 return ( ((rrpge_m_info.opc & 0x3U) << 14) +
          ((auint)(rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU]) & 0x3FFFU) );
}

/* 1001: BP + imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_bi16(void)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_bi16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 2U;
 return ( ( rrpge_m_info.bp +
            ((rrpge_m_info.opc & 0x3U) << 14) +
            ((auint)(rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU]) & 0x3FFFU) ) & 0xFFFFU);
}

/* 1010: Data: imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_di16(void)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_di16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 2U;
 rrpge_m_info.ada = ((rrpge_m_info.opc & 0x3U) << 14) +
                    ((auint)(rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU]) & 0x3FFFU);
 rrpge_m_addr_rd_data();
 return rrpge_m_info.add;
}

/* 1011: Stack: BP + imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_si16(void)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_si16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 2U;
 rrpge_m_info.ada = ( ( ((rrpge_m_info.opc & 0x3U) << 14) +
                        ((auint)(rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU]) & 0x3FFFU) +
                        rrpge_m_info.bp ) & 0xFFFFU) |
                    (rrpge_m_info.sbt & (~0xFFFFU));
 if ( (rrpge_m_info.ada <  rrpge_m_info.stp) &&
      (rrpge_m_info.ada >= rrpge_m_info.sbt) ){
  return (auint)(rrpge_m_edat->st.dram[rrpge_m_info.ada]);
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
  return 0U;
 }
}

/* 110-: xr */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_xr(void)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_xr;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 return (rrpge_m_info.xr[rrpge_m_info.opc & 0x7U] & 0xFFFFU);
}

/* 1110: Data: x16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_dx16(void)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_dx16;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 rrpge_m_xop_table[(rrpge_m_info.xmh[0] >> ((rrpge_m_info.opc & 0x3U) << 2)) & 0xFU]();
 rrpge_m_addr_rd_data();
 return (rrpge_m_info.add & rrpge_m_info.adm) >> rrpge_m_info.ads;
}

/* 1111: Stack: BP + x16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_sx16(void)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_sx16;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 rrpge_m_xop_table[(rrpge_m_info.xmh[0] >> ((rrpge_m_info.opc & 0x3U) << 2)) & 0xFU]();
 rrpge_m_info.ada = ((rrpge_m_info.ada + rrpge_m_info.bp) & 0xFFFFU) |
                    (rrpge_m_info.sbt & (~0xFFFFU));
 if ( (rrpge_m_info.ada <  rrpge_m_info.stp) &&
      (rrpge_m_info.ada >= rrpge_m_info.sbt) ){
  return ((auint)(rrpge_m_edat->st.dram[rrpge_m_info.ada]) & rrpge_m_info.adm) >> rrpge_m_info.ads;
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
** set. */
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
 &rrpge_m_addr_rd_bi16, &rrpge_m_addr_rd_bi16, &rrpge_m_addr_rd_bi16, &rrpge_m_addr_rd_bi16,
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
 t0 = ((rrpge_m_info.sp + rrpge_m_info.bp) & 0xFFFFU) | (rrpge_m_info.sbt & (~0xFFFFU));
 if ( (t0 <  rrpge_m_info.stp) &&
      (t0 >= rrpge_m_info.sbt) ){
  return (auint)(rrpge_m_edat->st.dram[t0]);
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
 t0 = ((rrpge_m_info.sp + rrpge_m_info.bp) & 0xFFFFU) | (rrpge_m_info.sbt & (~0xFFFFU));
 if ( (t0 <  rrpge_m_info.stp) &&
      (t0 >= rrpge_m_info.sbt) ){
  rrpge_m_edat->st.dram[t0] = (uint16)(val);
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
 }
 rrpge_m_info.sp++;
}
