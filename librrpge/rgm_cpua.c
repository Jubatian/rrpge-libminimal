/**
**  \file
**  \brief     CPU addressing unit
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#include "rgm_cpua.h"
#include "rgm_fifo.h"
#include "rgm_pram.h"
#include "rgm_vid.h"



/* Data mask values by pointer mode */
static const uint16 rrpge_m_addr_dms[16] = {
 0x00FFU, 0x000FU, 0x0003U, 0x0001U, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU,
 0x00FFU, 0x000FU, 0x0003U, 0x0001U, 0x00FFU, 0x000FU, 0x0003U, 0x0001U};

/* Address shift values by pointer mode */
static const uint8  rrpge_m_addr_ads[16] = {
 1U, 2U, 3U, 4U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 1U, 2U, 3U, 4U};



/* Data RAM read operation for assisting Read accesses. Uses the Read / Write
** offsets, returns in rrpge_m_info.add (only low 16 bits may be set). The
** parameter is the R-M-W signal: nonzero for the read of a R-M-W access. */
RRPGE_M_FASTCALL static void rrpge_m_addr_rd_data(auint rmw)
{
 if (rrpge_m_info.ada >= 0x0040U){ /* Normal RAM access */

  rrpge_m_info.add = (rrpge_m_edat->st.dram[rrpge_m_info.ada]) & 0xFFFFU;

 }else{                            /* User Peripheral Area */

  if ((rrpge_m_info.ada & 0x20U) == 0U){
   rrpge_m_info.add = (rrpge_m_edat->st.stat[RRPGE_STA_UPA + rrpge_m_info.ada]) & 0xFFFFU;
  }else{
   rrpge_m_info.add = rrpge_m_pramread(rrpge_m_info.ada, rmw);
  }

 }
}



/* Data RAM write operation for assisting Write accesses. Uses the Read /
** Write offsets. */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_data(auint val)
{
 if (rrpge_m_info.ada >= 0x0040U){ /* Normal RAM access */

  rrpge_m_edat->st.dram[rrpge_m_info.ada] = val & 0xFFFFU;

 }else{                            /* User Peripheral Area */

  switch (rrpge_m_info.ada & 0x3CU){

   case 0x00U:                     /* Writes ignored */
    break;

   case 0x04U:                     /* Audio writable regs: Writes proceed */
    rrpge_m_edat->st.stat[RRPGE_STA_UPA + rrpge_m_info.ada] = val & 0xFFFFU;
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



/* Write functions */

/* 00--: imm4 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_i4(auint val){}

/* 01--: Stack: BP + imm4 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_si4(auint val)
{
 if ((rrpge_m_info.hlt & RRPGE_HLT_STACK) == 0U){ /* There was no error before (in read) */
  rrpge_m_edat->st.dram[rrpge_m_info.ada] = val & 0xFFFFU;
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
 rrpge_m_addr_wr_data( ((val << rrpge_m_info.ads) & rrpge_m_info.adm) |
                       (rrpge_m_info.add & (~rrpge_m_info.adm)) );
}

/* 1111: Stack: BP + x16 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_sx16(auint val)
{
 val = ((val << rrpge_m_info.ads) & rrpge_m_info.adm) |
       (rrpge_m_info.add & (~rrpge_m_info.adm));
 if ((rrpge_m_info.hlt & RRPGE_HLT_STACK) == 0U){ /* There was no error before (in read) */
  rrpge_m_edat->st.dram[rrpge_m_info.ada] = val & 0xFFFFU;
 }
}



/* Read functions */

/* 00--: imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_i4(auint rmw)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_i4;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 return (rrpge_m_info.opc & 0xFU);
}

/* 01--: Stack: BP + imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_si4(auint rmw)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_si4;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 rrpge_m_info.ada = (((rrpge_m_info.opc & 0xFU) + rrpge_m_info.bp) & 0xFFFFU) |
                    (rrpge_m_info.sbt & (~0xFFFFU));
 if ( (rrpge_m_info.ada <  rrpge_m_info.stp) &&
      (rrpge_m_info.ada >= rrpge_m_info.sbt) ){
  return ((rrpge_m_edat->st.dram[rrpge_m_info.ada]) & 0xFFFFU);
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
  return 0U;
 }
}

/* 1000: imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_i16(auint rmw)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_i16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 2U;
 return ( ((rrpge_m_info.opc & 0x3U) << 14) +
          (rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU] & 0x3FFFU) );
}

/* 1001: BP + imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_bi16(auint rmw)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_bi16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 2U;
 return ( ( rrpge_m_info.bp +
            ((rrpge_m_info.opc & 0x3U) << 14) +
            (rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU] & 0x3FFFU) ) & 0xFFFFU);
}

/* 1010: Data: imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_di16(auint rmw)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_di16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 2U;
 rrpge_m_info.ada = ((rrpge_m_info.opc & 0x3U) << 14) +
                    (rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU] & 0x3FFFU);
 rrpge_m_addr_rd_data(rmw);
 return rrpge_m_info.add;
}

/* 1011: Stack: BP + imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_si16(auint rmw)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_si16;
 rrpge_m_info.ocy = 1U;
 rrpge_m_info.oaw = 2U;
 rrpge_m_info.ada = ( ( ((rrpge_m_info.opc & 0x3U) << 14) +
                        (rrpge_m_edat->crom[(rrpge_m_info.pc + 1U) & 0xFFFFU] & 0x3FFFU) +
                        rrpge_m_info.bp ) & 0xFFFFU) |
                    (rrpge_m_info.sbt & (~0xFFFFU));
 if ( (rrpge_m_info.ada <  rrpge_m_info.stp) &&
      (rrpge_m_info.ada >= rrpge_m_info.sbt) ){
  return ((rrpge_m_edat->st.dram[rrpge_m_info.ada]) & 0xFFFFU);
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
  return 0U;
 }
}

/* 110-: xr */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_xr(auint rmw)
{
 rrpge_m_info.awf = rrpge_m_addr_wr_xr;
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.oaw = 1U;
 return (rrpge_m_info.xr[rrpge_m_info.opc & 0x7U] & 0xFFFFU);
}

/* 1110: Data: x16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_dx16(auint rmw)
{
 auint s = rrpge_m_info.opc & 0x3U;            /* Pointer register select */
 auint t = (s << 2);                           /* 0, 4, 8 or 12, shift amount for xm & xh */
 auint m = (rrpge_m_info.xmh[0] >> t) & 0xFU;  /* Pointer mode */
 auint a;                                      /* Address */
 auint u;

 rrpge_m_info.awf = rrpge_m_addr_wr_dx16;
 rrpge_m_info.oaw = 1U;

 t  = 16U - t;                                 /* 16, 12, 8 or 4, shift amount for xh */
 a  = (rrpge_m_info.xr[s + 4U] & 0xFFFFU) +    /* Address base */
      ((rrpge_m_info.xmh[1] << t) & 0xF0000U); /* Address high */
 u  = rrpge_m_addr_ads[m];
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.ads = ((~a) << (4U - u)) & ((0xFF0FU >> (m & 0xCU)) & 0xFU);
 rrpge_m_info.adm = rrpge_m_addr_dms[m] << rrpge_m_info.ads;
 rrpge_m_info.ada = (a >> u) & 0xFFFFU;
 a += ( ((0x0F40U >> m) & 1U) |                /* 1 if post-incrementing ptr. mode */
        ((0xF080U >> m) & rmw) );              /* 1 if post-incrementing on write only mode & rmw set (it is 1) */
 rrpge_m_info.xr[s + 4U] = a;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (0xFFF0FFFFU >> t)) |
                       ((a & 0xF0000U) >> t);  /* Address write-back */

 rrpge_m_addr_rd_data(rmw);
 return (rrpge_m_info.add & rrpge_m_info.adm) >> rrpge_m_info.ads;
}

/* 1111: Stack: BP + x16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_sx16(auint rmw)
{
 auint s = rrpge_m_info.opc & 0x3U;            /* Pointer register select */
 auint t = (s << 2);                           /* 0, 4, 8 or 12, shift amount for xm & xh */
 auint m = (rrpge_m_info.xmh[0] >> t) & 0xFU;  /* Pointer mode */
 auint a;                                      /* Address */
 auint u;

 rrpge_m_info.awf = rrpge_m_addr_wr_sx16;
 rrpge_m_info.oaw = 1U;

 t  = 16U - t;                                 /* 16, 12, 8 or 4, shift amount for xh */
 a  = (rrpge_m_info.xr[s + 4U] & 0xFFFFU) +    /* Address base */
      ((rrpge_m_info.xmh[1] << t) & 0xF0000U); /* Address high */
 u  = rrpge_m_addr_ads[m];
 rrpge_m_info.ocy = 0U;
 rrpge_m_info.ads = ((~a) << (4U - u)) & ((0xFF0FU >> (m & 0xCU)) & 0xFU);
 rrpge_m_info.adm = rrpge_m_addr_dms[m] << rrpge_m_info.ads;
 rrpge_m_info.ada = a >> u;
 a += ( ((0x0F40U >> m) & 1U) |                /* 1 if post-incrementing ptr. mode */
        ((0xF080U >> m) & rmw) );              /* 1 if post-incrementing on write only mode & rmw set (it is 1) */
 rrpge_m_info.xr[s + 4U] = a;
 rrpge_m_info.xmh[1] = (rrpge_m_info.xmh[1] & (0xFFF0FFFFU >> t)) |
                       ((a & 0xF0000U) >> t);  /* Address write-back */

 rrpge_m_info.ada = ((rrpge_m_info.ada + rrpge_m_info.bp) & 0xFFFFU) |
                    (rrpge_m_info.sbt & (~0xFFFFU));
 if ( (rrpge_m_info.ada <  rrpge_m_info.stp) &&
      (rrpge_m_info.ada >= rrpge_m_info.sbt) ){
  return ((rrpge_m_edat->st.dram[rrpge_m_info.ada]) & rrpge_m_info.adm) >> rrpge_m_info.ads;
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
  return ((rrpge_m_edat->st.dram[t0]) & 0xFFFFU);
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
  rrpge_m_edat->st.dram[t0] = val & 0xFFFFU;
 }else{
  rrpge_m_info.hlt |= RRPGE_HLT_STACK;
 }
 rrpge_m_info.sp++;
}
