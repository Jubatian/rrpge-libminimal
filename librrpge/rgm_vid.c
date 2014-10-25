/**
**  \file
**  \brief     Graphics FIFO and display manager.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
*/



#include "rgm_vid.h"
#include "rgm_grln.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* Based on the cycles needing emulation, process the video, and the Graphics
** Display Generator's Display List clear function. Also calls back the line
** renderer as needed. */
void  rrpge_m_vidproc(auint cy)
{
 auint a;
 auint i;
 auint j;
 auint c;
 auint s;
 auint t;

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
   rrpge_m_info.hlt |= RRPGE_HLT_FRAME;
  }

  /* Before starting next frame (line counter is zero, so next iteration will
  ** result in rendering a line), if the flags request so, process the display
  ** list clear, and update the latch, so completing the double buffering
  ** assistance. */

  if ( (rrpge_m_info.vln == 0U) &&
       ((rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x7U] & 0x8000U) != 0U) ){

   /* Prepare for clear */

   i  = 9600U;
   a  = rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x15U] & ((PRAMS - 1U) >> 9);
   a  = a & (~(((auint)(4U) << (a & 3U)) - 4U)); /* Clear low bits of address part */
   j  = 1600U << (a & 3U);                       /* Clear limit, also guarantees no overrun */
   a  = (a & (~(auint)(3U))) << 9;
   s  = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x6U] >> 11; /* Initial skip */
   j -= s;
   a += s;
   s  = (rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x6U] >> 6) & 0x1FU; /* Skip amount / streak */
   c  = (rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x6U]     ) & 0x3FU; /* Clear amount / streak */

   /* Clear */

   if (c != 0U){
    do{
     for (t = 0U; t < c; t++){ /* Clear */
      rrpge_m_edat->st.pram[a] = 0U;
      a ++;
      i --;
      j --;
      if ((i == 0U) || (j == 0U)){ break; }
     }
     if (j <= s){              /* Skip */
      j = 0U;
     }else{
      j -= s;
      a += s;
     }
    }while ((i != 0U) && (j != 0U));
   }

   /* Update latch, and clear flags (both flags together) */

   rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x7U] &= ((PRAMS - 1U) >> 9); /* Clear flags */
   rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x15U]  = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x7U];

  }

 }

}



/* Operates the memory mapped interface of the Graphics Display Generator.
** Only the low 4 bits of the address are used. Only low 16 bits of value are
** used. */
void  rrpge_m_vidwrite(auint adr, auint val)
{
 adr = adr & 0xFU;

 switch (adr & 0xCU){

  case 0x0U:                  /* Mask / Colorkey definitions */

   rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + adr] = (uint16)(val);
   break;

  case 0x4U:                  /* Shift mode region & Controls & Flags */

   if ((adr & 0x2U) == 0U){   /* Shift mode region */
    rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + adr] = (uint16)(val & 0x7F7FU);
   }else{                     /* Controls & Flags */
    if (adr == 0x6U){         /* Display list clear */
     rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x6U] = (uint16)(val);
    }else{                    /* Display list definition & process flags (Flags become set) */
     rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x7U] = (val & ((PRAMS - 1U) >> 9)) | 0xC000U;
    }
   }
   break;

  default:                    /* Source definitions */

   rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + adr] = (uint16)(val);
   break;

 }
}
