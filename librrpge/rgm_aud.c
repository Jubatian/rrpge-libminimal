/**
**  \file
**  \brief     Audio output manager, rrpge_getaudio() implementation.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.26
*/


#include "rgm_aud.h"



/* Based on the cycles needing emulation, processes audio into the internal
** audio buffer, flagging the 512 sample passed event as required. */
void rrpge_m_audproc(auint cy)
{
 auint lof = (auint)(rrpge_m_edat->stat.ropd[0xE08U]) << 4;
 auint rof = (auint)(rrpge_m_edat->stat.ropd[0xE09U]) << 4;
 auint msk = (auint)(rrpge_m_edat->stat.ropd[0xE0AU]) << 4;
 auint t;

 lof &= msk;      /* Part to fetch from the start offset */
 rof &= msk;
 msk ^= 0xFFFFFU; /* Invert mask (staying in appropriate range) to use on next read offset */

 /* Consume the provided number of cycles */

 rrpge_m_info.auc -= cy;

 /* Until the remaining cycles to next audio event is negative or zero,
 ** process */

 while ( ((rrpge_m_info.auc & 0x80000000U) != 0U) ||
         (rrpge_m_info.auc == 0U) ){

  /* One audio tick will be processed */

  rrpge_m_info.auc += (RRPGE_M_OSC + 24000U) / 48000U;

  /* Load samples from data memory into the internal double buffer */

  t = rrpge_m_edat->stat.ropd[0xE0CU];
  rrpge_m_edat->audl[(rrpge_m_edat->audp) & 0x3FFU] =
    (uint8)( rrpge_m_edat->stat.dram[lof | ((t >> 1) & msk)] >> (((t & 1U) ^ 1U) << 3) );
  rrpge_m_edat->audr[(rrpge_m_edat->audp) & 0x3FFU] =
    (uint8)( rrpge_m_edat->stat.dram[rof | ((t >> 1) & msk)] >> (((t & 1U) ^ 1U) << 3) );

  /* Increment pointers */

  rrpge_m_edat->audp ++;
  rrpge_m_edat->stat.ropd[0xE0DU] ++;
  if (rrpge_m_edat->stat.ropd[0xE0DU] == rrpge_m_edat->stat.ropd[0xE0BU]){
   rrpge_m_edat->stat.ropd[0xE0DU] = 0U;
   rrpge_m_edat->stat.ropd[0xE0CU] ++;
  }

  /* Generate an event every 512 output (48KHz) samples */

  if ((rrpge_m_edat->audp & 0x1FF) == 0U){
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

 if ((hnd->aco) == 0U){ return 0; }    /* No audio event present */

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
