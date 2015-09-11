/**
**  \file
**  \brief     Audio output manager, rrpge_getaudio() implementation.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.11
*/


#include "rgm_aud.h"
#include "rgm_halt.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* Based on the cycles needing emulation, processes audio into the internal
** audio buffer, flagging the 512 sample passed event as required. */
void rrpge_m_audproc(auint cy)
{
 uint16* stat = &(rrpge_m_edat->st.stat[0]);
 auint lof = (stat[RRPGE_STA_UPA_A + 0x4U] & 0xFFFFU) << 4;
 auint rof = (stat[RRPGE_STA_UPA_A + 0x5U] & 0xFFFFU) << 4;
 auint msk = (stat[RRPGE_STA_UPA_A + 0x6U] & 0xFFFFU) << 4;
 auint t;

 lof &= msk;        /* Part to fetch from the start offset */
 rof &= msk;
 msk ^= PRAMS - 1U; /* Invert mask (staying in appropriate range) to use on next read offset */

 /* Consume the provided number of cycles */

 rrpge_m_info.atc -= cy;

 /* Until the remaining cycles to next audio event is negative or zero,
 ** process */

 while ( ((rrpge_m_info.atc & 0x80000000U) != 0U) ||
         (rrpge_m_info.atc == 0U) ){

  /* One audio tick will be processed */

  rrpge_m_info.atc += (rrpge_m_edat->clk) / 48000U;
  rrpge_m_edat->clkf += (rrpge_m_edat->clk) % 48000U;
  if (rrpge_m_edat->clkf >= 48000U){
   rrpge_m_edat->clkf -= 48000U;
   rrpge_m_info.atc ++;
  }

  /* Load samples from data memory into the internal double buffer */

  t = stat[RRPGE_STA_UPA_A + 2U] & 0xFFFFU;
  rrpge_m_edat->audl[(rrpge_m_edat->audp) & 0x3FFU] =
    ( rrpge_m_edat->st.pram[lof | ((t >> 2) & msk)] >> (((t & 3U) ^ 3U) << 3) ) & 0xFFU;
  rrpge_m_edat->audr[(rrpge_m_edat->audp) & 0x3FFU] =
    ( rrpge_m_edat->st.pram[rof | ((t >> 2) & msk)] >> (((t & 3U) ^ 3U) << 3) ) & 0xFFU;

  /* Increment pointers */

  rrpge_m_edat->audp ++;
  stat[RRPGE_STA_UPA_A + 3U] = (stat[RRPGE_STA_UPA_A + 3U] + 1U) & 0xFFFFU;
  if ( (stat[RRPGE_STA_UPA_A + 3U] & 0xFFFFU) ==
       (stat[RRPGE_STA_UPA_A + 7U] & 0xFFFFU) ){
   stat[RRPGE_STA_UPA_A + 3U] = 0U;
   stat[RRPGE_STA_UPA_A + 2U] = (stat[RRPGE_STA_UPA_A + 2U] + 1U) & 0xFFFFU;
  }

  /* Increment 187.5Hz clock */

  stat[RRPGE_STA_VARS + 0x14U] = (stat[RRPGE_STA_VARS + 0x14U] + 1U) & 0xFFFFU;
  if ((stat[RRPGE_STA_VARS + 0x14U] & 0xFFU) == 0U){
   stat[RRPGE_STA_UPA_A + 1U] =
       ((stat[RRPGE_STA_UPA_A + 1U] + 0x1U) & 0xFF00U) +
       ((stat[RRPGE_STA_VARS + 0x14U] >> 8) & 0xFFU);
  }

  /* Generate an event every 512 output (48KHz) samples */

  if ((rrpge_m_edat->audp & 0x1FFU) == 0U){
   rrpge_m_edat->aco ++;
   rrpge_m_halt_set(rrpge_m_edat, RRPGE_HLT_AUDIO);
  }

 }

}



/* Get audio events and streams - implementation of RRPGE library function */
rrpge_iuint rrpge_getaudio(rrpge_object_t* hnd, rrpge_uint8* lbuf, rrpge_uint8* rbuf)
{
 auint r;
 auint i;
 uint8 const* pl;
 uint8 const* pr;

 if (!rrpge_m_halt_isset(hnd, RRPGE_HLT_AUDIO)){ return 0; } /* No audio event present */

 r = hnd->aco;
 hnd->aco = 0U;
 rrpge_m_halt_clr(hnd, RRPGE_HLT_AUDIO);

 /* Fill in the 512 sample target buffers */

 i  = ((hnd->audp) & 0x200U) ^ 0x200U; /* Select not currently filled half */
 pl = &(hnd->audl[i]);
 pr = &(hnd->audr[i]);

 for (i = 0; i < 512U; i++){
  lbuf[i] = pl[i] & 0xFFU;
  rbuf[i] = pr[i] & 0xFFU;
 }

 /* OK, all done */

 return r;
}
