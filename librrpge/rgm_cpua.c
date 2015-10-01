/**
**  \file
**  \brief     CPU addressing unit
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.16
*/


#include "rgm_cpua.h"
#include "rgm_fifo.h"
#include "rgm_pram.h"
#include "rgm_vid.h"
#include "rgm_halt.h"
#include "rgm_stat.h"



/* Data mask values by pointer mode */
static const uint16 rrpge_m_addr_dms[16] = {
 0x00FFU, 0x000FU, 0x0003U, 0x0001U, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU,
 0x00FFU, 0x000FU, 0x0003U, 0x0001U, 0x00FFU, 0x000FU, 0x0003U, 0x0001U};

/* Address fractional bit mask values by pointer mode */
static const uint8  rrpge_m_addr_ams[16] = {
 0x8U, 0xCU, 0xEU, 0xFU, 0x0U, 0x0U, 0x0U, 0x0U,
 0x8U, 0xCU, 0xEU, 0xFU, 0x8U, 0xCU, 0xEU, 0xFU};

/* Address add shift values (to increment bit address) */
static const uint8  rrpge_m_addr_ash[16] = {
 3U, 2U, 1U, 0U, 4U, 4U, 4U, 4U, 3U, 2U, 1U, 0U, 3U, 2U, 1U, 0U};



/* Data RAM read operation for assisting Read accesses. Uses the Read / Write
** offsets, returns in hnd->cpu.add (only low 16 bits may be set). The
** parameter is the R-M-W signal: nonzero for the read of a R-M-W access. */
RRPGE_M_FASTCALL static void rrpge_m_addr_rd_data(rrpge_object_t* hnd, auint rmw)
{
 if (hnd->cpu.ada >= 0x0040U){ /* Normal RAM access */

  hnd->cpu.add = (hnd->st.dram[hnd->cpu.ada]) & 0xFFFFU;

 }else{                        /* User Peripheral Area */

  /* PRAM access read stalls are generated here (1 cycle for any PRAM access)
  ** since it is not possible for the pram component to signal this back. */

  hnd->cpu.add = rrpge_m_stat_read(hnd, RRPGE_STA_UPA + hnd->cpu.ada, rmw);
  if ((hnd->cpu.ada & 0x26U) == 0x26U){ hnd->cpu.ocy ++; }

 }
}



/* Data RAM write operation for assisting Write accesses. Uses the Read /
** Write offsets. */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_data(rrpge_object_t* hnd, auint val)
{
 if (hnd->cpu.ada >= 0x0040U){ /* Normal RAM access */

  hnd->st.dram[hnd->cpu.ada] = val & 0xFFFFU;

 }else{                        /* User Peripheral Area */

  /* !!! This range has to be written by rrpge_m_stat_set() once the targets
  ** implement it properly */

  switch (hnd->cpu.ada & 0x3CU){

   case 0x08U:
   case 0x0CU:                     /* FIFO */
    rrpge_m_fifowrite(hnd->cpu.ada, val);
    break;

   default:                        /* Audio, Graphics & PRAM interface */
    rrpge_m_stat_write(hnd, RRPGE_STA_UPA + hnd->cpu.ada, val);
    break;

  }

 }
}



/* Write functions */

/* 00--: imm4 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_i4(rrpge_object_t* hnd, auint val){}

/* 01--: Stack: BP + imm4 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_si4(rrpge_object_t* hnd, auint val)
{
 if (!rrpge_m_halt_isset(hnd, RRPGE_HLT_STACK)){ /* There was no error before (in read) */
  hnd->st.dram[hnd->cpu.ada] = val & 0xFFFFU;
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
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_xr(rrpge_object_t* hnd, auint val)
{
 hnd->cpu.xr[hnd->cpu.opc & 0x7U] = val;
}

/* 1110: Data: x16 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_dx16(rrpge_object_t* hnd, auint val)
{
 rrpge_m_addr_wr_data(hnd, ((val << hnd->cpu.ads) & hnd->cpu.adm) |
                           (hnd->cpu.add & (~hnd->cpu.adm)) );
}

/* 1111: Stack: BP + x16 */
RRPGE_M_FASTCALL static void rrpge_m_addr_wr_sx16(rrpge_object_t* hnd, auint val)
{
 val = ((val << hnd->cpu.ads) & hnd->cpu.adm) |
       (hnd->cpu.add & (~hnd->cpu.adm));
 if (!rrpge_m_halt_isset(hnd, RRPGE_HLT_STACK)){ /* There was no error before (in read) */
  hnd->st.dram[hnd->cpu.ada] = val & 0xFFFFU;
 }
}



/* Read functions */

/* 00--: imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_i4(rrpge_object_t* hnd, auint rmw)
{
 hnd->cpu.awf = rrpge_m_addr_wr_i4;
 hnd->cpu.ocy = 0U;
 hnd->cpu.oaw = 1U;
 return (hnd->cpu.opc & 0xFU);
}

/* 01--: Stack: BP + imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_si4(rrpge_object_t* hnd, auint rmw)
{
 hnd->cpu.awf = rrpge_m_addr_wr_si4;
 hnd->cpu.ocy = 1U;
 hnd->cpu.oaw = 1U;
 hnd->cpu.ada = (((hnd->cpu.opc & 0xFU) + hnd->cpu.bp) & 0xFFFFU) |
                (hnd->cpu.sbt & (~0xFFFFU));
 if ( (hnd->cpu.ada <  hnd->cpu.stp) &&
      (hnd->cpu.ada >= hnd->cpu.sbt) ){
  return ((hnd->st.dram[hnd->cpu.ada]) & 0xFFFFU);
 }else{
  rrpge_m_halt_set(hnd, RRPGE_HLT_STACK);
  return 0U;
 }
}

/* 1000: imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_i16(rrpge_object_t* hnd, auint rmw)
{
 hnd->cpu.awf = rrpge_m_addr_wr_i16;
 hnd->cpu.ocy = 1U;
 hnd->cpu.oaw = 2U;
 return ( ((hnd->cpu.opc & 0x3U) << 14) +
          (hnd->crom[(hnd->cpu.pc + 1U) & 0xFFFFU] & 0x3FFFU) );
}

/* 1001: BP + imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_bi16(rrpge_object_t* hnd, auint rmw)
{
 hnd->cpu.awf = rrpge_m_addr_wr_bi16;
 hnd->cpu.ocy = 1U;
 hnd->cpu.oaw = 2U;
 return ( ( hnd->cpu.bp +
            ((hnd->cpu.opc & 0x3U) << 14) +
            (hnd->crom[(hnd->cpu.pc + 1U) & 0xFFFFU] & 0x3FFFU) ) & 0xFFFFU);
}

/* 1010: Data: imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_di16(rrpge_object_t* hnd, auint rmw)
{
 hnd->cpu.awf = rrpge_m_addr_wr_di16;
 hnd->cpu.ocy = 2U;
 hnd->cpu.oaw = 2U;
 hnd->cpu.ada = ((hnd->cpu.opc & 0x3U) << 14) +
                (hnd->crom[(hnd->cpu.pc + 1U) & 0xFFFFU] & 0x3FFFU);
 rrpge_m_addr_rd_data(hnd, rmw);
 return hnd->cpu.add;
}

/* 1011: Stack: BP + imm16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_si16(rrpge_object_t* hnd, auint rmw)
{
 hnd->cpu.awf = rrpge_m_addr_wr_si16;
 hnd->cpu.ocy = 2U;
 hnd->cpu.oaw = 2U;
 hnd->cpu.ada = ( ( ((hnd->cpu.opc & 0x3U) << 14) +
                    (hnd->crom[(hnd->cpu.pc + 1U) & 0xFFFFU] & 0x3FFFU) +
                    hnd->cpu.bp ) & 0xFFFFU) |
                (hnd->cpu.sbt & (~0xFFFFU));
 if ( (hnd->cpu.ada <  hnd->cpu.stp) &&
      (hnd->cpu.ada >= hnd->cpu.sbt) ){
  return ((hnd->st.dram[hnd->cpu.ada]) & 0xFFFFU);
 }else{
  rrpge_m_halt_set(hnd, RRPGE_HLT_STACK);
  return 0U;
 }
}

/* 110-: xr */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_xr(rrpge_object_t* hnd, auint rmw)
{
 hnd->cpu.awf = rrpge_m_addr_wr_xr;
 hnd->cpu.ocy = 0U;
 hnd->cpu.oaw = 1U;
 return (hnd->cpu.xr[hnd->cpu.opc & 0x7U] & 0xFFFFU);
}

/* 1110: Data: x16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_dx16(rrpge_object_t* hnd, auint rmw)
{
 auint s = hnd->cpu.opc & 0x3U;            /* Pointer register select */
 auint t = (s << 2);                       /* 0, 4, 8 or 12, shift amount for xm & xb */
 auint m = (hnd->cpu.xmb[0] >> t) & 0xFU;  /* Pointer mode */
 auint b = (hnd->cpu.xmb[1] >> t) & 0xFU;  /* Pointer fraction */
 auint a;                                  /* Address */

 hnd->cpu.awf = rrpge_m_addr_wr_dx16;
 hnd->cpu.oaw = 1U;
 hnd->cpu.ocy = 1U;

 hnd->cpu.ads = (0xFU - b) & rrpge_m_addr_ams[m];
 hnd->cpu.adm = rrpge_m_addr_dms[m] << hnd->cpu.ads;
 hnd->cpu.ada = hnd->cpu.xr[s + 4U] & 0xFFFFU;
 a  = (hnd->cpu.ada << 4) + b;
 a += ( ((0x0F40U >> m) & 1U) |            /* 1 if post-incrementing ptr. mode */
        ((0xF080U >> m) & rmw) ) <<        /* 1 if post-incrementing on write only mode & rmw set (it is 1) */
      rrpge_m_addr_ash[m];
 hnd->cpu.xr[s + 4U] = a >> 4;
 hnd->cpu.xmb[1] = (hnd->cpu.xmb[1] & (~(0xFU << t))) |
                   ((a & 0xFU) << t);      /* Address write-back */

 rrpge_m_addr_rd_data(hnd, rmw);
 return (hnd->cpu.add & hnd->cpu.adm) >> hnd->cpu.ads;
}

/* 1111: Stack: BP + x16 */
RRPGE_M_FASTCALL static auint rrpge_m_addr_rd_sx16(rrpge_object_t* hnd, auint rmw)
{
 auint s = hnd->cpu.opc & 0x3U;            /* Pointer register select */
 auint t = (s << 2);                       /* 0, 4, 8 or 12, shift amount for xm & xb */
 auint m = (hnd->cpu.xmb[0] >> t) & 0xFU;  /* Pointer mode */
 auint b = (hnd->cpu.xmb[1] >> t) & 0xFU;  /* Pointer fraction */
 auint a;                                  /* Address */

 hnd->cpu.awf = rrpge_m_addr_wr_sx16;
 hnd->cpu.oaw = 1U;
 hnd->cpu.ocy = 1U;

 hnd->cpu.ads = (0xFU - b) & rrpge_m_addr_ams[m];
 hnd->cpu.adm = rrpge_m_addr_dms[m] << hnd->cpu.ads;
 hnd->cpu.ada = hnd->cpu.xr[s + 4U];
 a  = (hnd->cpu.ada << 4) + b;
 a += ( ((0x0F40U >> m) & 1U) |            /* 1 if post-incrementing ptr. mode */
        ((0xF080U >> m) & rmw) ) <<        /* 1 if post-incrementing on write only mode & rmw set (it is 1) */
      rrpge_m_addr_ash[m];
 hnd->cpu.xr[s + 4U] = a >> 4;
 hnd->cpu.xmb[1] = (hnd->cpu.xmb[1] & (~(0xFU << t))) |
                   ((a & 0xFU) << t);      /* Address write-back */

 hnd->cpu.ada = ((hnd->cpu.ada + hnd->cpu.bp) & 0xFFFFU) |
                (hnd->cpu.sbt & (~0xFFFFU));
 if ( (hnd->cpu.ada <  hnd->cpu.stp) &&
      (hnd->cpu.ada >= hnd->cpu.sbt) ){
  return ((hnd->st.dram[hnd->cpu.ada]) & hnd->cpu.adm) >> hnd->cpu.ads;
 }else{
  rrpge_m_halt_set(hnd, RRPGE_HLT_STACK);
  return 0U;
 }
}



/* Addressing mode specific read, function table by opcode bits 0-5. Uses
** hnd->cpu.opc for the addressing mode further on, assumes PC points here
** too for fetching the second opcode word as needed. Sets hnd->cpu.ocy
** and hnd->cpu.oaw (this latter to 1 or 2) depending on the requirements
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
RRPGE_M_FASTCALL auint rrpge_m_stk_pop(rrpge_object_t* hnd)
{
 auint t0;
 hnd->cpu.sp--;
 t0 = ((hnd->cpu.sp + hnd->cpu.bp) & 0xFFFFU) | (hnd->cpu.sbt & (~0xFFFFU));
 if ( (t0 <  hnd->cpu.stp) &&
      (t0 >= hnd->cpu.sbt) ){
  return ((hnd->st.dram[t0]) & 0xFFFFU);
 }else{
  rrpge_m_halt_set(hnd, RRPGE_HLT_STACK);
  return 0U;
 }
}



/* Pushes a value on the stack, sets halt cause if stack pointer is out of
** bounds (high). */
RRPGE_M_FASTCALL void  rrpge_m_stk_push(rrpge_object_t* hnd, auint val)
{
 auint t0;
 t0 = ((hnd->cpu.sp + hnd->cpu.bp) & 0xFFFFU) | (hnd->cpu.sbt & (~0xFFFFU));
 if ( (t0 <  hnd->cpu.stp) &&
      (t0 >= hnd->cpu.sbt) ){
  hnd->st.dram[t0] = val & 0xFFFFU;
 }else{
  rrpge_m_halt_set(hnd, RRPGE_HLT_STACK);
 }
 hnd->cpu.sp++;
}



/* Sets a value on the stack, sets halt cause if stack pointer is out of
** bounds. This is used to save the return address for function calls. */
RRPGE_M_FASTCALL void  rrpge_m_stk_set(rrpge_object_t* hnd, auint off, auint val)
{
 auint t0;
 t0 = (off & 0xFFFFU) | (hnd->cpu.sbt & (~0xFFFFU));
 if ( (t0 <  hnd->cpu.stp) &&
      (t0 >= hnd->cpu.sbt) ){
  hnd->st.dram[t0] = val & 0xFFFFU;
 }else{
  rrpge_m_halt_set(hnd, RRPGE_HLT_STACK);
 }
}
