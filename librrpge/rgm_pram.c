/**
**  \file
**  \brief     Peripheral RAM interface.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.03
*/


#include "rgm_pram.h"
#include "rgm_stat.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS


/* Write masks for the PRAM interface registers */
static const uint16 rrpge_m_pram_wms[8] = {0x01FFU, 0xFFFFU, 0x01FFU, 0xFFFFU, 0x000FU, 0x0000U, 0x0000U, 0x0000U};

/* Data size dependent address low masks (1 bit, 2 bit, 4 bit, 8 bit, 16 bit ...) */
static const uint8  rrpge_m_pram_ams[8] = {0x1FU, 0x1EU, 0x1CU, 0x18U, 0x10U, 0x10U, 0x10U, 0x10U};

/* Data size dependent data masks (1 bit, 2 bit, 4 bit, 8 bit, 16 bit ...) */
static const uint16 rrpge_m_pram_dms[8] = {0x0001U, 0x0003U, 0x000FU, 0x00FFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU};



/* State: PRAM interface (0x0E0 - 0x0FF), getter */
static auint rrpge_m_pram_stat_get(rrpge_object_t* hnd, auint adr)
{
 /* The memory-mapped PRAM is not provided through here, just the interface
 ** registers as they are. Simple. */

 if (adr < 0x20){ return (hnd->prm.sta[adr]) & 0xFFFFU; }
 return 0U;
}



/* State: PRAM interface (0x0E0 - 0x0FF), setter */
static auint rrpge_m_pram_stat_set(rrpge_object_t* hnd, auint adr, auint val)
{
 /* The memory-mapped PRAM is not provided through here, just the interface
 ** registers as they are. Simple. */

 if (adr < 0x20){
  hnd->prm.sta[adr] = val & (auint)(rrpge_m_pram_wms[adr & 0x7U]);
  return (hnd->prm.sta[adr]);
 }
 return 0U;
}



/* Initializes PRAM emulation adding the appropriate handlers to the state
** manager. */
void rrpge_m_pram_init(void)
{
 rrpge_m_stat_addhandler(&rrpge_m_pram_stat_get, &rrpge_m_pram_stat_set,
                         RRPGE_STA_UPA_P, 32U);
 rrpge_m_stat_setmmcell(RRPGE_STA_UPA_P + 0x06U, 1U);
 rrpge_m_stat_setmmcell(RRPGE_STA_UPA_P + 0x07U, 1U);
 rrpge_m_stat_setmmcell(RRPGE_STA_UPA_P + 0x0EU, 1U);
 rrpge_m_stat_setmmcell(RRPGE_STA_UPA_P + 0x0FU, 1U);
 rrpge_m_stat_setmmcell(RRPGE_STA_UPA_P + 0x16U, 1U);
 rrpge_m_stat_setmmcell(RRPGE_STA_UPA_P + 0x17U, 1U);
 rrpge_m_stat_setmmcell(RRPGE_STA_UPA_P + 0x1EU, 1U);
 rrpge_m_stat_setmmcell(RRPGE_STA_UPA_P + 0x1FU, 1U);
}



/* Operates the memory mapped Peripheral RAM interface for Reads. Only the
** low 5 bits of the address are used. Generates Peripheral bus stalls if
** necessary. */
RRPGE_M_FASTCALL auint rrpge_m_pram_read(rrpge_object_t* hnd, auint adr, auint rmw)
{
 auint   r;
 auint   a;
 auint   s;
 auint   m;
 auint*  stat;

 stat = &(hnd->prm.sta[adr & 0x18U]);
 adr &= 0x07U;

 if ((adr & 0x6U) != 0x6U){
  return stat[adr];
 }

 /* Access memory */

 a = ((stat[0] & 0xFFFFU) << 16) + (stat[1] & 0xFFFFU);
 s = stat[4] & 0x7U;
 r = (a >> 5) & (PRAMS - 1U);
 hnd->prm.pia = r;        /* Save original address value for possible write */
 r = hnd->st.pram[r];
 hnd->prm.pid = r;        /* Save original PRAM cell contents for possible write */
 m = rrpge_m_pram_dms[s];
 s = (0x1FU - a) & rrpge_m_pram_ams[s];
 m <<= s;
 hnd->prm.pim = m;        /* Save data mask for possible write */
 hnd->prm.pis = s;        /* Save shift amount for possible write */
 r = (r & m) >> s;

 rrpge_m_pram_cys_add(hnd, 2U); /* Add PRAM stall cycles to Peripheral bus stall */

 if ((adr & 0x7U) == 0x7U){ /* Post - increment */
  if (((stat[4] & 0x8U) == 0U) || rmw){
   a += ((stat[2] & 0xFFFFU) << 16) + (stat[3] & 0xFFFFU);
   stat[0] = (a >> 16) & 0xFFFFU;
   stat[1] = (a      ) & 0xFFFFU;
  }
 }

 return r;
}



/* Operates the memory mapped Peripheral RAM interface for Writes. Only the
** low 5 bits of the address are used. Generates Peripheral bus stalls if
** necessary. Note that it assumes a Read (rrpge_m_pramread() call) happened
** before. */
RRPGE_M_FASTCALL void  rrpge_m_pram_write(rrpge_object_t* hnd, auint adr, auint val)
{
 auint m;
 auint s;

 adr &= 0x1FU;

 if ((adr & 0x6U) != 0x6U){
  hnd->prm.sta[adr] = val & rrpge_m_pram_wms[adr & 0x7U];
  return;
 }

 /* Access memory */

 m = hnd->prm.pim;        /* Data mask saved at read */
 s = hnd->prm.pis;        /* Shift saved at read */
 val = (val << s);
 hnd->st.pram[hnd->prm.pia] = (hnd->prm.pid & (~m)) | (val & m);

 rrpge_m_pram_cys_add(hnd, 2U); /* Add PRAM stall cycles to Peripheral bus stall */
}



/* Peripheral bus: Consume stall cycles, returns remaining cycles of cy */
auint rrpge_m_pram_cys_cons(rrpge_object_t* hnd, auint cy)
{
 if (hnd->prm.cys > cy){

  hnd->prm.cys -= cy;
  return 0U;

 }else{

  cy -= hnd->prm.cys;
  hnd->prm.cys = 0U;
  return cy;

 }
}



/* Peripheral bus: Clear all stall cycles */
void  rrpge_m_pram_cys_clr(rrpge_object_t* hnd)
{
 hnd->prm.cys = 0U;
}
