/**
**  \file
**  \brief     Graphics FIFO and display manager.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.26
*/



#include "rgm_vid.h"
#include "rgm_grln.h"
#include "rgm_grop.h"
#include "rgm_grcy.h"



/* Based on the cycles needing emulation, process the video. Also calls back
** the line renderer as needed. */
void rrpge_m_vidproc(auint cy)
{
 auint fc;
 auint cm;
 auint dt;
 auint vl;

 /* Consume the provided number of cycles */

 rrpge_m_info.vlc -= cy;

 /* Until the remaining cycle count is negative or zero, process lines and
 ** Graphics FIFO operations */

 while ( ((rrpge_m_info.vlc & 0x80000000U) != 0U) ||
         (rrpge_m_info.vlc == 0U) ){

  /* A line worth of cycles will be consumed */

  rrpge_m_info.vlc += 400U;

  /* Render the current line */

  if (((rrpge_m_edat->rena) & 0x2U) != 0U){ /* Rendering enabled */
   rrpge_m_grln();
  }

  /* Process 400 cycles worth of FIFO & Accelerator operations */

  fc = 400U;
  while (1){

   if (fc > rrpge_m_info.vac){ /* Accelerator / FIFO op. can finish this line */
    fc -= rrpge_m_info.vac;
    rrpge_m_info.vac = 0U;
   }else{                      /* Can not finish, only some cycles drained */
    rrpge_m_info.vac -= fc;
    break;                     /* Will finish in a further line */
   }

   /* Check for next Graphics FIFO command if any, and load it. */

   if ((rrpge_m_info.frq != 0U){  /* FIFO operation start request */
    if (((rrpge_m_edat->stat.ropd[0xD5DU] ^ rrpge_m_edat->stat.ropd[0xD5CU]) & 0x7FFEU) != 0U){
     rrpge_m_edat->stat.ropd[0xD5FU] |= 1U; /* If there is anything loaded in the FIFO, start it */
    }
    rrpge_m_info.frq = 0U;
   }
   if ((rrpge_m_edat->stat.ropd[0xD5FU] & 1U) == 0U){
    break;                     /* Nothing to do since FIFO is not started */
   }
   cm = rrpge_m_edat->stat.fram[(rrpge_m_edat->stat.ropd[0xD5DU] & 0x7FFEU) + 0U];
   dt = rrpge_m_edat->stat.fram[(rrpge_m_edat->stat.ropd[0xD5DU] & 0x7FFEU) + 1U];
   rrpge_m_edat->stat.ropd[0xD5DU] += 2U; /* Command & Data read, increment pointer */
   if (((rrpge_m_edat->stat.ropd[0xD5DU] ^ rrpge_m_edat->stat.ropd[0xD5CU]) & 0x7FFEU) == 0U){
    rrpge_m_edat->stat.ropd[0xD5FU] = 0U; /* FIFO drained */
   }
   rrpge_m_info.vac += 4U;

   /* Process the Graphics FIFO command */

   if ((cm & 0x8000U) == 0U){  /* Beam wait */

    cm = (cm + 0x0200U) & 0x03FFU; /* Convert to 0 - 1023 range; 512: line 0 */
    dt = (dt + 0x0200U) & 0x03FFU; /* Convert to 0 - 1023 range; 512: line 0 */
    vl = (rrpge_m_info.vln + 0x0200U) & 0x03FFU;

    /* cm: (to be) already reached line */
    /* dt: (to be) not yet reached line */

    if (cm <= dt){
     if ( (cm == dt) ||        /* Never met due to equality */
          (cm >  (512U + 399U)) || /* Never met since already reached is beyond last valid */
          (dt <= (512U - (RRPGE_M_VLN - 400U)) ){ /* Never met since not yet reached is before 1st line */
      rrpge_m_info.vac  = 0xFFFFFFFFU;
     }else if (vl <  cm){      /* Need to reach the line */
      rrpge_m_info.vac += (cm - vl) * 400U;
     }else if (vl >= dt){      /* Need to go around, reaching in next frame */
      rrpge_m_info.vac += (cm - vl + RRPGE_M_VLN) * 400U;
     }else{                    /* Condition met, no wait */
     }
    }else{
     if ( (vl <  cm) &&
          (vl >= dt) ){        /* Need to get out the dt <= vl < cm range */
      if       (cm <= (512U + 399U)){ /* Will reach out at the bottom */
       rrpge_m_info.vac += (cm - vl) * 400U;
      }else if (dt >  (512U - (RRPGE_M_VLN - 400U))){ /* Will reach out at the top */
       rrpge_m_info.vac += (400U + 512U - vl) * 400U;
      }else{                   /* Range covers all lines */
       rrpge_m_info.vac  = 0xFFFFFFFFU;
      }
     }else{                    /* Condition met, no wait */
     }
    }

   }else{                      /* Graphics register set */

    cm = (cm & 0x1FFU) | 0xE00U;
    if (cm < 0xF00U){ cm |= 0xEE0U; }
    rrpge_m_edat->stat.ropd[cm] = dt;
    if (cm == 0xEEFU){         /* Accelerator start */
     rrpge_m_info.vac += rrpge_m_graccy();
     rrpge_m_grop_accel();
    }

   }

  }

  /* Increment counters */

  rrpge_m_info.vln = (rrpge_m_info.vln + 1U) & 0xFFFFU;
  if (rrpge_m_info.vln >= 400U){
   rrpge_m_info.vln = 0x10000U + 400U - RRPGE_M_VLN;
  }

 }

}



/* Performs a Graphics FIFO store using the parameters in the ROPD. It may
** flag a FIFO start if necessary (setting rrpge_m_info's "frq" member). */
void rrpge_m_vidfifoop(void)
{
 auint cm = rrpge_m_edat->stat.ropd[0xEC7U];

 /* Store current command & data words */

 rrpge_m_edat->stat.fram[(rrpge_m_edat->stat.ropd[0xD5CU] & 0x7FFEU) + 0U] =
   rrpge_m_edat->stat.ropd[0xEC6U];
 rrpge_m_edat->stat.fram[(rrpge_m_edat->stat.ropd[0xD5CU] & 0x7FFEU) + 1U] =
   cm;

 /* Increment write pointer */

 rrpge_m_edat->stat.ropd[0xD5CU] += 2U;
 if (((rrpge_m_edat->stat.ropd[0xD5DU] ^ rrpge_m_edat->stat.ropd[0xD5CU]) & 0x7FFEU) == 0U){
  rrpge_m_info.hlt |= RRPGE_M_GRAPHICS; /* FIFO overflow */
 }

 /* Check command word for accelerator trigger */

 if (((cm & 0x01FFU) & 0x011FU) == 0x000FU){
  rrpge_m_info.frq = 1U;                /* Autostart FIFO */
 }

 /* Increment command word */

 cm = (cm & 0xFE00U) | ((cm + 1U) & 0x01FFU);
 rrpge_m_edat->stat.ropd[0xEC7U] = cm;
}