/**
**  \file
**  \brief     CPU opcode decoder & ALU
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
*/


#include "rgm_cpuo.h"
#include "rgm_cpua.h"
#include "rgm_krnm.h"
#include "rgm_halt.h"


/* Guides:
** During emulating the CPU overflows to above 16 bits are not cared for
** except where it is necessary to ensure correct operation. Usages of
** registers in the info structure should be done accordingly. */


/* Address of the carry register within hnd->cpu.xr */
#define REG_C 2U


/* 0000 000r rraa aaaa: MOV adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_00(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 (void)(rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U));
 hnd->cpu.awf(hnd, hnd->cpu.xr[((op >> 6) & 0x7U)]);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 2U;
}
/* 0000 001r rraa aaaa: MOV rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_02(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 hnd->cpu.xr[((op >> 6) & 0x7U)] = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 2U;
}


/* 0000 010r rraa aaaa: XCH adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xch_04(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 1U);
 auint t1 = hnd->cpu.xr[ra];
 hnd->cpu.awf(hnd, t1);
 hnd->cpu.xr[ra] = t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}

/* 0000 011r rraa aaaa: MOV rx, imx */
static const uint16 rrpge_m_op_mov_06_tb[16] = {
 0x0280U, 0xFF0FU, 0xF0FFU, 0x0180U, 0x0300U, 0x01C0U, 0x0F00U, 0x0118U,
 0x0140U, 0x0168U, 0x0190U, 0x01B8U, 0x01E0U, 0x0208U, 0x0230U, 0x0258U};
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_06(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint i  = (op & 0x3FU);
 if (i < 16U){ i = rrpge_m_op_mov_06_tb[i]; }
 hnd->cpu.xr[(op >> 6) & 0x7U] = i;
 hnd->cpu.pc ++;
 return 2U;
}


/* 0000 100r rraa aaaa: ADD adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_add_08(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 + hnd->cpu.xr[((op >> 6) & 0x7U)]);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0000 101r rraa aaaa: ADD rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_add_0a(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.xr[((op >> 6) & 0x7U)] += t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0000 110r rraa aaaa: SUB adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_sub_0c(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 - hnd->cpu.xr[((op >> 6) & 0x7U)]);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0000 111r rraa aaaa: SUB rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_sub_0e(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.xr[((op >> 6) & 0x7U)] -= t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0001 000r rraa aaaa: ASR adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_asr_10(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t0 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU;
 hnd->cpu.awf(hnd, (t1 >> t0) | ((0U - (t1 >> 15U)) << (15U - t0)) );
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0001 001r rraa aaaa: ASR rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_asr_12(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U) & 0xFU;
 auint t1 = hnd->cpu.xr[ra] & 0xFFFFU;
 hnd->cpu.xr[ra] = (t1 >> t0) | ((0U - (t1 >> 15U)) << (15U - t0));
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0001 010r rraa aaaa: DIV adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_div_14(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t1 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU;
 if (t1 != 0U){ /* (If the divider is 0, then 0 has to be written out) */
  t1 = t0 / t1;
 }
 hnd->cpu.awf(hnd, t1);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 20U;
}
/* 0001 011r rraa aaaa: DIV rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_div_16(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t1 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 auint t0 = hnd->cpu.xr[ra] & 0xFFFFU;
 if (t1 != 0U){ /* (If the divider is 0, then 0 has to be written out) */
  t1 = t0 / t1;
 }
 hnd->cpu.xr[ra] = t1;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 20U;
}


/* 0001 100r rraa aaaa: ADC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_adc_18(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, (t0 + hnd->cpu.xr[((op >> 6) & 0x7U)]) +
                   (hnd->cpu.xr[REG_C] & 1U) );
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0001 101r rraa aaaa: ADC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_adc_1a(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 hnd->cpu.xr[ra] = (hnd->cpu.xr[ra] + t0) +
                   (hnd->cpu.xr[REG_C] & 1U);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0001 110r rraa aaaa: SBC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_sbc_1c(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, (t0 - hnd->cpu.xr[((op >> 6) & 0x7U)]) -
                   (hnd->cpu.xr[REG_C] & 1U) );
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0001 111r rraa aaaa: SBC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_sbc_1e(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 hnd->cpu.xr[ra] = (hnd->cpu.xr[ra] - t0) -
                   (hnd->cpu.xr[REG_C] & 1U);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0010 000r rraa aaaa: NOT adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_not_20(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 (void)(rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U));
 hnd->cpu.awf(hnd, hnd->cpu.xr[((op >> 6) & 0x7U)] ^ 0xFFFFU);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 2U;
}
/* 0010 001r rraa aaaa: NOT rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_not_22(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 hnd->cpu.xr[((op >> 6) & 0x7U)] = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U) ^ 0xFFFFU;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 2U;
}


/* 0010 010r rraa aaaa: MUL adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mul_24(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 * hnd->cpu.xr[((op >> 6) & 0x7U)]);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 12U;
}
/* 0010 011r rraa aaaa: MUL rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_mul_26(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.xr[((op >> 6) & 0x7U)] *= t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 12U;
}


/* 0010 100r rraa aaaa: SHR adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_shr_28(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 >> (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU));
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0010 101r rraa aaaa: SHR rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_shr_2a(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 hnd->cpu.xr[ra] = (hnd->cpu.xr[ra] & 0xFFFFU) >> (t0 & 0xFU);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0010 110r rraa aaaa: SHL adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_shl_2c(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 << (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU));
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0010 111r rraa aaaa: SHL rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_shl_2e(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.xr[((op >> 6) & 0x7U)] <<= t0 & 0xFU;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0011 000r rraa aaaa: OR adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_or_30(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 | hnd->cpu.xr[((op >> 6) & 0x7U)]);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0011 001r rraa aaaa: OR rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_or_32(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.xr[((op >> 6) & 0x7U)] |= t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0011 010r rraa aaaa: MAC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mac_34(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, (t0 * hnd->cpu.xr[((op >> 6) & 0x7U)]) +
                   hnd->cpu.xr[REG_C]);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 13U;
}
/* 0011 011r rraa aaaa: MAC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_mac_36(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 hnd->cpu.xr[ra] = (t0 * hnd->cpu.xr[ra]) +
                   hnd->cpu.xr[REG_C];
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 13U;
}


/* 0011 100r rraa aaaa: SRC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_src_38(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t0 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU;
 hnd->cpu.awf(hnd, (t1 >> t0) | hnd->cpu.xr[REG_C] );
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0011 101r rraa aaaa: SRC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_src_3a(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U) & 0xFU;
 auint t1 = hnd->cpu.xr[ra] & 0xFFFFU;
 hnd->cpu.xr[ra] = (t1 >> t0) | hnd->cpu.xr[REG_C];
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0011 110r rraa aaaa: SLC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_slc_3c(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t0 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU;
 hnd->cpu.awf(hnd, (t1 << t0) | hnd->cpu.xr[REG_C] );
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0011 111r rraa aaaa: SLC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_slc_3e(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U) & 0xFU;
 auint t1 = hnd->cpu.xr[ra];
 hnd->cpu.xr[ra] = (t1 << t0) | hnd->cpu.xr[REG_C];
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0100 000p rraa aaaa: MOV adr, xmn/xbn (Pointer mode moves) */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_40(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 (void)(rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U));
 hnd->cpu.awf(hnd, ( hnd->cpu.xmb[(op >> 8) & 0x1U] >>
                     ((op >> 4) & 0xCU) ) & 0xFU);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 2U;
}
/* 0100 001p rraa aaaa: MOV xmn/xbn, adr (Pointer mode moves) */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_42(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = (op >> 8) & 0x1U;
 auint t1 = (op >> 4) & 0xCU;
 auint t2 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.xmb[t0] = (hnd->cpu.xmb[t0] & (~(0xFU << t1))) |
                    ((t2 & 0xFU) << t1);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 2U;
}


/* Decodes a single function parameter */
RRPGE_M_FASTCALL static auint rrpge_m_op_fpr(rrpge_object_t* hnd, auint* cy)
{
 auint  op = hnd->cpu.opc;
 auint  r;
 if ((op & 0x3C00U) == 0x0000U){ /* Normal address parameter */
  r = (rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U)) & 0xFFFFU;
  *cy         += hnd->cpu.ocy + 2U;
  hnd->cpu.pc += hnd->cpu.oaw;
 }else{
  if       ((op & 0x2000U) != 0x0000U){ /* --1j jjjj jeii iiii */
   r = (op & 0x003FU) | ((op & 0x1F80U) >> 1);
  }else if ((op & 0x3800U) == 0x1800U){ /* --01 1jjj jeii iiii */
   r = ((op & 0x003FU) << 6) | ((op & 0x0780U) << 5) | 0x003FU;
  }else if ((op & 0x3800U) == 0x1000U){ /* --01 0jjj jeii iiii */
   r = ((op & 0x003FU) << 6) | ((op & 0x0780U) << 5);
  }else if ((op & 0x3800U) == 0x0800U){ /* --00 1jjj jeii iiii */
   r = (op & 0x003FU) | ((op & 0x0780U) >> 1) | 0xFC00U;
  }else{                                /* --00 01-j jeii iiii */
   r = (op & 0x003FU) | ((op & 0x0180U) >> 1);
   r = r | (r << 8);
  }
  *cy         += 2U;
  hnd->cpu.pc += 1U;
 }
 return r;
}

/* 0100 010f feaa aaaa: Function / Supervisor entry / return */
RRPGE_M_FASTCALL static auint rrpge_m_op_jfr_44(rrpge_object_t* hnd)
{
 auint  op = hnd->cpu.opc;
 auint  t0;
 auint  t1;
 auint  mx;
 auint  cy;
 uint16 kp[16]; /* Supervisor call parameters */

 if ((op & 0x0080U) == 0U){               /* JFR or JFA: Function entry */
  t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
  cy = hnd->cpu.ocy;
  if ((op & 0x0100U) == 0U){              /* JFR: Relative */
   t0 += hnd->cpu.pc;
  }
  hnd->cpu.pc += hnd->cpu.oaw;
  hnd->cpu.sp ++;                         /* For saving the return address */
  rrpge_m_stk_push(hnd, hnd->cpu.bp);
  t1 = hnd->cpu.bp + hnd->cpu.sp;         /* New base pointer */
  mx = 16U;                               /* Parameter limit */
  do{
   if ((op & 0x0040U) != 0U){ break; }    /* There are no more parameters */
   op           = hnd->crom[hnd->cpu.pc & 0xFFFFU];
   hnd->cpu.opc = op;
   rrpge_m_stk_push(hnd, rrpge_m_op_fpr(hnd, &cy));
   mx --;
  }while(mx);
  rrpge_m_stk_set(hnd, t1 - 2U, hnd->cpu.pc); /* Save return address */
  hnd->cpu.bp = t1;
  hnd->cpu.sp = 16U - mx;
  hnd->cpu.pc = t0;
  cy += 5U;

 }else if ((op & 0x0100U) == 0U){         /* JSV: Supervisor call */

  cy = 0;                                 /* Cycle count */
  kp[0] = op & 0x3FU;                     /* First parameter is the service to call */
  hnd->cpu.pc += 1U;
  mx = 1;                                 /* Parameter count (service to call included) */
  do{
   if ((op & 0x0040U) != 0U){ break; }    /* There are no more parameters */
   op           = hnd->crom[hnd->cpu.pc & 0xFFFFU];
   hnd->cpu.opc = op;
   kp[mx] = rrpge_m_op_fpr(hnd, &cy);
   mx ++;
  }while(mx < 16U);
  cy = rrpge_m_kcall(&kp[0], mx, &hnd->cpu.xr[2], &hnd->cpu.xr[7]); /* Process supervisor (kernel) call */
  /* Note: other cycles discarded as the kernel call timing is absolute
  ** (includes JSV overhead) */

 }else{ /* RFN: Return from function or exit from program */

  if ((hnd->cpu.sbt & 0xFFFFU) >= hnd->cpu.bp){ /* Return from program */

   rrpge_m_halt_set(hnd, RRPGE_HLT_EXIT); /* Exit program */
   cy = 0U;

  }else{ /* Ordinary return */

   hnd->cpu.xr[0x7U] = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U); /* Load X3 */
   cy = hnd->cpu.ocy + 6U;
   if ((op & 0x0040U) != 0U){
    hnd->cpu.xr[REG_C] = 0U;              /* Also clear carry */
   }
   hnd->cpu.sp = 0U;
   t0 = rrpge_m_stk_pop(hnd);             /* Previous bp */
   hnd->cpu.sp = hnd->cpu.bp - t0;
   hnd->cpu.bp = t0;                      /* Stack again points to prev. bp */
   hnd->cpu.sp --;                        /* Now to prev. pc */
   hnd->cpu.pc = rrpge_m_stk_pop(hnd);

  }
 }

 return cy;
}

/* 0100 011r rraa aaaa: MOV rx, imx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_46(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 hnd->cpu.xr[(op >> 6) & 0x7U] = (op & 0x3FU) | 0x40U;
 hnd->cpu.pc ++;
 return 2U;
}


/* 0100 100r rraa aaaa: ADD C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_addc_48(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 t0 = t0 + (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU);
 hnd->cpu.awf(hnd, t0);
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}
/* 0100 101r rraa aaaa: ADD C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_addc_4a(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 t0 = (hnd->cpu.xr[ra] & 0xFFFFU) + t0;
 hnd->cpu.xr[ra]    = t0;
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}


/* 0100 110r rraa aaaa: SUB C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_subc_4c(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 t0 = t0 - (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU);
 hnd->cpu.awf(hnd, t0);
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}
/* 0100 111r rraa aaaa: SUB C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_subc_4e(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 t0 = (hnd->cpu.xr[ra] & 0xFFFFU) - t0;
 hnd->cpu.xr[ra]    = t0;
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}


/* 0101 000r rraa aaaa: ASR C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_asrc_50(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t0 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = (t1 << (16U - t0)) | ((0U - (t1 >> 15U)) << (31U - t0));
 hnd->cpu.awf(hnd, t0 >> 16);
 hnd->cpu.xr[REG_C] = t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}
/* 0101 001r rraa aaaa: ASR C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_asrc_52(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U) & 0xFU;
 auint t1 = hnd->cpu.xr[ra] & 0xFFFFU;
 t0 = (t1 << (16U - t0)) | ((0U - (t1 >> 15U)) << (31U - t0));
 hnd->cpu.xr[ra]    = t0 >> 16;
 hnd->cpu.xr[REG_C] = t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}


/* 0101 010r rraa aaaa: DIV C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_divc_54(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t1 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU;
 auint t2 = 0U;
 if (t1 != 0U){ /* (If the divider is 0, then 0 has to be written out) */
  t2 = t0 % t1;
  t1 = t0 / t1;
 }
 hnd->cpu.awf(hnd, t1);
 hnd->cpu.xr[REG_C] = t2;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 21U;
}
/* 0101 011r rraa aaaa: DIV C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_divc_56(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t1 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 auint t0 = hnd->cpu.xr[ra] & 0xFFFFU;
 auint t2 = 0U;
 if (t1 != 0U){ /* (If the divider is 0, then 0 has to be written out) */
  t2 = t0 % t1;
  t1 = t0 / t1;
 }
 hnd->cpu.xr[ra]    = t1;
 hnd->cpu.xr[REG_C] = t2;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 21U;
}


/* 0101 100r rraa aaaa: ADC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_adcc_58(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 t0 = (t0 + (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU)) +
      (hnd->cpu.xr[REG_C] & 1U);
 hnd->cpu.awf(hnd, t0);
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}
/* 0101 101r rraa aaaa: ADC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_adcc_5a(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 t0 = ((hnd->cpu.xr[ra] & 0xFFFFU) + t0) +
      (hnd->cpu.xr[REG_C] & 1U);
 hnd->cpu.xr[ra]    = t0;
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}


/* 0101 110r rraa aaaa: SBC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_sbcc_5c(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 t0 = (t0 - (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU)) -
      (hnd->cpu.xr[REG_C] & 1U);
 hnd->cpu.awf(hnd, t0);
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}
/* 0101 111r rraa aaaa: SBC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_sbcc_5e(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 t0 = ((hnd->cpu.xr[ra] & 0xFFFFU) - t0) -
      (hnd->cpu.xr[REG_C] & 1U);
 hnd->cpu.xr[ra]    = t0;
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}


/* 0110 000r rraa aaaa: NEG adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_neg_60(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 (void)(rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U));
 hnd->cpu.awf(hnd, 0U - hnd->cpu.xr[((op >> 6) & 0x7U)]);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0110 001r rraa aaaa: NEG rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_neg_62(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 hnd->cpu.xr[((op >> 6) & 0x7U)] = 0U - rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0110 010r rraa aaaa: MUL C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mulc_64(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 t0 = t0 * (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU);
 hnd->cpu.awf(hnd, t0);
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 13U;
}
/* 0110 011r rraa aaaa: MUL C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_mulc_66(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 t0 = (hnd->cpu.xr[ra] & 0xFFFFU) * t0;
 hnd->cpu.xr[ra]    = t0;
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 13U;
}


/* 0110 100r rraa aaaa: SHR C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_shrc_68(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t0 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = t1 << (16U - t0);
 hnd->cpu.awf(hnd, t0 >> 16);
 hnd->cpu.xr[REG_C] = t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}
/* 0110 101r rraa aaaa: SHR C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_shrc_6a(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U) & 0xFU;
 auint t1 = hnd->cpu.xr[ra] & 0xFFFFU;
 t0 = t1 << (16U - t0);
 hnd->cpu.xr[ra]    = t0 >> 16;
 hnd->cpu.xr[REG_C] = t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}


/* 0110 110r rraa aaaa: SHL C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_shlc_6c(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t0 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = t1 << t0;
 hnd->cpu.awf(hnd, t0);
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}
/* 0110 111r rraa aaaa: SHL C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_shlc_6e(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U) & 0xFU;
 auint t1 = hnd->cpu.xr[ra] & 0xFFFFU;
 t0 = t1 << t0;
 hnd->cpu.xr[ra]    = t0;
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}


/* 0111 000r rraa aaaa: XOR adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xor_70(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 ^ hnd->cpu.xr[((op >> 6) & 0x7U)]);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 0111 001r rraa aaaa: XOR rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_xor_72(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.xr[((op >> 6) & 0x7U)] ^= t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 0111 010r rraa aaaa: MAC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_macc_74(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 t0 = (t0 * (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU)) +
      (hnd->cpu.xr[REG_C] & 0xFFFFU);
 hnd->cpu.awf(hnd, t0);
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 14U;
}
/* 0111 011r rraa aaaa: MAC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_macc_76(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U);
 t0 = ((hnd->cpu.xr[ra] & 0xFFFFU) * t0) +
      (hnd->cpu.xr[REG_C] & 0xFFFFU);
 hnd->cpu.xr[ra]    = t0;
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 14U;
}


/* 0111 100r rraa aaaa: SRC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_srcc_78(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t0 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = (t1 << (16U - t0)) | (hnd->cpu.xr[REG_C] << 16);
 hnd->cpu.awf(hnd, t0 >> 16);
 hnd->cpu.xr[REG_C] = t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}
/* 0111 101r rraa aaaa: SRC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_srcc_7a(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U) & 0xFU;
 auint t1 = hnd->cpu.xr[ra] & 0xFFFFU;
 t0 = (t1 << (16U - t0)) | (hnd->cpu.xr[REG_C] << 16);
 hnd->cpu.xr[ra]    = t0 >> 16;
 hnd->cpu.xr[REG_C] = t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}


/* 0111 110r rraa aaaa: SLC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_slcc_7c(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 auint t0 = hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = (t1 << t0) | (hnd->cpu.xr[REG_C] & 0xFFFFU);
 hnd->cpu.awf(hnd, t0);
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}
/* 0011 111r rraa aaaa: SLC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_slcc_7e(rrpge_object_t* hnd)
{
 auint ra = ((hnd->cpu.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[hnd->cpu.opc & 0x3FU](hnd, 0U) & 0xFU;
 auint t1 = hnd->cpu.xr[ra] & 0xFFFFU;
 t0 = (t1 << t0) | (hnd->cpu.xr[REG_C] & 0xFFFFU);
 hnd->cpu.xr[ra]    = t0;
 hnd->cpu.xr[REG_C] = t0 >> 16;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 4U;
}


/* 1000 000r rraa aaaa: MOV adr, special (SP, XM or XB) & SP ops */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_80(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0;
 if ((op & 0x0100U) == 0U){
  if ((op & 0x00C0U) != 0x00C0U){ /* Register move */
   t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
   hnd->cpu.pc += hnd->cpu.oaw;
   if ((op & 0x0080U) != 0U){     /* SP */
    hnd->cpu.awf(hnd, hnd->cpu.sp);
   }else{                         /* XM or XB in order by bit6 of opcode */
    hnd->cpu.awf(hnd, hnd->cpu.xmb[((op >> 6) & 0x1U)]);
   }
   return hnd->cpu.ocy + 2U;
  }else{                          /* XUG adr, SP */
   t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
   hnd->cpu.pc += hnd->cpu.oaw;
   if (t0 >  (hnd->cpu.sp & 0xFFFFU)){
    hnd->cpu.pc++;
    hnd->cpu.ocy++;
   }
   return hnd->cpu.ocy + 3U;
  }
 }else{
  if ((op & 0x00C0U) == 0x0040U){ /* XEQ adr, SP */
   t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
   hnd->cpu.pc += hnd->cpu.oaw;
   if (t0 == (hnd->cpu.sp & 0xFFFFU)){
    hnd->cpu.pc++;
    hnd->cpu.ocy++;
   }
   return hnd->cpu.ocy + 3U;
  }else if ((op & 0x0087U) == 0x0080U){ /* MOV imx, SP (NOP) */
   hnd->cpu.pc += 1U;
   return 2U;
  }else{                          /* PSH */
   t0 = 2U;                       /* Cycles */
   if ((op & 0x80U) != 0U){ rrpge_m_stk_push(hnd, hnd->cpu.xmb[0U]); t0 ++; }
   if ((op & 0x40U) != 0U){ rrpge_m_stk_push(hnd, hnd->cpu.xmb[1U]); t0 ++; }
   if ((op & 0x20U) != 0U){ rrpge_m_stk_push(hnd, hnd->cpu.xr [0U]); t0 ++; }
   if ((op & 0x10U) != 0U){ rrpge_m_stk_push(hnd, hnd->cpu.xr [1U]); t0 ++; }
   if ((op & 0x08U) != 0U){ rrpge_m_stk_push(hnd, hnd->cpu.xr [3U]); t0 ++; }
   if ((op & 0x04U) != 0U){ rrpge_m_stk_push(hnd, hnd->cpu.xr [4U]); t0 ++; }
   if ((op & 0x02U) != 0U){ rrpge_m_stk_push(hnd, hnd->cpu.xr [5U]); t0 ++; }
   if ((op & 0x01U) != 0U){ rrpge_m_stk_push(hnd, hnd->cpu.xr [6U]); t0 ++; }
   hnd->cpu.pc += 1U;
   return t0;
  }
 }
}
/* 1000 001r rraa aaaa: MOV special, adr (SP, XM or XB) & SP ops */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_82(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0;
 if ((op & 0x0100U) == 0U){
  t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
  hnd->cpu.pc += hnd->cpu.oaw;
  if ((op & 0x00C0U) != 0x00C0U){ /* Register move */
   if ((op & 0x0080U) != 0U){     /* SP */
    hnd->cpu.sp = t0;
   }else{                         /* XM or XB in order by bit6 of opcode */
    hnd->cpu.xmb[((op >> 6) & 0x1U)] = t0;
   }
   return hnd->cpu.ocy + 2U;
  }else{                          /* XUG SP, adr */
   if (t0 <  (hnd->cpu.sp & 0xFFFFU)){
    hnd->cpu.pc++;
    hnd->cpu.ocy++;
   }
   return hnd->cpu.ocy + 3U;
  }
 }else{
  if ((op & 0x00C0U) == 0x0040U){ /* XNE SP, adr */
   t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
   hnd->cpu.pc += hnd->cpu.oaw;
   if (t0 != (hnd->cpu.sp & 0xFFFFU)){
    hnd->cpu.pc++;
    hnd->cpu.ocy++;
   }
   return hnd->cpu.ocy + 3U;
  }else if ((op & 0x0087U) == 0x0080U){ /* MOV SP, imx */
   hnd->cpu.sp = (op & 0x00F8U) >> 3;   /* Immediate: 16 - 31 */
   hnd->cpu.pc += 1U;
   return 2U;
  }else{                          /* POP */
   t0 = 2U;                       /* Cycles */
   if ((op & 0x01U) != 0U){ hnd->cpu.xr [6U] = rrpge_m_stk_pop(hnd); t0 ++; }
   if ((op & 0x02U) != 0U){ hnd->cpu.xr [5U] = rrpge_m_stk_pop(hnd); t0 ++; }
   if ((op & 0x04U) != 0U){ hnd->cpu.xr [4U] = rrpge_m_stk_pop(hnd); t0 ++; }
   if ((op & 0x08U) != 0U){ hnd->cpu.xr [3U] = rrpge_m_stk_pop(hnd); t0 ++; }
   if ((op & 0x10U) != 0U){ hnd->cpu.xr [1U] = rrpge_m_stk_pop(hnd); t0 ++; }
   if ((op & 0x20U) != 0U){ hnd->cpu.xr [0U] = rrpge_m_stk_pop(hnd); t0 ++; }
   if ((op & 0x40U) != 0U){ hnd->cpu.xmb[1U] = rrpge_m_stk_pop(hnd); t0 ++; }
   if ((op & 0x80U) != 0U){ hnd->cpu.xmb[0U] = rrpge_m_stk_pop(hnd); t0 ++; }
   hnd->cpu.pc += 1U;
   return t0;
  }
 }
}


/* 1000 010f rraa aaaa: JMR, JMA (absolute / relative jumps) */
RRPGE_M_FASTCALL static auint rrpge_m_op_jmp_84(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 if ((op & 0x00C0U) != 0U){ /* B, C or D should receive PC after jump */
  hnd->cpu.xr[(op >> 6) & 0x3U] = hnd->cpu.pc + hnd->cpu.oaw;
 }
 if ((op & 0x0100U) == 0U){ /* JMR */
  hnd->cpu.pc += t0;
  hnd->cpu.ocy ++;
 }else{                     /* JMA */
  hnd->cpu.pc = t0;
 }
 return hnd->cpu.ocy + 4U;
}

/* 1000 011r rraa aaaa: MOV rx, imx */
static const uint16 rrpge_m_op_mov_86_tb[64] = {
 0x0080U, 0x0088U, 0x0090U, 0x0098U, 0x0010U, 0x0020U, 0x0040U, 0x0080U,
 0x0100U, 0x0200U, 0x0400U, 0x0800U, 0x1000U, 0x2000U, 0x4000U, 0x8000U,
 0x00A0U, 0x00A8U, 0x00B0U, 0x00B8U, 0xFFEFU, 0xFFDFU, 0xFFBFU, 0xFF7FU,
 0xFEFFU, 0xFDFFU, 0xFBFFU, 0xF7FFU, 0xEFFFU, 0xDFFFU, 0xBFFFU, 0x7FFFU,
 0x00C0U, 0x00C8U, 0x00D0U, 0x00D8U, 0xFFE0U, 0xFFC0U, 0xFF80U, 0xFF00U,
 0xFE00U, 0xFC00U, 0xF800U, 0xF000U, 0xE000U, 0xC000U, 0x8000U, 0x0000U,
 0x00E0U, 0x00E8U, 0x00F0U, 0x00F8U, 0x001FU, 0x003FU, 0x007FU, 0x00FFU,
 0x01FFU, 0x03FFU, 0x07FFU, 0x0FFFU, 0x1FFFU, 0x3FFFU, 0x7FFFU, 0xFFFFU};
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_86(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 hnd->cpu.xr[(op >> 6) & 0x7U] = rrpge_m_op_mov_86_tb[op & 0x3FU];
 hnd->cpu.pc ++;
 return 2U;
}


/* 1000 10ir rrii iiii: JNZ rx, simm7 */
RRPGE_M_FASTCALL static auint rrpge_m_op_jnz_88(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 if ((hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU) != 0U){
  hnd->cpu.pc += ((op & 0x3FU) | ((~(op >> 3)) & 0x40U)) - 0x40U;
  return 4U;
 }else{
  hnd->cpu.pc ++;
  return 2U;
 }
}


/* 1000 11ii iiii iiii: JMS simm10 */
RRPGE_M_FASTCALL static auint rrpge_m_op_jms_8c(rrpge_object_t* hnd)
{
 auint t0 = ((hnd->cpu.opc & 0x03FFU) ^ 0x0200U) - 0x0200U;
 hnd->cpu.pc += t0;
 return 4U;
}


/* 1001 ---- ---- ----: Supervisor mode ops (application fault) */
RRPGE_M_FASTCALL static auint rrpge_m_op_sv(rrpge_object_t* hnd)
{
 rrpge_m_halt_set(hnd, RRPGE_HLT_INVOP);
 return 0;
}


/* 1010 00ii iiaa aaaa: BTC adr, imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_op_btc_a0(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 & (~((auint)(1U) << ((op >> 6) & 0xFU))));
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 1010 10ii iiaa aaaa: BTS adr, imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_op_bts_a8(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 | ( ((auint)(1U) << ((op >> 6) & 0xFU))));
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 1010 01ii iiaa aaaa: XBC adr, imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_op_xbc_a4(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 if ( (t0 & ((auint)(1U) << ((op >> 6) & 0xFU))) == 0U){
  hnd->cpu.pc++;
  hnd->cpu.ocy++;
 }
 return hnd->cpu.ocy + 3U;
}
/* 1010 11ii iiaa aaaa: XBS adr, imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_op_xbs_ac(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 if ( (t0 & ((auint)(1U) << ((op >> 6) & 0xFU))) != 0U){
  hnd->cpu.pc++;
  hnd->cpu.ocy++;
 }
 return hnd->cpu.ocy + 3U;
}


/* 1000 100r rraa aaaa: AND adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_and_b0(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 1U);
 hnd->cpu.awf(hnd, t0 & hnd->cpu.xr[((op >> 6) & 0x7U)]);
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}
/* 1000 101r rraa aaaa: AND rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_and_b2(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.xr[((op >> 6) & 0x7U)] &= t0;
 hnd->cpu.pc += hnd->cpu.oaw;
 return hnd->cpu.ocy + 3U;
}


/* 1011 010r rraa aaaa: XSG adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xsg_b4(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 if ( ((t0 + 0x8000U) & 0xFFFFU) >
      ((hnd->cpu.xr[((op >> 6) & 0x7U)] + 0x8000U) & 0xFFFFU) ){
  hnd->cpu.pc++;
  hnd->cpu.ocy++;
 }
 return hnd->cpu.ocy + 3U;
}
/* 1011 011r rraa aaaa: XSG rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_xsg_b6(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 if ( ((t0 + 0x8000U) & 0xFFFFU) <
      ((hnd->cpu.xr[((op >> 6) & 0x7U)] + 0x8000U) & 0xFFFFU) ){
  hnd->cpu.pc++;
  hnd->cpu.ocy++;
 }
 return hnd->cpu.ocy + 3U;
}


/* 1011 100r rraa aaaa: XEQ adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xeq_b8(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 if (t0 == (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU)){
  hnd->cpu.pc++;
  hnd->cpu.ocy++;
 }
 return hnd->cpu.ocy + 3U;
}
/* 1011 101r rraa aaaa: XNE rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_xne_ba(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 if (t0 != (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU)){
  hnd->cpu.pc++;
  hnd->cpu.ocy++;
 }
 return hnd->cpu.ocy + 3U;
}


/* 1011 110r rraa aaaa: XUG adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xug_bc(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 if (t0 > (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU)){
  hnd->cpu.pc++;
  hnd->cpu.ocy++;
 }
 return hnd->cpu.ocy + 3U;
}
/* 1011 111r rraa aaaa: XUG rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_xug_be(rrpge_object_t* hnd)
{
 auint op = hnd->cpu.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](hnd, 0U);
 hnd->cpu.pc += hnd->cpu.oaw;
 if (t0 < (hnd->cpu.xr[((op >> 6) & 0x7U)] & 0xFFFFU)){
  hnd->cpu.pc++;
  hnd->cpu.ocy++;
 }
 return hnd->cpu.ocy + 3U;
}


/* 11-- ---- ---- ----: NOP */
RRPGE_M_FASTCALL static auint rrpge_m_op_nop(rrpge_object_t* hnd)
{
 hnd->cpu.pc++;
 return 1U;
}



/* CPU opcode call table. The rrpge_m_info structure must be set up
** appropriately to call these (note the opcode cache member). As above, the
** effect of the opcode's execution varies depending on what the user program
** attempted to perform. Callbacks may also happen (here through calling the
** supervisor). */
rrpge_m_opf_t* const rrpge_m_optable[128] = {
 &rrpge_m_op_mov_00,  &rrpge_m_op_mov_02,  &rrpge_m_op_xch_04,  &rrpge_m_op_mov_06,
 &rrpge_m_op_add_08,  &rrpge_m_op_add_0a,  &rrpge_m_op_sub_0c,  &rrpge_m_op_sub_0e,
 &rrpge_m_op_asr_10,  &rrpge_m_op_asr_12,  &rrpge_m_op_div_14,  &rrpge_m_op_div_16,
 &rrpge_m_op_adc_18,  &rrpge_m_op_adc_1a,  &rrpge_m_op_sbc_1c,  &rrpge_m_op_sbc_1e,
 &rrpge_m_op_not_20,  &rrpge_m_op_not_22,  &rrpge_m_op_mul_24,  &rrpge_m_op_mul_26,
 &rrpge_m_op_shr_28,  &rrpge_m_op_shr_2a,  &rrpge_m_op_shl_2c,  &rrpge_m_op_shl_2e,
 &rrpge_m_op_or_30,   &rrpge_m_op_or_32,   &rrpge_m_op_mac_34,  &rrpge_m_op_mac_36,
 &rrpge_m_op_src_38,  &rrpge_m_op_src_3a,  &rrpge_m_op_slc_3c,  &rrpge_m_op_slc_3e,
 &rrpge_m_op_mov_40,  &rrpge_m_op_mov_42,  &rrpge_m_op_jfr_44,  &rrpge_m_op_mov_46,
 &rrpge_m_op_addc_48, &rrpge_m_op_addc_4a, &rrpge_m_op_subc_4c, &rrpge_m_op_subc_4e,
 &rrpge_m_op_asrc_50, &rrpge_m_op_asrc_52, &rrpge_m_op_divc_54, &rrpge_m_op_divc_56,
 &rrpge_m_op_adcc_58, &rrpge_m_op_adcc_5a, &rrpge_m_op_sbcc_5c, &rrpge_m_op_sbcc_5e,
 &rrpge_m_op_neg_60,  &rrpge_m_op_neg_62,  &rrpge_m_op_mulc_64, &rrpge_m_op_mulc_66,
 &rrpge_m_op_shrc_68, &rrpge_m_op_shrc_6a, &rrpge_m_op_shlc_6c, &rrpge_m_op_shlc_6e,
 &rrpge_m_op_xor_70,  &rrpge_m_op_xor_72,  &rrpge_m_op_macc_74, &rrpge_m_op_macc_76,
 &rrpge_m_op_srcc_78, &rrpge_m_op_srcc_7a, &rrpge_m_op_slcc_7c, &rrpge_m_op_slcc_7e,
 &rrpge_m_op_mov_80,  &rrpge_m_op_mov_82,  &rrpge_m_op_jmp_84,  &rrpge_m_op_mov_86,
 &rrpge_m_op_jnz_88,  &rrpge_m_op_jnz_88,  &rrpge_m_op_jms_8c,  &rrpge_m_op_jms_8c,
 &rrpge_m_op_sv,      &rrpge_m_op_sv,      &rrpge_m_op_sv,      &rrpge_m_op_sv,
 &rrpge_m_op_sv,      &rrpge_m_op_sv,      &rrpge_m_op_sv,      &rrpge_m_op_sv,
 &rrpge_m_op_btc_a0,  &rrpge_m_op_btc_a0,  &rrpge_m_op_xbc_a4,  &rrpge_m_op_xbc_a4,
 &rrpge_m_op_bts_a8,  &rrpge_m_op_bts_a8,  &rrpge_m_op_xbs_ac,  &rrpge_m_op_xbs_ac,
 &rrpge_m_op_and_b0,  &rrpge_m_op_and_b2,  &rrpge_m_op_xsg_b4,  &rrpge_m_op_xsg_b6,
 &rrpge_m_op_xeq_b8,  &rrpge_m_op_xne_ba,  &rrpge_m_op_xug_bc,  &rrpge_m_op_xug_be,
 &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,
 &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,
 &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,
 &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,
 &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,
 &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,
 &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,
 &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop,     &rrpge_m_op_nop
};
