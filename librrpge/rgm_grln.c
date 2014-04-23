/**
**  \file
**  \brief     Graphics line renderer
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#include "rgm_grln.h"


/* Video memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  VRAMS  RRPGE_M_VRAMS


/* Local buffer for rendering lines */
static uint32 rrpge_m_grln_buf[80U];



/* Calculates display list pointers into frep[] of rrpge_m_edat. Increments
** the line counter afterwards, also working through VBlank into the next
** frame. Transfers render request (rena) flag on frame end and sets frame
** end halt cause. Sets the frld flag of rrpge_m_edat in display lines,
** clears it in VBlank lines. */
void rrpge_m_grpr(void)
{
 auint*  pptr;                 /* Pointer to the layer pointers to use */
 auint   ln;                   /* Line to render */
 auint   i;
 auint   p;
 auint   dpart;                /* Partitioning setting */
 auint   bgent;                /* Background display list entry */
 auint   vpart;                /* Video RAM partition mask */

 ln   = rrpge_m_edat->frln;
 rrpge_m_edat->frln ++;        /* Advance line for next run */

 if (ln >= 400U){              /* VBlank lines */
  if (ln >= (RRPGE_M_VLN - 1)){
   rrpge_m_edat->frln = 0U;    /* Reached frame end, wrap around */
   rrpge_m_edat->rena = (rrpge_m_edat->rena & (~0x2U)) | /* Transfer requested render state */
                        ((rrpge_m_edat->rena & (0x1U)) << 1);
   rrpge_m_info.hlt |= RRPGE_HLT_FRAME;
  }
  rrpge_m_edat->frld = 0U;     /* No line rendering necessary */
  return;
 }

 rrpge_m_edat->frld = 1U;      /* Previous line was not yet rendered */
 pptr = &rrpge_m_edat->frep[ln * 4U];

 /* If in line 0, reset offsets used for relative pointers */

 if (ln == 0){
  pptr[0] = 0;
  pptr[1] = 0;
  pptr[2] = 0;
  pptr[3] = 0;
 }

 /* Read background display list to get the appropriate partition masks */

 p = ((auint)(rrpge_m_edat->stat.ropd[0xEE3U]) << 9) & (VRAMS - 1U);
 bgent = rrpge_m_edat->stat.vram[p + ln]; /* Background layer display list entry */

 /* Read layer display lists & set up pointers */

 dpart = rrpge_m_edat->stat.ropd[0xEE2U];

 for (i = 0; i < 4; i++){

  p = ((auint)(rrpge_m_edat->stat.ropd[0xEE4U + i]) << 9) & (VRAMS - 1U);
  p = rrpge_m_edat->stat.vram[p + ln]; /* Layer display list entry */
  if ((p & 0x100U) == 0U){             /* Relative pointer */
   vpart = (dpart >> (((bgent >> (30U - (i << 1))) & 3U) << 2) ) & 0x7U;
   vpart = (512U << vpart) - 1U;
   vpart = (vpart << 16) | 0xFFFFU;
   p = (p & (~vpart)) | ((p + pptr[i + 0U]) & vpart);
  }
  pptr[i + 4U] = p & 0xFFFFFE00U;

 }

}



/* Renders current graphics line. This must be called after an rrpge_m_grpr()
** call. Clears the frld flag of rrpge_m_edat. */
void rrpge_m_grln(void)
{
 uint32* lptr;                 /* Pointer to the line to render */
 auint*  pptr;                 /* Pointer to the layer pointers to use */
 auint   ln;                   /* Line to render */
 uint32  ckm7;                 /* Colorkey parameters */
 uint32  ckm8;                 /* (Depending on graphics mode: 4/8 bits) */
 auint   cksh;
 uint32  bg;                   /* Background pattern */
 uint32  s0,   s1,   s2,   s3;    /* Shifted sources */
 uint32  c0,   c1,   c2,   c3;    /* Read sources */
 auint   pb0,  pb1,  pb2,  pb3;   /* Source pointer high parts */
 auint   pp0,  pp1,  pp2,  pp3;   /* Source pointer low parts */
 uint32  m0,   m1,   m2,   m3;    /* Write masks */
 uint32  ck0,  ck1,  ck2,  ck3;   /* Mask or colorkey, depending on layer's mode */
 auint   p0sl, p1sl, p2sl, p3sl;  /* Source left shift */
 auint   p0sr, p1sr, p2sr, p3sr;  /* Source right shift (halved) */
 uint32  vpart[4];             /* Video RAM partition mask */
 auint   i;
 auint   t;
 auint   d;
 auint   lmod;                 /* Display mode of the line */
 auint   gmsk;                 /* Global mask of the line */


 ln   = (rrpge_m_edat->frln - 1U); /* rrpge_m_grpr(); already incremented it */
 rrpge_m_edat->frld = 0U;      /* Line will be rendered */

 if (ln >= 400U) return;       /* Nothing to do in VBlank */

 lptr = &rrpge_m_grln_buf[0];
 pptr = &rrpge_m_edat->frep[(ln * 4U) + 4U];

 /* Read Video RAM partitioning setting for each layer */

 i = ((auint)(rrpge_m_edat->stat.ropd[0xEE3U]) << 9) & (VRAMS - 1U);
 bg = rrpge_m_edat->stat.vram[i + ln]; /* Background display list entry */

 d = rrpge_m_edat->stat.ropd[0xEE2U];
 for (i = 0; i < 4U; i++){
  t = d >> (((bg >> (30U - (i << 1))) & 3U) << 2);
  if ((t & 0x8U) != 0U){ t = 0x7FU; }
  else                 { t = (512U << (t & 0x7U)) - 1U; }
  vpart[i] = t;
 }

 /* Read display list data & set up line parameters */

 lmod = (bg >> 16) & 0x1FU;    /* Layer display configuration & mask/key setting */
 gmsk = (bg >> 21) & 0x07U;    /* Global mask */
 gmsk = (1U << (8U - gmsk)) - 1U;
 pb0 = ((bg >> 30) & 0x3U) << 16;
 pb1 = ((bg >> 28) & 0x3U) << 16;
 pb2 = ((bg >> 26) & 0x3U) << 16;
 pb3 = ((bg >> 24) & 0x3U) << 16;
 bg = (bg & 0xFFFFU);
 bg |= bg << 16;               /* Background color pattern */

 /* Read layer display lists & set up layer parameters. Only the colorkey or
 ** mask data has to be read at this point (pointers were done in
 ** rrpge_m_grpr()). */

 i = ((auint)(rrpge_m_edat->stat.ropd[0xEE4U]) << 9) & (VRAMS - 1U);
 ck0 = rrpge_m_edat->stat.vram[i + ln];
 i = ((auint)(rrpge_m_edat->stat.ropd[0xEE5U]) << 9) & (VRAMS - 1U);
 ck1 = rrpge_m_edat->stat.vram[i + ln];
 i = ((auint)(rrpge_m_edat->stat.ropd[0xEE6U]) << 9) & (VRAMS - 1U);
 ck2 = rrpge_m_edat->stat.vram[i + ln];
 i = ((auint)(rrpge_m_edat->stat.ropd[0xEE7U]) << 9) & (VRAMS - 1U);
 ck3 = rrpge_m_edat->stat.vram[i + ln];

 /* Fix layer masks depending on their priority order so all masks (ckx) will
 ** only have the bits set which are to be displayed, or colorkeys will be
 ** limited to the bits actually reserved for the colorkeyed layers. The
 ** global mask will hold the mask for colorkeyed layers. */
 switch (lmod & 0x18U){
  case 0x00U:                  /* L0-CK; L1-MS; L2-MS; L3-MS */
   i    = gmsk & (0xFFU ^ (ck3 | ck2 | ck1));
   ck0 &= i;
   ck1 &= gmsk & (0xFFU ^ (ck3 | ck2));
   ck2 &= gmsk & (0xFFU ^ (ck3));
   ck3 &= gmsk;
   gmsk = i;
   break;
  case 0x08U:                  /* L0-CK; L1-CK; L2-MS; L3-MS */
   i    = gmsk & (0xFFU ^ (ck3 | ck2));
   ck0 &= i;
   ck1 &= i;
   ck2 &= gmsk & (0xFFU ^ (ck3));
   ck3 &= gmsk;
   gmsk = i;
   break;
  case 0x10U:                  /* L0-CK; L1-CK; L2-CK; L3-MS */
   i    = gmsk & (0xFFU ^ (ck3));
   ck0 &= i;
   ck1 &= i;
   ck2 &= i;
   ck3 &= gmsk;
   gmsk = i;
   break;
  default:                     /* L0-CK; L1-CK; L2-CK; L3-CK */
   ck0 &= gmsk;
   ck1 &= gmsk;
   ck2 &= gmsk;
   ck3 &= gmsk;
   break;
 }

 /* Build layer pointers */
 pb0 += pptr[0] >> 16;
 pb1 += pptr[1] >> 16;
 pb2 += pptr[2] >> 16;
 pb3 += pptr[3] >> 16;
 pp0  = pb0 & vpart[0];
 pb0 &= ~vpart[0];
 pp1  = pb1 & vpart[1];
 pb1 &= ~vpart[1];
 pp2  = pb2 & vpart[2];
 pb2 &= ~vpart[2];
 pp3  = pb3 & vpart[3];
 pb3 &= ~vpart[3];

 /* Calculate layer shifts, first part (mode indifferent) */
 p0sl = pptr[0] >> 11;
 p1sl = pptr[1] >> 11;
 p2sl = pptr[2] >> 11;
 p3sl = pptr[3] >> 11;

 /* Mode specific setups */

 if ((rrpge_m_info.vbm & 0x80U) == 0U){ /* 4bit mode */

  /* Colorkey calculation values */
  ckm7 = 0x77777777U;
  ckm8 = 0x88888888U;
  cksh = 3U;

  /* Expand global mask for colorkeyed layers */
  gmsk &= 0xFU;
  gmsk |= gmsk << 4;

  /* Expand colorkey / mask from display list */
  ck0 &= 0xFU;
  ck0 |= ck0 << 4;
  ck1 &= 0xFU;
  ck1 |= ck1 << 4;
  ck2 &= 0xFU;
  ck2 |= ck2 << 4;
  ck3 &= 0xFU;
  ck3 |= ck3 << 4;

  /* Mask layer shifts */
  p0sl &= 0x1CU;
  p1sl &= 0x1CU;
  p2sl &= 0x1CU;
  p3sl &= 0x1CU;

 }else{                        /* 8bit mode */

  /* Colorkey calculation values */
  ckm7 = 0x7F7F7F7FU;
  ckm8 = 0x80808080U;
  cksh = 7U;

  /* Mask layer shifts */
  p0sl &= 0x18U;
  p1sl &= 0x18U;
  p2sl &= 0x18U;
  p3sl &= 0x18U;

 }

 /* Finish expanding global mask for colorkeyed layers */
 gmsk |= gmsk << 8;
 gmsk |= gmsk << 16;

 /* Finish expanding colorkey / mask */
 ck0 |= ck0 << 8;
 ck0 |= ck0 << 16;
 ck1 |= ck1 << 8;
 ck1 |= ck1 << 16;
 ck2 |= ck2 << 8;
 ck2 |= ck2 << 16;
 ck3 |= ck3 << 8;
 ck3 |= ck3 << 16;

 /* Calculate layer right shifts (halved to avoid shifts with 32) */
 p0sr = (32U - p0sl) >> 1;
 p1sr = (32U - p1sl) >> 1;
 p2sr = (32U - p2sl) >> 1;
 p3sr = (32U - p3sl) >> 1;

 /* Colorkey mask calculation (for 4bit):
 ** mx = sx ^ ckx;
 ** In mx there will be zeros at pixel locations matching the colorkey.
 ** mx = (((mx & 0x77777777U) + 0x77777777U) | mx) & 0x88888888U;
 ** This gives 0x8 at every pixel not matching the key (nonzero), and 0 at
 ** pixels matching the key. The main trick is carry over to high bit of color
 ** performed by the addition, the "| mx" part fixing for pixel 0x8U.
 ** mx = (mx - (mx >> 3)) + mx;
 ** This extends the 0x8U's to 0xFU's while preserving 0's, creating a mask
 ** which can be used to mask out pixels matching the colorkey. */

 switch (lmod){

  case 0x00U:
  case 0x08U:  /* Mode 0x00; 0x08;            ->               L2-MS        */

   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   for (i = 0; i < 80U; i++){
    pp2 = (pp2 + 1U) & vpart[2];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    s2 |= (c2 >> p2sr) >> p2sr;
    lptr[i] = (bg & (~ck2)) | (s2 & ck2);
    s2 = c2 << p2sl;
   }
   break;

  case 0x01U:
  case 0x05U:
  case 0x09U:
  case 0x0DU:  /* Mode 0x01; 0x05; 0x09; 0x0D -> L0-CK;        L2-MS        */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp2 = (pp2 + 1U) & vpart[2];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    s0 |= (c0 >> p0sr) >> p0sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    m0 = (s0 & gmsk) ^ ck0;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    lptr[i] = (( (bg & (~m0)) |
               (s0 & m0)) & (~ck2)) |
              (s2 & ck2);
    s0 = c0 << p0sl;
    s2 = c2 << p2sl;
   }
   break;

  case 0x02U:
  case 0x0AU:  /* Mode 0x02; 0x0A             -> L0-CK;        L2-MS; L3-MS */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   s3 = rrpge_m_edat->stat.vram[pb3 + pp3] << p3sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp2 = (pp2 + 1U) & vpart[2];
    pp3 = (pp3 + 1U) & vpart[3];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    c3 = rrpge_m_edat->stat.vram[pb3 + pp3];
    s0 |= (c0 >> p0sr) >> p0sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    s3 |= (c3 >> p3sr) >> p3sr;
    m0 = (s0 & gmsk) ^ ck0;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    lptr[i] = (( (( (bg & (~m0)) |
                  (s0 & m0)) & (~ck2)) |
               (s2 & ck2)) & (~ck3)) |
              (s3 & ck3);
    s0 = c0 << p0sl;
    s2 = c2 << p2sl;
    s3 = c3 << p3sl;
   }
   break;

  case 0x03U:
  case 0x07U:  /* Mode 0x03; 0x07             -> L0-CK; L1-MS; L2-MS; L3-MS */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s1 = rrpge_m_edat->stat.vram[pb1 + pp1] << p1sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   s3 = rrpge_m_edat->stat.vram[pb3 + pp3] << p3sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp1 = (pp1 + 1U) & vpart[1];
    pp2 = (pp2 + 1U) & vpart[2];
    pp3 = (pp3 + 1U) & vpart[3];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c1 = rrpge_m_edat->stat.vram[pb1 + pp1];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    c3 = rrpge_m_edat->stat.vram[pb3 + pp3];
    s0 |= (c0 >> p0sr) >> p0sr;
    s1 |= (c1 >> p1sr) >> p1sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    s3 |= (c3 >> p3sr) >> p3sr;
    m0 = (s0 & gmsk) ^ ck0;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    lptr[i] = (( (( (( (bg & (~m0)) |
                     (s0 & m0)) & (~ck1)) |
                  (s1 & ck1)) & (~ck2)) |
               (s2 & ck2)) & (~ck3)) |
              (s3 & ck3);
    s0 = c0 << p0sl;
    s1 = c1 << p1sl;
    s2 = c2 << p2sl;
    s3 = c3 << p3sl;
   }
   break;

  case 0x04U:
  case 0x0CU:
  case 0x14U:
  case 0x1CU:  /* Mode 0x04; 0x0C; 0x14; 0x1C -> L0-CK                      */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    s0 |= (c0 >> p0sr) >> p0sr;
    m0 = (s0 & gmsk) ^ ck0;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    lptr[i] = (bg & (~m0)) | (s0 & m0);
    s0 = c0 << p0sl;
   }
   break;

  case 0x06U:  /* Mode 0x06                   -> L0-CK; L1-MS; L2-MS        */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s1 = rrpge_m_edat->stat.vram[pb1 + pp1] << p1sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp1 = (pp1 + 1U) & vpart[1];
    pp2 = (pp2 + 1U) & vpart[2];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c1 = rrpge_m_edat->stat.vram[pb1 + pp1];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    s0 |= (c0 >> p0sr) >> p0sr;
    s1 |= (c1 >> p1sr) >> p1sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    m0 = (s0 & gmsk) ^ ck0;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    lptr[i] = (( (( (bg & (~m0)) |
                  (s0 & m0)) & (~ck1)) |
               (s1 & ck1)) & (~ck2)) |
              (s2 & ck2);
    s0 = c0 << p0sl;
    s1 = c1 << p1sl;
    s2 = c2 << p2sl;
   }
   break;

  case 0x0BU:
  case 0x0FU:  /* Mode 0x0B; 0x0F             -> L0-CK; L1-CK; L2-MS; L3-MS */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s1 = rrpge_m_edat->stat.vram[pb1 + pp1] << p1sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   s3 = rrpge_m_edat->stat.vram[pb3 + pp3] << p3sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp1 = (pp1 + 1U) & vpart[1];
    pp2 = (pp2 + 1U) & vpart[2];
    pp3 = (pp3 + 1U) & vpart[3];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c1 = rrpge_m_edat->stat.vram[pb1 + pp1];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    c3 = rrpge_m_edat->stat.vram[pb3 + pp3];
    s0 |= (c0 >> p0sr) >> p0sr;
    s1 |= (c1 >> p1sr) >> p1sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    s3 |= (c3 >> p3sr) >> p3sr;
    m0 = (s0 & gmsk) ^ ck0;
    m1 = (s1 & gmsk) ^ ck1;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m1 = (((m1 & ckm7) + ckm7) | m1) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    m1 = (m1 - (m1 >> cksh)) + m1;
    lptr[i] = (( (( (( (bg & (~m0)) |
                     (s0 & m0)) & (~m1)) |
                  (s1 & m1)) & (~ck2)) |
               (s2 & ck2)) & (~ck3)) |
              (s3 & ck3);
    s0 = c0 << p0sl;
    s1 = c1 << p1sl;
    s2 = c2 << p2sl;
    s3 = c3 << p3sl;
   }
   break;

  case 0x0EU:  /* Mode 0x0E                   -> L0-CK; L1-CK; L2-MS        */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s1 = rrpge_m_edat->stat.vram[pb1 + pp1] << p1sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp1 = (pp1 + 1U) & vpart[1];
    pp2 = (pp2 + 1U) & vpart[2];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c1 = rrpge_m_edat->stat.vram[pb1 + pp1];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    s0 |= (c0 >> p0sr) >> p0sr;
    s1 |= (c1 >> p1sr) >> p1sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    m0 = (s0 & gmsk) ^ ck0;
    m1 = (s1 & gmsk) ^ ck1;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m1 = (((m1 & ckm7) + ckm7) | m1) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    m1 = (m1 - (m1 >> cksh)) + m1;
    lptr[i] = (( (( (bg & (~m0)) |
                  (s0 & m0)) & (~m1)) |
               (s1 & m1)) & (~ck2)) |
              (s2 & ck2);
    s0 = c0 << p0sl;
    s1 = c1 << p1sl;
    s2 = c2 << p2sl;
   }
   break;

  case 0x10U:
  case 0x18U:  /* Mode 0x10; 0x18             ->               L2-CK        */

   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   for (i = 0; i < 80U; i++){
    pp2 = (pp2 + 1U) & vpart[2];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    s2 |= (c2 >> p2sr) >> p2sr;
    m2 = (s2 & gmsk) ^ ck2;
    m2 = (((m2 & ckm7) + ckm7) | m2) & ckm8;
    m2 = (m2 - (m2 >> cksh)) + m2;
    lptr[i] = (bg & (~m2)) | (s2 & m2);
    s2 = c2 << p2sl;
   }
   break;

  case 0x11U:
  case 0x15U:
  case 0x19U:
  case 0x1DU:  /* Mode 0x11; 0x15; 0x19; 0x1D -> L0-CK;        L2-CK        */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp2 = (pp2 + 1U) & vpart[2];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    s0 |= (c0 >> p0sr) >> p0sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    m0 = (s0 & gmsk) ^ ck0;
    m2 = (s2 & gmsk) ^ ck2;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m2 = (((m2 & ckm7) + ckm7) | m2) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    m2 = (m2 - (m2 >> cksh)) + m2;
    lptr[i] = (( (bg & (~m0)) |
               (s0 & m0)) & (~m2)) |
              (s2 & m2);
    s0 = c0 << p0sl;
    s2 = c2 << p2sl;
   }
   break;

  case 0x12U:  /* Mode 0x12                   -> L0-CK;        L2-CK; L3-MS */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   s3 = rrpge_m_edat->stat.vram[pb3 + pp3] << p3sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp2 = (pp2 + 1U) & vpart[2];
    pp3 = (pp3 + 1U) & vpart[3];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    c3 = rrpge_m_edat->stat.vram[pb3 + pp3];
    s0 |= (c0 >> p0sr) >> p0sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    s3 |= (c3 >> p3sr) >> p3sr;
    m0 = (s0 & gmsk) ^ ck0;
    m2 = (s2 & gmsk) ^ ck2;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m2 = (((m2 & ckm7) + ckm7) | m2) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    m2 = (m2 - (m2 >> cksh)) + m2;
    lptr[i] = (( (( (bg & (~m0)) |
                  (s0 & m0)) & (~m2)) |
               (s2 & m2)) & (~ck3)) |
              (s3 & ck3);
    s0 = c0 << p0sl;
    s2 = c2 << p2sl;
    s3 = c3 << p3sl;
   }
   break;

  case 0x13U:
  case 0x17U:  /* Mode 0x13; 0x17             -> L0-CK; L1-CK; L2-CK; L3-MS */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s1 = rrpge_m_edat->stat.vram[pb1 + pp1] << p1sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   s3 = rrpge_m_edat->stat.vram[pb3 + pp3] << p3sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp1 = (pp1 + 1U) & vpart[1];
    pp2 = (pp2 + 1U) & vpart[2];
    pp3 = (pp3 + 1U) & vpart[3];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c1 = rrpge_m_edat->stat.vram[pb1 + pp1];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    c3 = rrpge_m_edat->stat.vram[pb3 + pp3];
    s0 |= (c0 >> p0sr) >> p0sr;
    s1 |= (c1 >> p1sr) >> p1sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    s3 |= (c3 >> p3sr) >> p3sr;
    m0 = (s0 & gmsk) ^ ck0;
    m1 = (s1 & gmsk) ^ ck1;
    m2 = (s2 & gmsk) ^ ck2;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m1 = (((m1 & ckm7) + ckm7) | m1) & ckm8;
    m2 = (((m2 & ckm7) + ckm7) | m2) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    m1 = (m1 - (m1 >> cksh)) + m1;
    m2 = (m2 - (m2 >> cksh)) + m2;
    lptr[i] = (( (( (( (bg & (~m0)) |
                     (s0 & m0)) & (~m1)) |
                  (s1 & m1)) & (~m2)) |
               (s2 & m2)) & (~ck3)) |
              (s3 & ck3);
    s0 = c0 << p0sl;
    s1 = c1 << p1sl;
    s2 = c2 << p2sl;
    s3 = c3 << p3sl;
   }
   break;

  case 0x16U:
  case 0x1EU:  /* Mode 0x16; 0x1E             -> L0-CK; L1-CK; L2-CK        */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s1 = rrpge_m_edat->stat.vram[pb1 + pp1] << p1sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp1 = (pp1 + 1U) & vpart[1];
    pp2 = (pp2 + 1U) & vpart[2];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c1 = rrpge_m_edat->stat.vram[pb1 + pp1];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    s0 |= (c0 >> p0sr) >> p0sr;
    s1 |= (c1 >> p1sr) >> p1sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    m0 = (s0 & gmsk) ^ ck0;
    m1 = (s1 & gmsk) ^ ck1;
    m2 = (s2 & gmsk) ^ ck2;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m1 = (((m1 & ckm7) + ckm7) | m1) & ckm8;
    m2 = (((m2 & ckm7) + ckm7) | m2) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    m1 = (m1 - (m1 >> cksh)) + m1;
    m2 = (m2 - (m2 >> cksh)) + m2;
    lptr[i] = (( (( (bg & (~m0)) |
                  (s0 & m0)) & (~m1)) |
               (s1 & m1)) & (~m2)) |
              (s2 & m2);
    s0 = c0 << p0sl;
    s1 = c1 << p1sl;
    s2 = c2 << p2sl;
   }
   break;

  case 0x1AU:  /* Mode 0x1A                   -> L0-CK;        L2-CK; L3-CK */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   s3 = rrpge_m_edat->stat.vram[pb3 + pp3] << p3sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp2 = (pp2 + 1U) & vpart[2];
    pp3 = (pp3 + 1U) & vpart[3];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    c3 = rrpge_m_edat->stat.vram[pb3 + pp3];
    s0 |= (c0 >> p0sr) >> p0sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    s3 |= (c3 >> p3sr) >> p3sr;
    m0 = (s0 & gmsk) ^ ck0;
    m2 = (s2 & gmsk) ^ ck2;
    m3 = (s3 & gmsk) ^ ck3;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m2 = (((m2 & ckm7) + ckm7) | m2) & ckm8;
    m3 = (((m3 & ckm7) + ckm7) | m3) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    m2 = (m2 - (m2 >> cksh)) + m2;
    m3 = (m3 - (m3 >> cksh)) + m3;
    lptr[i] = (( (( (bg & (~m0)) |
                  (s0 & m0)) & (~m2)) |
               (s2 & m2)) & (~m3)) |
              (s3 & m3);
    s0 = c0 << p0sl;
    s2 = c2 << p2sl;
    s3 = c3 << p3sl;
   }
   break;

  default:     /* Mode 0x1B; 0x1F             -> L0-CK; L1-CK; L2-CK; L3-CK */

   s0 = rrpge_m_edat->stat.vram[pb0 + pp0] << p0sl;
   s1 = rrpge_m_edat->stat.vram[pb1 + pp1] << p1sl;
   s2 = rrpge_m_edat->stat.vram[pb2 + pp2] << p2sl;
   s3 = rrpge_m_edat->stat.vram[pb3 + pp3] << p3sl;
   for (i = 0; i < 80U; i++){
    pp0 = (pp0 + 1U) & vpart[0];
    pp1 = (pp1 + 1U) & vpart[1];
    pp2 = (pp2 + 1U) & vpart[2];
    pp3 = (pp3 + 1U) & vpart[3];
    c0 = rrpge_m_edat->stat.vram[pb0 + pp0];
    c1 = rrpge_m_edat->stat.vram[pb1 + pp1];
    c2 = rrpge_m_edat->stat.vram[pb2 + pp2];
    c3 = rrpge_m_edat->stat.vram[pb3 + pp3];
    s0 |= (c0 >> p0sr) >> p0sr;
    s1 |= (c1 >> p1sr) >> p1sr;
    s2 |= (c2 >> p2sr) >> p2sr;
    s3 |= (c3 >> p3sr) >> p3sr;
    m0 = (s0 & gmsk) ^ ck0;
    m1 = (s1 & gmsk) ^ ck1;
    m2 = (s2 & gmsk) ^ ck2;
    m3 = (s3 & gmsk) ^ ck3;
    m0 = (((m0 & ckm7) + ckm7) | m0) & ckm8;
    m1 = (((m1 & ckm7) + ckm7) | m1) & ckm8;
    m2 = (((m2 & ckm7) + ckm7) | m2) & ckm8;
    m3 = (((m3 & ckm7) + ckm7) | m3) & ckm8;
    m0 = (m0 - (m0 >> cksh)) + m0;
    m1 = (m1 - (m1 >> cksh)) + m1;
    m2 = (m2 - (m2 >> cksh)) + m2;
    m3 = (m3 - (m3 >> cksh)) + m3;
    lptr[i] = (( (( (( (bg & (~m0)) |
                     (s0 & m0)) & (~m1)) |
                  (s1 & m1)) & (~m2)) |
               (s2 & m2)) & (~m3)) |
              (s3 & m3);
    s0 = c0 << p0sl;
    s1 = c1 << p1sl;
    s2 = c2 << p2sl;
    s3 = c3 << p3sl;
   }
   break;

 }

 /* Send rendered data to host */
 rrpge_m_edat->cb_lin(rrpge_m_edat, ln, lptr);
}
