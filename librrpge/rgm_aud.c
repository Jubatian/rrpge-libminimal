/**
**  \file
**  \brief     Audio output manager, rrpge_getaudio() implementation.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.23
*/


#include "rgm_aud.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* Based on the cycles needing emulation, processes audio into the internal
** audio buffer, flagging the 512 sample passed event as required. */
void rrpge_m_audproc(auint cy)
{
 auint lof = (auint)(rrpge_m_edat->st.stat[RRPGE_STA_UPA_A + 0x4U]) << 4;
 auint rof = (auint)(rrpge_m_edat->st.stat[RRPGE_STA_UPA_A + 0x5U]) << 4;
 auint msk = (auint)(rrpge_m_edat->st.stat[RRPGE_STA_UPA_A + 0x6U]) << 4;
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

  rrpge_m_info.atc += (RRPGE_M_OSC + 24000U) / 48000U;

  /* Load samples from data memory into the internal double buffer */

  t = rrpge_m_edat->st.stat[RRPGE_STA_UPA_A + 0x2U];
  rrpge_m_edat->audl[(rrpge_m_edat->audp) & 0x3FFU] =
    (uint8)( rrpge_m_edat->st.pram[lof | ((t >> 2) & msk)] >> (((t & 3U) ^ 3U) << 3) );
  rrpge_m_edat->audr[(rrpge_m_edat->audp) & 0x3FFU] =
    (uint8)( rrpge_m_edat->st.pram[rof | ((t >> 2) & msk)] >> (((t & 3U) ^ 3U) << 3) );

  /* Increment pointers */

  rrpge_m_edat->audp ++;
  rrpge_m_edat->st.stat[RRPGE_STA_UPA_A + 0x3U] ++;
  if (rrpge_m_edat->st.stat[RRPGE_STA_UPA_A + 0x3U] ==
      rrpge_m_edat->st.stat[RRPGE_STA_UPA_A + 0x7U]){
   rrpge_m_edat->stat.ropd[RRPGE_STA_UPA_A + 3U] = 0U;
   rrpge_m_edat->stat.ropd[RRPGE_STA_UPA_A + 2U] ++;
  }

  /* Increment 187.5Hz clock */

  rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x14U] ++;
  if ((rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x14U] & 0xFFU) == 0U){
   rrpge_m_edat->st.stat[RRPGE_STA_UPA_A + 1U] =
       ((rrpge_m_edat->st.stat[RRPGE_STA_UPA_A + 1U] + 0x1) & 0xFF00U) +
       (rrpge_m_edat->st.stat[RRPGE_STA_VARS + 14U] >> 8);
  }

  /* Generate an event every 512 output (48KHz) samples */

  if ((rrpge_m_edat->audp & 0x1FFU) == 0U){
   rrpge_m_edat->aco ++;
   rrpge_m_info.hlt |= RRPGE_HLT_AUDIO;
  }

 }

}



/* Get audio events and streams - implementation of RRPGE library function */
rrpge_uint32 rrpge_getaudio(rrpge_object_t* hnd, rrpge_uint8* lbuf, rrpge_uint8* rbuf)
{
 auint r;
 auint i;
 uint8 const* pl;
 uint8 const* pr;

 if (((hnd->hlt) & RRPGE_HLT_AUDIO) == 0U){ return 0; } /* No audio event present */

 r = hnd->aco;
 hnd->aco = 0U;
 hnd->hlt &= ~(auint)(RRPGE_HLT_AUDIO);

 /* Fill in the 512 sample target buffers */

 i  = ((hnd->audp) & 0x200U) ^ 0x200U; /* Select not currently filled half */
 pl = &(hnd->audl[i]);
 pr = &(hnd->audr[i]);

 for (i = 0; i < 512U; i++){
  lbuf[i] = pl[i];
  rbuf[i] = pr[i];
 }

 /* OK, all done */

 return r;
}
