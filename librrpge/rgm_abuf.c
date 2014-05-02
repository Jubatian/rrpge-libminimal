/**
**  \file
**  \brief     Audio buffer pointer calculation
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#include "rgm_abuf.h"


/* Calculates audio buffer pointers from the given configuration word (use
** ROPD: 0xBC2), and given half (0: first half; 1: second half; 0xD56 in
** ROPD; only uses bit 0 of this parameter!). Returns left and right sample
** offsets. */
void rrpge_m_abuf_getptr(auint cfg, auint hlf, auint* lpt, auint* rpt)
{
 hlf &= 1U;                   /* Only use bit0 of the halfbuffer parameter */

 if (hlf == 0){               /* Calculate for low half */

  *lpt = 0;                   /* Left or mono start pointer is always zero this case */
  if ((cfg & 0x4000U) != 0){  /* Stereo */
   if ((cfg & 0x2000U) != 0){ /* 1024 samples */
    *rpt = 2048U;
   }else{                     /* 512 samples */
    *rpt = 1024U;
   }
  }else{                      /* Mono */
   *rpt = 0U;
  }

 }else{                       /* Calculate for high half */

  if ((cfg & 0x4000U) != 0){  /* Stereo */
   if ((cfg & 0x2000U) != 0){ /* 1024 samples */
    *lpt = 1024U;
    *rpt = 3072U;
   }else{                     /* 512 samples */
    *lpt =  512U;
    *rpt = 1536U;
   }
  }else{                      /* Mono */
   if ((cfg & 0x2000U) != 0){ /* 1024 samples */
    *lpt = 1024U;
    *rpt = 1024U;
   }else{                     /* 512 samples */
    *lpt =  512U;
    *rpt =  512U;
   }
  }

 }

}
