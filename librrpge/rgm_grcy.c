/**
**  \file
**  \brief     Graphics timing calculator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.26
*/


#include "rgm_grcy.h"


/* Video memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  VRAMS  RRPGE_M_VRAMS


/* Cell cycle multiplier table */
static uint8 const rrpge_m_graccy_tc[32] = {
 6, 4, 4, 0, 8, 8, 4, 0,
 4, 2, 0, 0, 8, 8, 0, 0,
 6, 4, 4, 0, 6, 4, 4, 0,
 4, 2, 0, 0, 4, 4, 0, 0};


/* Calculates cycles taken by an accelerator function according to the current
** accelerator register contents. Call it before actually executing the
** operation. */
auint rrpge_m_graccy(void)
{
 auint ac;           /* Accelerator cycles summed */
 auint nc;           /* Cell count extracted from parameters */
 auint np;           /* Pixel count extracted from parameters */
 auint flg;          /* Mode flags */
 auint t;

 /* Retrieve mode flags affecting performance, and add video mode */

 flg = rrpge_m_edat->stat.ropd[0xEEDU];

 /* Retrieve for cell and pixel counts from the accelerator registers */

 np = ((rrpge_m_edat->stat.ropd[0xEEEU] - 1U) & 0x3FFU) + 1U;
 nc = rrpge_m_edat->stat.ropd[0xEFDU] >> 13; /* 4 bit pixels right shift count */

 /* Determine if blit may be accelerated (no destination read), and finish the
 ** flags: bit 0-1: mode, bit 2: reindex, bit 3: accel, bit 4: display mode */

 t = flg;
 flg  = (flg >> 10) & 0x7U;
 if ( ((t & 0x0400U) == 0x0000U) && /* BB or FL mode */
      ((t & 0x3000U) != 0x3000U) && /* Not reindex by destination (VDR) */
      ((t & 0x0200U) == 0x0000U) && /* No colorkey */
      (rrpge_m_edat->stat.ropd[0xEE0U] == 0xFFFFU) && /* No bits masked for write */
      (rrpge_m_edat->stat.ropd[0xEE1U] == 0xFFFFU) && /* No bits masked for write */
      ((np & 0x0007U) == 0U) &&     /* Low 3 bits of count are zero */
      ( nc            == 0U) ){     /* High 3 bits of dest. fraction are zero */
  flg |= 0x8U;                      /* Accelerated blit */
 }
 flg |= rrpge_m_info.vbm & 0x10U;   /* Display mode (nonzero: 8 bit, zero: 4 bit) */

 /* Calculate cell & pixel counts */

 if ((flg & 0x10U) == 0U){ /* 4 bit mode */
  nc = ((nc + np) + 7U) >> 3;
 }else{                    /* 8 bit mode */
  np = np >> 1;
  nc = ((nc >> 1) + np + 3U) >> 2;
 }

 /* First: Overhead cycles */

 if ((flg & 0x4U) == 0U){ ac = 20U; } /* No reindex */
 else                   { ac = 28U; } /* Has reindex */

 /* Cell multipliers */

 ac += nc * (auint)(rrpge_m_graccy_tc[flg]);

 /* Pixel multipliers */

 if ((flg & 0x2U) != 0U){  /* SC or LI mode */
  if ((flg & 0x1U) == 0U){
   ac += np * 2U;          /* SC mode */
  }else{
   ac += np * 4U;          /* LI mode */
  }
 }

 /* Produce return value (add to the cycle count, if any is already there,
 ** for example from the Graphics FIFO). */

 return ac;
}
