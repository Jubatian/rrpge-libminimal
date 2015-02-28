/**
**  \file
**  \brief     CPU opcode decoder & ALU
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.02.28
*/


#include "rgm_cpuo.h"
#include "rgm_cpua.h"
#include "rgm_krnm.h"


/* Guides:
** During emulating the CPU overflows to above 16 bits are not cared for
** except where it is necessary to ensure correct operation. Usages of
** registers in the info structure should be done accordingly. */


/* Address of the carry register within rrpge_m_info.xr */
#define REG_C 2U


/* 0000 000r rraa aaaa: MOV adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_00(void)
{
 auint op = rrpge_m_info.opc;
 (void)(rrpge_m_addr_read_table[op & 0x3FU](1U));
 rrpge_m_info.awf(rrpge_m_info.xr[((op >> 6) & 0x7U)]);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 3U;
}
/* 0000 001r rraa aaaa: MOV rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_02(void)
{
 auint op = rrpge_m_info.opc;
 rrpge_m_info.xr[((op >> 6) & 0x7U)] = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 3U;
}


/* 0000 010r rraa aaaa: XCH adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xch_04(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](1U);
 auint t1 = rrpge_m_info.xr[ra];
 rrpge_m_info.awf(t1);
 rrpge_m_info.xr[ra] = t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}

/* 0000 011r rraa aaaa: MOV rx, imx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_06(void)
{
 auint op = rrpge_m_info.opc;
 auint p  = (0U - ((op >> 5) & 1U)) & 0xFU;
 auint q  = (0U - ((op >> 4) & 1U)) & 0xFU;
 auint i  = (op & 0xFU);
 rrpge_m_info.xr[((op >> 6) & 0x7U)] = (p << 12) | (p << 8) | (i << 4) | (q);
 rrpge_m_info.pc ++;
 return 3U;
}


/* 0000 100r rraa aaaa: ADD adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_add_08(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 + rrpge_m_info.xr[((op >> 6) & 0x7U)]);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0000 101r rraa aaaa: ADD rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_add_0a(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.xr[((op >> 6) & 0x7U)] += t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0000 110r rraa aaaa: SUB adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_sub_0c(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 - rrpge_m_info.xr[((op >> 6) & 0x7U)]);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0000 111r rraa aaaa: SUB rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_sub_0e(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.xr[((op >> 6) & 0x7U)] -= t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0001 000r rraa aaaa: OR adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_or_10(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 | rrpge_m_info.xr[((op >> 6) & 0x7U)]);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0001 001r rraa aaaa: OR rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_or_12(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.xr[((op >> 6) & 0x7U)] |= t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0001 010r rraa aaaa: DIV adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_div_14(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t1 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU;
 if (t1 != 0U){ /* (If the divider is 0, then 0 has to be written out) */
  t1 = t0 / t1;
 }
 rrpge_m_info.awf(t1);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 21U;
}
/* 0001 011r rraa aaaa: DIV rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_div_16(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t1 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 auint t0 = rrpge_m_info.xr[ra] & 0xFFFFU;
 if (t1 != 0U){ /* (If the divider is 0, then 0 has to be written out) */
  t1 = t0 / t1;
 }
 rrpge_m_info.xr[ra] = t1;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 21U;
}


/* 0001 100r rraa aaaa: ADC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_adc_18(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf( (t0 + rrpge_m_info.xr[((op >> 6) & 0x7U)]) +
                   (rrpge_m_info.xr[REG_C] & 1U) );
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0001 101r rraa aaaa: ADC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_adc_1a(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 rrpge_m_info.xr[ra] = (rrpge_m_info.xr[ra] + t0) +
                       (rrpge_m_info.xr[REG_C] & 1U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0001 110r rraa aaaa: SBC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_sbc_1c(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf( (t0 - rrpge_m_info.xr[((op >> 6) & 0x7U)]) -
                   (rrpge_m_info.xr[REG_C] & 1U) );
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0001 111r rraa aaaa: SBC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_sbc_1e(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 rrpge_m_info.xr[ra] = (rrpge_m_info.xr[ra] - t0) -
                       (rrpge_m_info.xr[REG_C] & 1U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0010 000r rraa aaaa: NOT adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_not_20(void)
{
 auint op = rrpge_m_info.opc;
 (void)(rrpge_m_addr_read_table[op & 0x3FU](1U));
 rrpge_m_info.awf(rrpge_m_info.xr[((op >> 6) & 0x7U)] ^ 0xFFFFU);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 3U;
}
/* 0010 001r rraa aaaa: NOT rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_not_22(void)
{
 auint op = rrpge_m_info.opc;
 rrpge_m_info.xr[((op >> 6) & 0x7U)] = rrpge_m_addr_read_table[op & 0x3FU](0U) ^ 0xFFFFU;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 3U;
}


/* 0010 010r rraa aaaa: MUL adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mul_24(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 * rrpge_m_info.xr[((op >> 6) & 0x7U)]);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 13U;
}
/* 0010 011r rraa aaaa: MUL rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_mul_26(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.xr[((op >> 6) & 0x7U)] *= t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 13U;
}


/* 0010 100r rraa aaaa: SHR adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_shr_28(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 >> (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU));
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0010 101r rraa aaaa: SHR rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_shr_2a(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 rrpge_m_info.xr[ra] = (rrpge_m_info.xr[ra] & 0xFFFFU) >> (t0 & 0xFU);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0010 110r rraa aaaa: SHL adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_shl_2c(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 << (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU));
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0010 111r rraa aaaa: SHL rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_shl_2e(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.xr[((op >> 6) & 0x7U)] <<= t0 & 0xFU;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0011 000r rraa aaaa: ASR adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_asr_30(void)
{
 auint op = rrpge_m_info.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t0 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU;
 rrpge_m_info.awf( (t1 >> t0) | ((0U - (t1 >> 15U)) << (15U - t0)) );
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0011 001r rraa aaaa: ASR rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_asr_32(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U) & 0xFU;
 auint t1 = rrpge_m_info.xr[ra] & 0xFFFFU;
 rrpge_m_info.xr[ra] = (t1 >> t0) | ((0U - (t1 >> 15U)) << (15U - t0));
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0011 010r rraa aaaa: MAC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mac_34(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf( (t0 * rrpge_m_info.xr[((op >> 6) & 0x7U)]) +
                   rrpge_m_info.xr[REG_C]);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 14U;
}
/* 0011 011r rraa aaaa: MAC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_mac_36(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 rrpge_m_info.xr[ra] = (t0 * rrpge_m_info.xr[ra]) +
                       rrpge_m_info.xr[REG_C];
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 14U;
}


/* 0011 100r rraa aaaa: SRC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_src_38(void)
{
 auint op = rrpge_m_info.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t0 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU;
 rrpge_m_info.awf( (t1 >> t0) | rrpge_m_info.xr[REG_C] );
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0011 101r rraa aaaa: SRC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_src_3a(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U) & 0xFU;
 auint t1 = rrpge_m_info.xr[ra] & 0xFFFFU;
 rrpge_m_info.xr[ra] = (t1 >> t0) | rrpge_m_info.xr[REG_C];
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0011 110r rraa aaaa: SLC adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_slc_3c(void)
{
 auint op = rrpge_m_info.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t0 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU;
 rrpge_m_info.awf( (t1 << t0) | rrpge_m_info.xr[REG_C] );
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0011 111r rraa aaaa: SLC rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_slc_3e(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U) & 0xFU;
 auint t1 = rrpge_m_info.xr[ra];
 rrpge_m_info.xr[ra] = (t1 << t0) | rrpge_m_info.xr[REG_C];
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0100 000p rraa aaaa: MOV adr, xmn/xhn (Pointer mode moves) */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_40(void)
{
 auint op = rrpge_m_info.opc;
 (void)(rrpge_m_addr_read_table[op & 0x3FU](1U));
 rrpge_m_info.awf( ( rrpge_m_info.xmh[(op >> 8) & 0x1U] >>
                     ((op >> 4) & 0xCU) ) & 0xFU);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 3U;
}
/* 0100 001p rraa aaaa: MOV xmn/xhn, adr (Pointer mode moves) */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_42(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = (op >> 8) & 0x1U;
 auint t1 = (op >> 4) & 0xCU;
 auint t2 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.xmh[t0] = (rrpge_m_info.xmh[t0] & (~(0xFU << t1))) |
                        ((t2 & 0xFU) << t1);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 3U;
}


/* Decodes a single function parameter */
RRPGE_M_FASTCALL static auint rrpge_m_op_fpr(auint* cy)
{
 auint  op = rrpge_m_info.opc;
 auint  r;
 if ((op & 0x3C00U) == 0x0000U){ /* Normal address parameter */
  r = (rrpge_m_addr_read_table[op & 0x3FU](0U)) & 0xFFFFU;
  *cy             += rrpge_m_info.ocy + 4U;
  rrpge_m_info.pc += rrpge_m_info.oaw;
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
  *cy             += 4U;
  rrpge_m_info.pc += 1U;
 }
 return r;
}

/* 0100 010f feaa aaaa: Function / Supervisor entry / return */
RRPGE_M_FASTCALL static auint rrpge_m_op_jfr_44(void)
{
 auint  op = rrpge_m_info.opc;
 auint  t0;
 auint  t1;
 auint  mx;
 auint  cy;
 uint16 kp[16]; /* Supervisor call parameters */

 if ((op & 0x0080U) == 0U){               /* JFR or JFA: Function entry */
  t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
  cy = rrpge_m_info.ocy;
  if ((op & 0x0100U) == 0U){              /* JFR: Relative */
   t0 += rrpge_m_info.pc;
  }
  rrpge_m_info.pc += rrpge_m_info.oaw;
  rrpge_m_stk_push(rrpge_m_info.pc);
  rrpge_m_stk_push(rrpge_m_info.bp);
  t1 = rrpge_m_info.bp + rrpge_m_info.sp; /* New base pointer */
  mx = 16U;                               /* Parameter limit */
  do{
   if ((op & 0x0040U) != 0U){ break; }    /* There are no more parameters */
   op               = rrpge_m_edat->crom[rrpge_m_info.pc & 0xFFFFU];
   rrpge_m_info.opc = op;
   rrpge_m_stk_push(rrpge_m_op_fpr(&cy));
   mx --;
  }while(mx);
  rrpge_m_info.bp = t1;
  rrpge_m_info.sp = 16U - mx;
  rrpge_m_info.pc = t0;
  cy += 2U + 7U;

 }else if ((op & 0x0100U) == 0U){         /* JSV: Supervisor call */

  cy = 0;                                 /* Cycle count */
  kp[0] = op & 0x3FU;                     /* First parameter is the service to call */
  rrpge_m_info.pc += 1U;
  mx = 1;                                 /* Parameter count (service to call included) */
  do{
   if ((op & 0x0040U) != 0U){ break; }    /* There are no more parameters */
   op               = rrpge_m_edat->crom[rrpge_m_info.pc & 0xFFFFU];
   rrpge_m_info.opc = op;
   kp[mx] = rrpge_m_op_fpr(&cy);
   mx ++;
  }while(mx < 16U);
  cy += 2U + 7U;
  rrpge_m_kcall(&kp[0], mx);              /* Process supervisor (kernel) call */

 }else{ /* RFN: Return from function or exit from program */

  if ((rrpge_m_info.sbt & 0xFFFFU) >= rrpge_m_info.bp){ /* Return from program */

   rrpge_m_info.hlt = RRPGE_HLT_EXIT;     /* Exit program */
   cy = 0U;

  }else{ /* Ordinary return */

   rrpge_m_info.xr[0x7U] = rrpge_m_addr_read_table[op & 0x3FU](0U); /* Load X3 */
   cy = rrpge_m_info.ocy + 9U;
   if ((op & 0x0040U) != 0U){
    rrpge_m_info.xr[REG_C] = 0U;          /* Also clear carry */
    cy ++;
   }
   rrpge_m_info.sp = 0U;
   t0 = rrpge_m_stk_pop();   /* Previous bp */
   rrpge_m_info.sp = rrpge_m_info.bp - t0;
   rrpge_m_info.bp = t0;     /* Stack again points to prev. bp */
   rrpge_m_info.sp --;       /* Now to prev. pc */
   rrpge_m_info.pc = rrpge_m_stk_pop();

  }
 }

 return cy;
}

/* 0100 011r rraa aaaa: MOV rx, imx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_46(void)
{
 auint op = rrpge_m_info.opc;
 auint p  = (0U - ((op >> 5) & 1U)) & 0xFU;
 auint q  = (0U - ((op >> 4) & 1U)) & 0xFU;
 auint i  = (op & 0xFU);
 rrpge_m_info.xr[((op >> 6) & 0x7U)] = (p << 12) | (i << 8) | (q << 4) | (q);
 rrpge_m_info.pc ++;
 return 3U;
}


/* 0100 100r rraa aaaa: ADD C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_addc_48(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 t0 = t0 + (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU);
 rrpge_m_info.awf(t0);
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0100 101r rraa aaaa: ADD C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_addc_4a(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 t0 = (rrpge_m_info.xr[ra] & 0xFFFFU) + t0;
 rrpge_m_info.xr[ra]    = t0;
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0100 110r rraa aaaa: SUB C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_subc_4c(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 t0 = t0 - (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU);
 rrpge_m_info.awf(t0);
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0100 111r rraa aaaa: SUB C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_subc_4e(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 t0 = (rrpge_m_info.xr[ra] & 0xFFFFU) - t0;
 rrpge_m_info.xr[ra]    = t0;
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0101 000r rraa aaaa: XOR adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xor_50(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 ^ rrpge_m_info.xr[((op >> 6) & 0x7U)]);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0101 001r rraa aaaa: XOR rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_xor_52(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.xr[((op >> 6) & 0x7U)] ^= t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0101 010r rraa aaaa: DIV C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_divc_54(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t1 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU;
 auint t2 = 0U;
 if (t1 != 0U){ /* (If the divider is 0, then 0 has to be written out) */
  t2 = t0 % t1;
  t1 = t0 / t1;
 }
 rrpge_m_info.awf(t1);
 rrpge_m_info.xr[REG_C] = t2;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 11U;
}
/* 0101 011r rraa aaaa: DIV C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_divc_56(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t1 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 auint t0 = rrpge_m_info.xr[ra] & 0xFFFFU;
 auint t2 = 0U;
 if (t1 != 0U){ /* (If the divider is 0, then 0 has to be written out) */
  t2 = t0 % t1;
  t1 = t0 / t1;
 }
 rrpge_m_info.xr[ra]    = t1;
 rrpge_m_info.xr[REG_C] = t2;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 11U;
}


/* 0101 100r rraa aaaa: ADC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_adcc_58(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 t0 = (t0 + (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU)) +
      (rrpge_m_info.xr[REG_C] & 1U);
 rrpge_m_info.awf(t0);
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0101 101r rraa aaaa: ADC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_adcc_5a(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 t0 = ((rrpge_m_info.xr[ra] & 0xFFFFU) + t0) +
      (rrpge_m_info.xr[REG_C] & 1U);
 rrpge_m_info.xr[ra]    = t0;
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0101 110r rraa aaaa: SBC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_sbcc_5c(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 t0 = (t0 - (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU)) -
      (rrpge_m_info.xr[REG_C] & 1U);
 rrpge_m_info.awf(t0);
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0101 111r rraa aaaa: SBC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_sbcc_5e(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 t0 = ((rrpge_m_info.xr[ra] & 0xFFFFU) - t0) -
      (rrpge_m_info.xr[REG_C] & 1U);
 rrpge_m_info.xr[ra]    = t0;
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0110 000r rraa aaaa: NEG adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_neg_60(void)
{
 auint op = rrpge_m_info.opc;
 (void)(rrpge_m_addr_read_table[op & 0x3FU](1U));
 rrpge_m_info.awf(0U - rrpge_m_info.xr[((op >> 6) & 0x7U)]);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0110 001r rraa aaaa: NEG rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_neg_62(void)
{
 auint op = rrpge_m_info.opc;
 rrpge_m_info.xr[((op >> 6) & 0x7U)] = 0U - rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0110 010r rraa aaaa: MUL C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_mulc_64(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 t0 = t0 * (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU);
 rrpge_m_info.awf(t0);
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 11U;
}
/* 0110 011r rraa aaaa: MUL C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_mulc_66(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 t0 = (rrpge_m_info.xr[ra] & 0xFFFFU) * t0;
 rrpge_m_info.xr[ra]    = t0;
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 11U;
}


/* 0110 100r rraa aaaa: SHR C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_shrc_68(void)
{
 auint op = rrpge_m_info.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t0 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = t1 << (16U - t0);
 rrpge_m_info.awf(t0 >> 16);
 rrpge_m_info.xr[REG_C] = t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0110 101r rraa aaaa: SHR C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_shrc_6a(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U) & 0xFU;
 auint t1 = rrpge_m_info.xr[ra] & 0xFFFFU;
 t0 = t1 << (16U - t0);
 rrpge_m_info.xr[ra]    = t0 >> 16;
 rrpge_m_info.xr[REG_C] = t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0110 110r rraa aaaa: SHL C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_shlc_6c(void)
{
 auint op = rrpge_m_info.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t0 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = t1 << t0;
 rrpge_m_info.awf(t0);
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0110 111r rraa aaaa: SHL C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_shlc_6e(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U) & 0xFU;
 auint t1 = rrpge_m_info.xr[ra] & 0xFFFFU;
 t0 = t1 << t0;
 rrpge_m_info.xr[ra]    = t0;
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0111 000r rraa aaaa: ASR C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_asrc_70(void)
{
 auint op = rrpge_m_info.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t0 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = (t1 << (16U - t0)) | ((0U - (t1 >> 15U)) << (31U - t0));
 rrpge_m_info.awf(t0 >> 16);
 rrpge_m_info.xr[REG_C] = t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0111 001r rraa aaaa: ASR C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_asrc_72(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U) & 0xFU;
 auint t1 = rrpge_m_info.xr[ra] & 0xFFFFU;
 t0 = (t1 << (16U - t0)) | ((0U - (t1 >> 15U)) << (31U - t0));
 rrpge_m_info.xr[ra]    = t0 >> 16;
 rrpge_m_info.xr[REG_C] = t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0111 010r rraa aaaa: MAC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_macc_74(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 t0 = (t0 * (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU)) +
      (rrpge_m_info.xr[REG_C] & 0xFFFFU);
 rrpge_m_info.awf(t0);
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 12U;
}
/* 0111 011r rraa aaaa: MAC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_macc_76(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U);
 t0 = ((rrpge_m_info.xr[ra] & 0xFFFFU) * t0) +
      (rrpge_m_info.xr[REG_C] & 0xFFFFU);
 rrpge_m_info.xr[ra]    = t0;
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 12U;
}


/* 0111 100r rraa aaaa: SRC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_srcc_78(void)
{
 auint op = rrpge_m_info.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t0 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = (t1 << (16U - t0)) | (rrpge_m_info.xr[REG_C] << 16);
 rrpge_m_info.awf(t0 >> 16);
 rrpge_m_info.xr[REG_C] = t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0111 101r rraa aaaa: SRC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_srcc_7a(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U) & 0xFU;
 auint t1 = rrpge_m_info.xr[ra] & 0xFFFFU;
 t0 = (t1 << (16U - t0)) | (rrpge_m_info.xr[REG_C] << 16);
 rrpge_m_info.xr[ra]    = t0 >> 16;
 rrpge_m_info.xr[REG_C] = t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 0111 110r rraa aaaa: SLC C:adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_slcc_7c(void)
{
 auint op = rrpge_m_info.opc;
 auint t1 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 auint t0 = rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFU;
 t0 = (t1 << t0) | (rrpge_m_info.xr[REG_C] & 0xFFFFU);
 rrpge_m_info.awf(t0);
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 0011 111r rraa aaaa: SLC C:rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_slcc_7e(void)
{
 auint ra = ((rrpge_m_info.opc >> 6) & 0x7U);
 auint t0 = rrpge_m_addr_read_table[rrpge_m_info.opc & 0x3FU](0U) & 0xFU;
 auint t1 = rrpge_m_info.xr[ra] & 0xFFFFU;
 t0 = (t1 << t0) | (rrpge_m_info.xr[REG_C] & 0xFFFFU);
 rrpge_m_info.xr[ra]    = t0;
 rrpge_m_info.xr[REG_C] = t0 >> 16;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 1000 000r rraa aaaa: MOV adr, special (SP, XM or XH) & SP ops */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_80(void)
{
 auint op = rrpge_m_info.opc;
 auint t0;
 if ((op & 0x0100U) == 0U){
  (void)(rrpge_m_addr_read_table[op & 0x3FU](1U));
  if ((op & 0x0080U) != 0U){  /* SP */
   rrpge_m_info.awf(rrpge_m_info.sp);
  }else{                      /* XM or XH in order by bit6 of opcode */
   rrpge_m_info.awf(rrpge_m_info.xmh[((op >> 6) & 0x1U)]);
  }
  rrpge_m_info.pc += rrpge_m_info.oaw;
  return rrpge_m_info.ocy + 3U;
 }else{
  if ((op & 0x0080U) != 0U){  /* MOV imx, SP (NOP) */
   rrpge_m_info.pc += 1U;
   return 3U;
  }else{
   t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
   rrpge_m_info.pc += rrpge_m_info.oaw;
   if ((op & 0x0040U) != 0U){ /* XEQ adr, SP */
    if (t0 == (rrpge_m_info.sp & 0xFFFFU)){
     rrpge_m_info.pc++;
     rrpge_m_info.ocy++;
    }
   }else{                     /* XUG adr, SP */
    if (t0 >  (rrpge_m_info.sp & 0xFFFFU)){
     rrpge_m_info.pc++;
     rrpge_m_info.ocy++;
    }
   }
   return rrpge_m_info.ocy + 4U;
  }
 }
}
/* 1000 001r rraa aaaa: MOV special, adr (SP, XM or XH) & SP ops */
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_82(void)
{
 auint op = rrpge_m_info.opc;
 auint t0;
 if ((op & 0x0100U) == 0U){
  t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
  if ((op & 0x0080U) != 0U){  /* SP */
   rrpge_m_info.sp = t0;
  }else{                      /* XM or XH in order by bit6 of opcode */
   rrpge_m_info.xmh[((op >> 6) & 0x1U)] = t0;
  }
  rrpge_m_info.pc += rrpge_m_info.oaw;
  return rrpge_m_info.ocy + 3U;
 }else{
  if ((op & 0x0080U) != 0U){  /* MOV SP, imx */
   rrpge_m_info.sp = op & 0x007FU;
   rrpge_m_info.pc += 1U;
   return 3U;
  }else{
   t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
   rrpge_m_info.pc += rrpge_m_info.oaw;
   if ((op & 0x0040U) != 0U){ /* XNE SP, adr */
    if (t0 != (rrpge_m_info.sp & 0xFFFFU)){
     rrpge_m_info.pc++;
     rrpge_m_info.ocy++;
    }
   }else{                     /* XUG SP, adr */
    if (t0 <  (rrpge_m_info.sp & 0xFFFFU)){
     rrpge_m_info.pc++;
     rrpge_m_info.ocy++;
    }
   }
   return rrpge_m_info.ocy + 4U;
  }
 }
}


/* 1000 010f rraa aaaa: JMR, JMA (absolute / relative jumps) */
RRPGE_M_FASTCALL static auint rrpge_m_op_jmp_84(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 if ((op & 0x00C0U) != 0U){ /* B, C or D should receive PC after jump */
  rrpge_m_info.xr[(op >> 6) & 0x3U] = rrpge_m_info.pc + rrpge_m_info.oaw;
 }
 if ((op & 0x0100U) == 0U){ /* JMR */
  rrpge_m_info.pc += t0;
 }else{                     /* JMA */
  rrpge_m_info.pc = t0;
 }
 return rrpge_m_info.ocy + 6U;
}

/* 0100 011r rraa aaaa: MOV rx, imx */
static const uint16 rrpge_m_op_mov_86_tb[16] = {
 0x0280U, 0x0028U, 0x0064U, 0x0078U, 0x03E8U, 0x00C8U, 0x2710U, 0x0118U,
 0x0140U, 0x0168U, 0x0190U, 0x01B8U, 0x01E0U, 0x0208U, 0x0230U, 0x0258U};
RRPGE_M_FASTCALL static auint rrpge_m_op_mov_86(void)
{
 auint op = rrpge_m_info.opc;
 auint ra = ((op >> 6) & 0x7U);
 auint q  = (op >> 4) & 3U;
 auint i  = (op & 0xFU);
 switch (q){
  case 0x2U:
   rrpge_m_info.xr[ra] = i | 0x0010U;
   break;
  case 0x3U:
   rrpge_m_info.xr[ra] = rrpge_m_op_mov_86_tb[i];
   break;
  default:
   q = (0U - ((op >> 4) & 1U)) & 0xFU;
   rrpge_m_info.xr[ra] = (i << 12) | (q << 8) | (q << 4) | (q);
   break;
 }
 rrpge_m_info.pc ++;
 return 3U;
}


/* 1000 100r rraa aaaa: AND adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_and_88(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 & rrpge_m_info.xr[((op >> 6) & 0x7U)]);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 1000 101r rraa aaaa: AND rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_and_8a(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.xr[((op >> 6) & 0x7U)] &= t0;
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 1000 11ii iiii iiii: JMS simm10 */
RRPGE_M_FASTCALL static auint rrpge_m_op_jms_8c(void)
{
 auint t0 = ((rrpge_m_info.opc & 0x03FFU) ^ 0x0200U) - 0x0200U;
 rrpge_m_info.pc += t0;
 return 4U;
}


/* 1001 ---- ---- ----: Supervisor mode ops (application fault) */
RRPGE_M_FASTCALL static auint rrpge_m_op_sv(void)
{
 rrpge_m_info.hlt = RRPGE_HLT_INVOP;
 return 0;
}


/* 1010 00ii iiaa aaaa: BTC adr, imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_op_btc_a0(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 & (~((auint)(1U) << ((op >> 6) & 0xFU))));
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}
/* 1010 10ii iiaa aaaa: BTS adr, imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_op_bts_a8(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](1U);
 rrpge_m_info.awf(t0 | ( ((auint)(1U) << ((op >> 6) & 0xFU))));
 rrpge_m_info.pc += rrpge_m_info.oaw;
 return rrpge_m_info.ocy + 4U;
}


/* 1010 01ii iiaa aaaa: XBC adr, imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_op_xbc_a4(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if ( (t0 & ((auint)(1U) << ((op >> 6) & 0xFU))) == 0U){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}
/* 1010 11ii iiaa aaaa: XBS adr, imm4 */
RRPGE_M_FASTCALL static auint rrpge_m_op_xbs_ac(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if ( (t0 & ((auint)(1U) << ((op >> 6) & 0xFU))) != 0U){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}


/* 1011 000r rraa aaaa: XST adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xst_b0(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if ((t0 & (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU)) != 0U){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}
/* 1011 001r rraa aaaa: XNS rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_xns_b2(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if ((t0 & (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU)) == 0U){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}


/* 1011 010r rraa aaaa: XSG adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xsg_b4(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if ( ((t0 + 0x8000U) & 0xFFFFU) >
      ((rrpge_m_info.xr[((op >> 6) & 0x7U)] + 0x8000U) & 0xFFFFU) ){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}
/* 1011 011r rraa aaaa: XSG rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_xsg_b6(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if ( ((t0 + 0x8000U) & 0xFFFFU) <
      ((rrpge_m_info.xr[((op >> 6) & 0x7U)] + 0x8000U) & 0xFFFFU) ){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}


/* 1011 100r rraa aaaa: XEQ adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xeq_b8(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if (t0 == (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU)){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}
/* 1011 101r rraa aaaa: XNE rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_xne_ba(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if (t0 != (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU)){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}


/* 1011 110r rraa aaaa: XUG adr, rx */
RRPGE_M_FASTCALL static auint rrpge_m_op_xug_bc(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if (t0 > (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU)){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}
/* 1011 111r rraa aaaa: XUG rx, adr */
RRPGE_M_FASTCALL static auint rrpge_m_op_xug_be(void)
{
 auint op = rrpge_m_info.opc;
 auint t0 = rrpge_m_addr_read_table[op & 0x3FU](0U);
 rrpge_m_info.pc += rrpge_m_info.oaw;
 if (t0 < (rrpge_m_info.xr[((op >> 6) & 0x7U)] & 0xFFFFU)){
  rrpge_m_info.pc++;
  rrpge_m_info.ocy++;
 }
 return rrpge_m_info.ocy + 4U;
}


/* 11-- ---- ---- ----: NOP */
RRPGE_M_FASTCALL static auint rrpge_m_op_nop(void)
{
 rrpge_m_info.pc++;
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
 &rrpge_m_op_or_10,   &rrpge_m_op_or_12,   &rrpge_m_op_div_14,  &rrpge_m_op_div_16,
 &rrpge_m_op_adc_18,  &rrpge_m_op_adc_1a,  &rrpge_m_op_sbc_1c,  &rrpge_m_op_sbc_1e,
 &rrpge_m_op_not_20,  &rrpge_m_op_not_22,  &rrpge_m_op_mul_24,  &rrpge_m_op_mul_26,
 &rrpge_m_op_shr_28,  &rrpge_m_op_shr_2a,  &rrpge_m_op_shl_2c,  &rrpge_m_op_shl_2e,
 &rrpge_m_op_asr_30,  &rrpge_m_op_asr_32,  &rrpge_m_op_mac_34,  &rrpge_m_op_mac_36,
 &rrpge_m_op_src_38,  &rrpge_m_op_src_3a,  &rrpge_m_op_slc_3c,  &rrpge_m_op_slc_3e,
 &rrpge_m_op_mov_40,  &rrpge_m_op_mov_42,  &rrpge_m_op_jfr_44,  &rrpge_m_op_mov_46,
 &rrpge_m_op_addc_48, &rrpge_m_op_addc_4a, &rrpge_m_op_subc_4c, &rrpge_m_op_subc_4e,
 &rrpge_m_op_xor_50,  &rrpge_m_op_xor_52,  &rrpge_m_op_divc_54, &rrpge_m_op_divc_56,
 &rrpge_m_op_adcc_58, &rrpge_m_op_adcc_5a, &rrpge_m_op_sbcc_5c, &rrpge_m_op_sbcc_5e,
 &rrpge_m_op_neg_60,  &rrpge_m_op_neg_62,  &rrpge_m_op_mulc_64, &rrpge_m_op_mulc_66,
 &rrpge_m_op_shrc_68, &rrpge_m_op_shrc_6a, &rrpge_m_op_shlc_6c, &rrpge_m_op_shlc_6e,
 &rrpge_m_op_asrc_70, &rrpge_m_op_asrc_72, &rrpge_m_op_macc_74, &rrpge_m_op_macc_76,
 &rrpge_m_op_srcc_78, &rrpge_m_op_srcc_7a, &rrpge_m_op_slcc_7c, &rrpge_m_op_slcc_7e,
 &rrpge_m_op_mov_80,  &rrpge_m_op_mov_82,  &rrpge_m_op_jmp_84,  &rrpge_m_op_mov_86,
 &rrpge_m_op_and_88,  &rrpge_m_op_and_8a,  &rrpge_m_op_jms_8c,  &rrpge_m_op_jms_8c,
 &rrpge_m_op_sv,      &rrpge_m_op_sv,      &rrpge_m_op_sv,      &rrpge_m_op_sv,
 &rrpge_m_op_sv,      &rrpge_m_op_sv,      &rrpge_m_op_sv,      &rrpge_m_op_sv,
 &rrpge_m_op_btc_a0,  &rrpge_m_op_btc_a0,  &rrpge_m_op_xbc_a4,  &rrpge_m_op_xbc_a4,
 &rrpge_m_op_bts_a8,  &rrpge_m_op_bts_a8,  &rrpge_m_op_xbs_ac,  &rrpge_m_op_xbs_ac,
 &rrpge_m_op_xst_b0,  &rrpge_m_op_xns_b2,  &rrpge_m_op_xsg_b4,  &rrpge_m_op_xsg_b6,
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
