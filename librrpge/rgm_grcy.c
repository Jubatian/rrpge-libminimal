/**
**  \file
**  \brief     Graphics timing calculator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#include "rgm_grcy.h"


/* Video memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  VRAMS  RRPGE_M_VRAMS



/* Calculates stall mode (number of layers enabled) for a given line by the
** current display list. Returns the number of layers displayed on the given
** line. */
auint rrpge_m_grlsm(auint lin)
{
 auint r;

 if (lin >= 400U){       /* In VBlank: No video display stalls */
  r = 0U;
 }else{                  /* In display area: depends on layers enabled */
  r = ((auint)(rrpge_m_edat->stat.ropd[0xEE3U]) << 9) & (VRAMS - 1U);
  r = rrpge_m_edat->stat.vram[r + lin]; /* Background display list entry */
  r = ((r >> 16) & 0x3U) + 1U;          /* Number of enabled layers from layer combination */
 }

 return r;
}



/* Calculates cycles taken by an accelerator function according to the current
** display list contents. Stores the result in rrpge_m_info.vac. */
void rrpge_m_graccy(void)
{
 auint rc;           /* Current cycle within line (0 - 399) */
 auint rl;           /* Current video line (0 - RRPGE_M_VLN-1) */
 auint ac;           /* Accelerator cycles summed */
 auint co;           /* Number of overhead cycles */
 auint cc2;          /* Number of cycles per cell, 0-2 layers */
 auint cp2;          /* Number of cycles per pixel, 0-2 layers */
 auint cc3;          /* Number of cycles per cell, 3 layers */
 auint cp3;          /* Number of cycles per pixel, 3 layers */
 auint nc;           /* Cell count extracted from parameters */
 auint np;           /* Pixel count extracted from parameters */
 auint flg;          /* Mode flags */
 auint i;
 auint t;
 auint m;

 /* Retrieve mode flags affecting performance, and add video mode */

 flg   = rrpge_m_edat->stat.ropd[0xEF8U] & 0x1C00U;
 flg  |= (rrpge_m_info.vbm & 0x80U) << 6;
 flg >>= 10;         /* bit 0-1: mode (0:BB 1:LI 2:SC 3:LI), bit 2: reindex, bit3: 8bit if set */

 /* Retrieve cell and pixel counts from the accelerator registers */

 np = ((rrpge_m_edat->stat.ropd[0xEF9U] - 1U) & 0x3FFU) + 1U;
 nc = rrpge_m_edat->stat.ropd[0xEF9U] >> 13; /* 4 bit pixels right shift count */
 if ((flg & 0x8U) == 0){ /* 4 bit mode */
  nc = ((nc + np) + 7U) >> 3;
 }else{                  /* 8 bit mode */
  np = np >> 1;
  nc = ((nc >> 1) + np + 3U) >> 2;
 }

 /* Prepare cycle counts according to the mode */

 if ((flg & 0x4U) == 0U){ co = 20U; } /* No reindex */
 else                   { co = 28U; } /* Has reindex */
 if ((flg & 0x3U) == 2U){ cp2 = 2U; cp3 = 4U; } /* SC mode */
 else                   { cp2 = 0U; cp3 = 0U; } /* BB or LI mode */

 switch (flg){

  case 0x0U:         /* 4 bit, no reindex, BB */
  case 0x8U:         /* 8 bit, no reindex, BB */
  case 0xCU:         /* 8 bit, reindex, BB */
   cc2 = 6U;  cc3 = 12U;
   break;

  case 0x4U:         /* 4 bit, reindex, BB */
   cc2 = 8U;  cc3 = 12U;
   break;

  case 0x5U:         /* 4 bit, reindex, LI */
  case 0x7U:         /* 4 bit, reindex, LI */
   cc2 = 8U;  cc3 = 8U;
   break;

  default:           /* 0x1U: 4 bit, no reindex, LI */
                     /* 0x2U: 4 bit, no reindex, SC */
                     /* 0x3U: 4 bit, no reindex, LI */
                     /* 0x6U: 4 bit, reindex, SC */
                     /* 0x9U: 8 bit, no reindex, LI */
                     /* 0xAU: 8 bit, no reindex, SC */
                     /* 0xBU: 8 bit, no reindex, LI */
                     /* 0xDU: 8 bit, reindex, LI */
                     /* 0xEU: 8 bit, reindex, SC */
                     /* 0xFU: 8 bit, reindex, LI */
   cc2 = 4U;  cc3 = 8U;
   break;

 }

 /* Assume that the accelerator operation immediately starts accessing memory
 ** in it's usual access pattern, and only add the overhead cycles (co) to the
 ** end result. Normally these cycles are distributed along the begin and end
 ** of the accelerator operation as the pipeline fills up and depletes, but it
 ** does not produce much inaccuracy in overall performance if these cycles
 ** are just padded at the end. */

 /* Set up, determine initial display mode */

 rl = rrpge_m_info.vln;
 rc = rrpge_m_info.vlc;
 ac = 0U;
 if (rl >= 400U){        /* In VBlank: No video display stalls */
  m = 0U;
 }else if (rc < 80U){    /* In HBlank: Display list stalls only */
  m = 1U;
 }else{                  /* In display area: depends on planes enabled */
  m = rrpge_m_grlsm(rl);
  if (m == 4U){          /* All planes enabled: complete block */
   ac += 400U - rc;      /* Advance to next line */
   rl ++;
   rc = 0U;
   m  = 1U;              /* No need to check VBlank, from the point of the acc. mode 0 and 1 is the same */
  }
 }

 /* Walk through the accelerator operation cell by cell, pixel by pixel, and
 ** count the total cycles taken as the video mode permits. Note: empty render
 ** is possible in 8 bits mode, this case only overhead pixels are added. */

 while (nc != 0U){

  t = rc;

  /* Consume one cell */

  if (m == 3U){ i = cc3; }
  else        { i = cc2; }
  ac += i;
  rc += i;
  nc --;

  /* Consume pixels relating the cell */

  if (cp2 != 0U){             /* Pixel cycles are specified (SC mode) */
   if ((flg & 0x8U) == 0U){   /* 4 bit mode: 8 pixels consumed in a cell */
    if (np >= 8U){ i = 8U; }
    else         { i = np; }
   }else{                     /* 8 bit mode: 4 pixels consumed in a cell */
    if (np >= 4U){ i = 4U; }
    else         { i = np; }
   }
   np -= i;
   if (m == 3U){ i *= cp3; }
   else        { i *= cp2; }
   ac += i;
   rc += i;
  }

  /* Check if pixel counter reached a new boundary by display, if so, update
  ** display mode information (and consume a line when blocking). Note that at
  ** worst 'rc' may increment with 8 + 8 * 4, less than 80. */

  if (rc >= 400U){       /* Wrap line, will be in HBlank period */
   rc -= 400U;
   rl ++;
   if (rl >= RRPGE_M_VLN){ rl = 0; }
   m   = 1U;             /* Just skip checking VBlank, mode 0 or 1 does not matter for the accelerator */
  }else if (rl >= 400U){
   m   = 0U;             /* In VBlank no accesses are performed by display */
  }else{                 /* In display area */
   if ((t < 80U) && (rc >= 80U)){ /* Passed HBlank period */
    m = rrpge_m_grlsm(rl);
    if (m == 4U){        /* All planes enabled: complete block */
     ac += 400U - rc;    /* Advance to next line */
     rl ++;
     rc = 0U;
     m  = 1U;            /* Again no need to check VBlank */
    }
   }
  }

 }

 /* Add overhead cycles and return */

 rrpge_m_info.vac = ac + co;

}
