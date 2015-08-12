/**
**  \file
**  \brief     Graphics FIFO and display manager.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.11
*/



#include "rgm_vid.h"
#include "rgm_grln.h"
#include "rgm_halt.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* Based on the cycles needing emulation, process the video, and the Graphics
** Display Generator's Display List clear function. Also calls back the line
** renderer as needed. */
void  rrpge_m_vidproc(auint cy)
{
 auint a;
 auint o;
 auint i;
 auint j;
 auint c;
 auint s;
 auint t;
 uint16* stat = &(rrpge_m_edat->st.stat[0]);

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

  rrpge_m_info.vln ++;
  rrpge_m_info.vln &= 0xFFFFU;
  if ((rrpge_m_info.vln >= 400U) && ((rrpge_m_info.vln & 0x8000U) == 0U)){
   rrpge_m_info.vln = 0x10000U + 400U - RRPGE_M_VLN;
   rrpge_m_edat->rena = (rrpge_m_edat->rena & (~0x2U)) | /* Transfer requested render state */
                        ((rrpge_m_edat->rena & (0x1U)) << 1);
   rrpge_m_halt_set(rrpge_m_edat, RRPGE_HLT_FRAME);
  }

  /* Before starting next frame (line counter is zero, so next iteration will
  ** result in rendering a line), if the flags request so, process the display
  ** list clear, and update the latch, so completing the double buffering
  ** assistance. */

  if ( (rrpge_m_info.vln == 0U) &&
       ((stat[RRPGE_STA_UPA_G + 0x7U] & 0x8000U) != 0U) ){

   /* Prepare for clear */

   i  = stat[RRPGE_STA_VARS + 0x15U];       /* Display list def. latch */
   a  = ((i & 0xF000U) << 4) & (PRAMS - 1U);   /* PRAM bank */
   o  = ((i & 0x0FFCU) << 4);               /* Start offset */
   j  = 1600U << (i & 3U);                  /* Display list size limit */
   i  = stat[RRPGE_STA_UPA_G + 0x3U];       /* Display list clear controls */
   s  = (i >> 11) & 0x1FU;                  /* Initial skip */
   j -= s;
   o += s;
   s  = (i >> 6) & 0x1FU;                   /* Skip amount / streak */
   c  = (i     ) & 0x3FU;                   /* Clear amount / streak */
   i  = 9600U;                              /* Allowed cycles */

   /* Clear */

   if (c != 0U){
    do{
     for (t = 0U; t < c; t++){ /* Clear */
      rrpge_m_edat->st.pram[a + o] = 0U;
      o  = (o + 1U) & 0xFFFFU;
      i --;
      j --;
      if ((i == 0U) || (j == 0U)){ break; }
     }
     if (j <= s){              /* Skip */
      j  = 0U;
     }else{
      j -= s;
      o  = (o +  s) & 0xFFFFU;
     }
    }while ((i != 0U) && (j != 0U));
   }

   /* Update latch, and clear flags */

   stat[RRPGE_STA_UPA_G + 0x7U] = 0U;       /* Clear flags */
   stat[RRPGE_STA_VARS + 0x15U] = stat[RRPGE_STA_UPA_G + 0x6U];

   /* Clear FIFO suspend, so it may continue processing */

   stat[RRPGE_STA_UPA_GF + 0x1U] &= ~2U; /* Graphics FIFO suspend ends */

  }

 }

}



/* Operates the memory mapped interface of the Graphics Display Generator.
** Only the low 4 bits of the address are used. Only low 16 bits of value are
** used. */
void  rrpge_m_vidwrite(auint adr, auint val)
{
 uint16* stat = &(rrpge_m_edat->st.stat[0]);

 adr = adr & 0xFU;

 switch (adr){

  case 0x0U:
  case 0x1U:                  /* Unused */

   break;

  case 0x2U:                  /* Double scan split */

   stat[RRPGE_STA_UPA_G + adr] = val & 0x00FFU;
   break;

  case 0x3U:                  /* Display list clear controls */

   stat[RRPGE_STA_UPA_G + adr] = val & 0xFFFFU;
   break;

  case 0x4U:
  case 0x5U:                  /* Shift mode region */

   stat[RRPGE_STA_UPA_G + adr] = val & 0xFFFFU;
   break;

  case 0x6U:                  /* Display list definition */

   stat[RRPGE_STA_UPA_G + adr] = val & 0xFFFFU;
   stat[RRPGE_STA_UPA_G +  7U] = 0x8000U; /* Frame not complete */
   stat[RRPGE_STA_UPA_GF + 1U] |= 2U;     /* Graphics FIFO suspended */
   break;

  case 0x7U:                  /* Status flags */

   break;

  default:                    /* Source definitions */

   stat[RRPGE_STA_UPA_G + adr] = val & 0xFFFFU;
   break;

 }
}
