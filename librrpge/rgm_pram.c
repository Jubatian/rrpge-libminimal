/**
**  \file
**  \brief     Peripheral RAM interface.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.24
*/


#include "rgm_pram.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS


/* Write masks for the PRAM interface registers */
static const uint16 rrpge_m_pram_wms[6] = {0x01FFU, 0xFFFFU, 0x01FFU, 0xFFFFU, 0x0007U, 0x0000U};

/* Data size dependent address low masks (1 bit, 2 bit, 4 bit, 8 bit, 16 bit ...) */
static const uint8  rrpge_m_pram_ams[8] = {0x1FU, 0x1EU, 0x1CU, 0x18U, 0x10U, 0x10U, 0x10U, 0x10U};

/* Data size dependent data masks (1 bit, 2 bit, 4 bit, 8 bit, 16 bit ...) */
static const uint16 rrpge_m_pram_dms[8] = {0x0001U, 0x0003U, 0x000FU, 0x00FFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU};



/* Operates the memory mapped Peripheral RAM interface for Reads. Only the
** low 5 bits of the address are used. Generates Peripheral bus stalls if
** necessary. */
RRPGE_M_FASTCALL auint rrpge_m_pramread(auint adr)
{
 auint   r;
 auint   a;
 auint   s;
 auint   m;
 uint16* stat;

 stat = &(rrpge_m_edat->st.stat[RRPGE_STA_UPA_P + (adr & 0x18U)]);
 adr &= 0x07U;

 if ((adr & 0x6U) != 0x6U){
  return stat[adr];
 }

 /* Access memory */

 a = ((auint)(stat[0]) << 16) + ((auint)(stat[1]));
 s = stat[4] & 0x7U;
 r = (a >> 5) & (PRAMS - 1U);
 rrpge_m_info.pia = r;        /* Save original address value for possible write */
 r = rrpge_m_edat->st.pram[r];
 rrpge_m_info.pid = r;        /* Save original PRAM cell contents for possible write */
 m = rrpge_m_pram_dms[s];
 s = (0x1FU - a) & rrpge_m_pram_ams[s];
 m <<= s;
 rrpge_m_info.pim = m;        /* Save data mask for possible write */
 rrpge_m_info.pis = s;        /* Save shift amount for possible write */
 r = (r & m) >> s;

 rrpge_m_info.cys += 2U;      /* Add PRAM stall cycles to Peripheral bus stall */
 rrpge_m_info.ocy += 1U;      /* Add PRAM access stall to CPU stall */

 if ((adr & 0x7U) == 0x7U){   /* Post - increment */
  a += ((auint)(stat[2]) << 16) + ((auint)(stat[3]));
  stat[0] = (uint16)(a >> 16);
  stat[1] = (uint16)(a);
 }

 return r;
}



/* Operates the memory mapped Peripheral RAM interface for Writes. Only the
** low 5 bits of the address are used. Generates Peripheral bus stalls if
** necessary. Note that it assumes a Read (rrpge_m_pramread() call) happened
** before, rolling back it's assumed increment before writing in the PRAM (an
** alternate way for emulating the R-M-W line of the CPU for this case) */
RRPGE_M_FASTCALL void  rrpge_m_pramwrite(auint adr, auint val)
{
 auint m;
 auint s;

 adr &= 0x1FU;

 if ((adr & 0x6U) != 0x6U){
  rrpge_m_edat->st.stat[RRPGE_STA_UPA_P + adr] = val & rrpge_m_pram_wms[adr & 0x7U];
  return;
 }

 /* Access memory */

 m = rrpge_m_info.pim;        /* Data mask saved at read */
 s = rrpge_m_info.pis;        /* Shift saved at read */
 val = (val << s);
 rrpge_m_edat->st.pram[rrpge_m_info.pia] = (rrpge_m_info.pid & (~m)) | (val & m);

 rrpge_m_info.cys += 2U;      /* Add PRAM stall cycles to Peripheral bus stall */
}
