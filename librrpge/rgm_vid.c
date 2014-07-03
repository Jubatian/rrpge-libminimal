/**
**  \file
**  \brief     Graphics FIFO and display manager.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.07.02
*/



#include "rgm_vid.h"
#include "rgm_grln.h"
#include "rgm_grop.h"



/* Based on the cycles needing emulation, process the video. Also calls back
** the line renderer as needed. */
void rrpge_m_vidproc(auint cy)
{
 auint fc;
 auint cm;
 auint dt;

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

  /* Process 400 cycles worth of FIFO & Accelerator operations. Note that
  ** rrpge_m_info.vac will stay nonzero as long as there is anything still
  ** processing. */

  fc = 400U;
  while (1){

   if (fc >= rrpge_m_info.vac){   /* Accelerator / FIFO op. can finish this line */
    fc -= rrpge_m_info.vac;
    rrpge_m_info.vac = 0U;
   }else{                      /* Can not finish, only some cycles drained */
    rrpge_m_edat->stat.ropd[0xD71U] = 0x0001U; /* FIFO busy (just to be sure) */
    rrpge_m_info.vac -= fc;
    break;                     /* Will finish in a further line */
   }

   /* Check for next Graphics FIFO command if any, and load it. */

   if ((rrpge_m_edat->stat.ropd[0xD5FU] & 1U) == 0U){ /* Not started */
    rrpge_m_edat->stat.ropd[0xD71U] = 0x0000U; /* FIFO is not busy */
    break;                     /* Nothing to do since FIFO is not started */
   }
   cm = rrpge_m_edat->stat.fram[(rrpge_m_edat->stat.ropd[0xD5DU] & 0x7FFEU) + 0U];
   dt = rrpge_m_edat->stat.fram[(rrpge_m_edat->stat.ropd[0xD5DU] & 0x7FFEU) + 1U];
   rrpge_m_edat->stat.ropd[0xD5DU] += 2U; /* Command & Data read, increment pointer */
   if (((rrpge_m_edat->stat.ropd[0xD5DU] ^ rrpge_m_edat->stat.ropd[0xD5CU]) & 0x7FFEU) == 0U){
    rrpge_m_edat->stat.ropd[0xD5FU] = 0U; /* FIFO drained, so started flag goes off */
   }
   rrpge_m_info.vac += 4U;

   /* Process the Graphics FIFO command */

   cm = (cm & 0x1FFU) | 0xE00U;
   if (cm < 0xF00U){ cm |= 0xEE0U; }
   rrpge_m_edat->stat.ropd[cm] = dt;
   if (cm == 0xEEFU){         /* Accelerator start */
    rrpge_m_info.vac += rrpge_m_grop_accel();
   }

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



/* Triggers a Graphics FIFO start. */
void  rrpge_m_vidfifost(void)
{
 if (((rrpge_m_edat->stat.ropd[0xD5DU] ^ rrpge_m_edat->stat.ropd[0xD5CU]) & 0x7FFEU) != 0U){
  rrpge_m_edat->stat.ropd[0xD5FU] |= 1U; /* If there is anything loaded in the FIFO, start it */
  rrpge_m_edat->stat.ropd[0xD71U] = 0x0001U; /* FIFO busy */
 }
}



/* Performs a Graphics FIFO store using the parameters in the ROPD. It may
** flag a FIFO start if necessary. Returns number of cycles the store takes */
auint rrpge_m_vidfifoop(void)
{
 auint cm = rrpge_m_edat->stat.ropd[0xD72U];

 /* Store current command & data words */

 rrpge_m_edat->stat.fram[(rrpge_m_edat->stat.ropd[0xD5CU] & 0x7FFEU) + 0U] =
   cm;
 rrpge_m_edat->stat.fram[(rrpge_m_edat->stat.ropd[0xD5CU] & 0x7FFEU) + 1U] =
   rrpge_m_edat->stat.ropd[0xD73U];

 /* Increment write pointer */

 rrpge_m_edat->stat.ropd[0xD5CU] += 2U;
 if (((rrpge_m_edat->stat.ropd[0xD5DU] ^ rrpge_m_edat->stat.ropd[0xD5CU]) & 0x7FFEU) == 0U){
  rrpge_m_info.hlt |= RRPGE_HLT_GRAPHICS; /* FIFO overflow */
 }

 /* Check command word for accelerator trigger */

 if ((cm & 0x011FU) == 0x000FU){          /* Accelerator start trigger */
  rrpge_m_edat->stat.ropd[0xD5FU] |= 1U;  /* Autostart FIFO */
  rrpge_m_edat->stat.ropd[0xD71U] = 0x0001U; /* FIFO busy */
 }

 /* Increment command word */

 cm = (cm & 0xFE00U) | ((cm + 1U) & 0x01FFU);
 rrpge_m_edat->stat.ropd[0xD72U] = cm;

 /* Takes 5 cycles according to allowing emulating later FIFO DMA accesses this way. */

 return 5U;
}
