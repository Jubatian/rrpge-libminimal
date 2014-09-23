/**
**  \file
**  \brief     Graphics FIFO and display manager.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.23
*/



#include "rgm_vid.h"
#include "rgm_grln.h"



/* Based on the cycles needing emulation, process the video. Also calls back
** the line renderer as needed. */
void rrpge_m_vidproc(auint cy)
{
 auint fc;
 auint cm;
 auint dt;

 /* Consume the provided number of cycles */

 rrpge_m_info.vlc -= cy;

 /* Until the remaining cycle count is negative or zero, process lines. */

 while ( ((rrpge_m_info.vlc & 0x80000000U) != 0U) ||
         (rrpge_m_info.vlc == 0U) ){

  /* A line worth of cycles will be consumed */

  rrpge_m_info.vlc += 400U;

  /* Render the current line */

  if (((rrpge_m_edat->rena) & 0x2U) != 0U){ /* Rendering enabled */
   rrpge_m_grln();
  }

  /* Increment counters */

  rrpge_m_info.vln = (rrpge_m_info.vln + 1U) & 0xFFFFU;
  if ((rrpge_m_info.vln >= 400U) && ((rrpge_m_info.vln & 0x8000U) == 0U)){
   rrpge_m_info.vln = 0x10000U + 400U - RRPGE_M_VLN;
   rrpge_m_edat->rena = (rrpge_m_edat->rena & (~0x2U)) | /* Transfer requested render state */
                        ((rrpge_m_edat->rena & (0x1U)) << 1);
   rrpge_m_info.hlt |= RRPGE_HLT_FRAME;
  }

 }

}
