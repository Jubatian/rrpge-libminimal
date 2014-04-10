/**
**  \file
**  \brief     Graphics accelerator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.04.04
*/


#include "rgm_grop.h"


/* Video memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  VRAMS  RRPGE_M_VRAMS



/* Internal: Rebuild reindex cache in info structure if necessary */
static void rrpge_m_grop_recbinit(void)
{
 auint i;

 if (rrpge_m_info.grr){ /* Need to rebuild */
  rrpge_m_info.grr = 0;

  i = 256U;
  do{
   i--;
   rrpge_m_info.grb[(i << 1)     ] = (rrpge_m_edat->stat.ropd[0xF00U + i] >> 8) & rrpge_m_info.vbm;
   rrpge_m_info.grb[(i << 1) + 1U] = (rrpge_m_edat->stat.ropd[0xF00U + i]     ) & rrpge_m_info.vbm;
  }while(i);

 }
}



/* Reindex bank base pointer, pointing into the appropriate bank of
** rrpge_m_info.grb[]. For destination based reindexing (blending) this is set
** to the entry of the reindex array, for normal reindexing it is set to the
** appropriate bank, and zero is supplied in the pd parameter of the reindex
** functions. */
static uint8 const* rrpge_m_grop_reb;


/* Internal: Calculates reindex 32bit chunk for 4bit */
RRPGE_M_FASTCALL static uint32 rrpge_m_grop_rec4(uint32 ps, uint32 pd)
{
 uint32 t0 = (ps & 0x0F0F0F0FU) | ((pd << 4) & 0xF0F0F0F0U);
 uint32 t1 = ((ps >> 4) & 0x0F0F0F0FU) | (pd & 0xF0F0F0F0U);
 return ((uint32)(rrpge_m_grop_reb[(t0      ) & 0xFFU])      ) |
        ((uint32)(rrpge_m_grop_reb[(t1      ) & 0xFFU]) <<  4) |
        ((uint32)(rrpge_m_grop_reb[(t0 >>  8) & 0xFFU]) <<  8) |
        ((uint32)(rrpge_m_grop_reb[(t1 >>  8) & 0xFFU]) << 12) |
        ((uint32)(rrpge_m_grop_reb[(t0 >> 16) & 0xFFU]) << 16) |
        ((uint32)(rrpge_m_grop_reb[(t1 >> 16) & 0xFFU]) << 20) |
        ((uint32)(rrpge_m_grop_reb[(t0 >> 24)        ]) << 24) |
        ((uint32)(rrpge_m_grop_reb[(t1 >> 24)        ]) << 28);
}


/* Internal: Calculates reindex 32bit chunk for 8bit */
RRPGE_M_FASTCALL static uint32 rrpge_m_grop_rec8(uint32 ps, uint32 pd)
{
 uint32 t0 = ( ps       & 0x000F000FU) | ((pd << 4) & 0x01F001F0U);
 uint32 t1 = ((ps >> 8) & 0x000F000FU) | ((pd >> 4) & 0x01F001F0U);
 return ((uint32)(rrpge_m_grop_reb[t0 & 0xFFFFU])      ) |
        ((uint32)(rrpge_m_grop_reb[t1 & 0xFFFFU]) <<  8) |
        ((uint32)(rrpge_m_grop_reb[t0   >>   16]) << 16) |
        ((uint32)(rrpge_m_grop_reb[t1   >>   16]) << 24);
}





/* Executes a Graphic accelerator operation. Rebuilds the reindex cache if
** necessary (clearing rrpge_m_info.grr). Updates accelerator's pointers in
** the ROPD as defined in the specification. Does not calculate timing. */
void rrpge_m_grop_accel(void)
{
 auint  dfrac = ((auint)(rrpge_m_edat->stat.ropd[0xEF2U]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEF3U]);
 auint  dwhol = ((auint)(rrpge_m_edat->stat.ropd[0xEFBU]) << 16);
 auint  dincr = ((auint)(rrpge_m_edat->stat.ropd[0xEF5U]) << 16); /* Whole part only */
 auint  sfrbb = ((auint)(rrpge_m_edat->stat.ropd[0xEF0U]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEF1U]);
 auint  swhbb = ((auint)(rrpge_m_edat->stat.ropd[0xEFAU]) << 16);
 auint  sinbb = ((auint)(rrpge_m_edat->stat.ropd[0xEF4U]) << 16);
 auint  sfxsc = ((auint)(rrpge_m_edat->stat.ropd[0xEE8U]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEE9U]);
 auint  swxsc = swhbb;
 auint  sixsc = ((auint)(rrpge_m_edat->stat.ropd[0xEECU]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEEDU]);
 auint  sfysc = ((auint)(rrpge_m_edat->stat.ropd[0xEEAU]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEEBU]);
 auint  swysc = swhbb;
 auint  siysc = ((auint)(rrpge_m_edat->stat.ropd[0xEEEU]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEEFU]);
 auint  sspsc = ((auint)(rrpge_m_edat->stat.ropd[0xEFCU]));
 auint  spart;  /* Partition mask for source - controls the swhol - sfrac split */
 auint  dpart;  /* Partition mask for destination - controls the dwhol - dfrac split */
 uint32 wmask = ((uint32)(rrpge_m_edat->stat.ropd[0xEE0U]) << 16) + (uint32)(rrpge_m_edat->stat.ropd[0xEE1U]);
 auint  count = ((auint)(rrpge_m_edat->stat.ropd[0xEF9U])); /* Count of 4bit px units to copy */
 auint  codst;                                       /* Destination oriented (bit) count */
 uint32 ckey;
 auint  flags = rrpge_m_edat->stat.ropd[0xEF8U];     /* Colorkey also here, exported in ckey later */
 uint32 maskr = rrpge_m_edat->stat.ropd[0xEF7U];     /* Read AND mask */
 uint32 maskl;
 auint  rotr  = rrpge_m_edat->stat.ropd[0xEF7U] >> 8;   /* Read rotation */
 auint  rotl;
 auint  dshfr;                                       /* Destination alignment shifts (Block Blitter) */
 auint  dshfl;
 uint32 prevs = 0U;                                  /* Source -> destination aligning shifter memory */
 uint32 bmems;                                       /* Begin / Mid / End mask */
 uint32 reinm;                                       /* Reindex mask */
 uint32 sdata = rrpge_m_edat->stat.ropd[0xEFFU];     /* Source data preparation - line mode pattern */
 auint  renex = 0U;                                  /* Exit from render request */
 auint  i;
 uint32 t32;

 /* If necessary inicialize the reindex bank cache */

 rrpge_m_grop_recbinit();

 /* Mask source & destination to video RAM size (power of 2 multiple of 64K) */

 swhbb &= VRAMS - 1U;
 swxsc &= VRAMS - 1U;
 swysc &= VRAMS - 1U;
 dwhol &= VRAMS - 1U;

 /* Calculate source & destination splits based on partition settings */

 spart  = (rrpge_m_edat->stat.ropd[0xEF7U] >> 11);   /* spart: 0 - 31 */
 spart  = ((auint)(  1U) << spart) - 1U;             /* Source partition mask */
 dpart  = (rrpge_m_edat->stat.ropd[0xEE2U] & 0x7U);  /* dpart: 0 - 7 */
 dpart  = ((auint)(512U) << dpart) - 1U;             /* Global partition mask */
 spart  = spart & dpart;             /* Source mask is restricted to global mask */
 swhbb |= sfrbb >> 16;
 swxsc |= sfxsc >> 16;
 swysc |= sfysc >> 16;
 dwhol |= dfrac >> 16;
 swhbb &= ~spart;
 swxsc &= ~spart;
 swysc &= ~spart;
 dwhol &= ~dpart;
 spart  = (spart << 16) | 0xFFFFU;
 dpart  = (dpart << 16) | 0xFFFFU;
 sfrbb &= spart;
 sfxsc &= spart;
 sfysc &= spart;
 dfrac &= dpart;

 /* Add graphics mode to flags, so it is more accessible (increases locality
 ** accessing this). */

 flags |= (rrpge_m_info.vbm & 0x80U) << 8; /* bit 15 set if in 8bit mode */

 /* Pre-calculate bank pointer for reindex modes */

 rrpge_m_grop_reb = &rrpge_m_info.grb[0];
 if ((flags & 0x3000U) != 0x3000U){  /* Not blending mode: init to requested bank */
  rrpge_m_grop_reb += ((auint)(rrpge_m_edat->stat.ropd[0xEF6U] & 0x1FU) << 4);
  reinm = 0x00000000U;               /* Normal reindex mode masks destination */
 }else{
  reinm = wmask;                     /* In blending reindex mode the destination passes the write mask */
 }

 /* Mode specific calculations */

 if ((flags & 0x8000U) == 0U){       /* 4bit mode specific calculations */

  /* Calculate count */
  count = ((count - 1U) & 0x3FFU) + 1U;

  /* Prepare colorkey */
  ckey   = flags & 0xFU;
  ckey  |= ckey << 4;

  /* Source read mask & barrel rotate calculation. In C a right and a left
  ** shift has to be combined for the effect (no rotate operation). Used in
  ** all except line mode. */
  rotr  &= 0x3U;
  rotl   = 4U - rotr;
  maskr &= 0xFU;
  maskl  = (maskr << rotl) & 0xFU;
  maskr  = (maskr >> rotr);
  maskl |= maskl << 4;
  maskr |= maskr << 4;

  /* Init right shift to destination. Used in Scaled & Block Blitter, and in
  ** Line filler to prepare the begin cell. */
  dshfr  = (dfrac & 0xE000U) >> 11;

 }else{                              /* 8bit mode specific calculations */

  /* Calculate count */
  count = ((count - 2U) & 0x3FEU) + 2U;

  /* Prepare colorkey */
  ckey   = flags & 0xFFU;

  /* Source read mask & barrel rotate calculation. In C a right and a left
  ** shift has to be combined for the effect (no rotate operation). Used in
  ** all except line mode. */
  rotr  &= 0x7U;
  rotl   = 8U - rotr;
  maskr &= 0xFFU;
  maskl  = (maskr << rotl) & 0xFFU;
  maskr  = (maskr >> rotr);

  /* Init right shift to destination. Used in Scaled & Block Blitter, and in
  ** Line filler to prepare the begin cell. */
  dshfr  = (dfrac & 0xC000U) >> 11;

 }

 /* Replicate colorkey & source read mask to the whole 32bit unit */

 ckey  |= ckey  << 8;
 maskl |= maskl << 8;
 maskr |= maskr << 8;
 ckey  |= ckey  << 16;
 maskl |= maskl << 16;
 maskr |= maskr << 16;

 /* Prepare pattern for Line mode */

 sdata |= sdata << 16;

 /* Calculate begin mask */

 bmems  = 0xFFFFFFFFU >> dshfr;
 bmems &= wmask;

 /* Calculate destination oriented (bit) count (for the Line Filler this is
 ** the last use of count). */

 codst = (count << 2) + dshfr;

 /* Prepare for the Block Blitter: apply the source fraction, and set up the
 ** omission of the first destination combine if necessary. */

 flags &= 0xBFFFU;                   /* bit 14 will hold the need for omission */
 if ((flags & 0x0C00U) == 0U){       /* Block Blitter */
  i = (dfrac & 0xFFFFU) - (sfrbb & 0xFFFFU);
  flags |= (i >> 2) & 0x4000U;       /* Wrapped around - so omit first combine */
  if ((flags & 0x8000U) == 0U){ i &= 0xE000U; }  /* 4bit mode */
  else{                         i &= 0xC000U; }  /* 8bit mode */
  dshfr = i >> 11;
  i = (sfrbb + (count << 13));       /* Calculate new source fraction */
  if ((flags & 0x8000U) == 0U){      /* 4bit mode */
   sfrbb = (sfrbb & 0xFFFF1FFFU) | (i & 0xE000U);
  }else{                             /* 8bit mode */
   sfrbb = (sfrbb & 0xFFFF3FFFU) | (i & 0xC000U);
  }
 }

 /* Calculate source to destination shift. Used in Scaled & Block Blitter */

 dshfl = (32U - dshfr) >> 1;         /* Could be 32, so split up the shift in two parts */

 /* Run the main rendering loop. Each iteration processes one Video RAM cell,
 ** while the loop mostly relies on a proper branch predictor to lock on the
 ** appropriate path through it (most of the conditionals branch by the
 ** operation mode flags which are fixed through an operation). */

 while (1){

  /* Check end of blit condition and produce an end mask if so. Note that more
  ** than 0 pixels are remaining at this point which is relied upon for
  ** generating the shift. */

  if ((flags & 0x4000) == 0U){       /* Destination combine enabled for this run */
   if (codst < 32U){                 /* Fewer than 8 (4bit) destination pixels remaining */
    bmems &= 0xFFFFFFFFU << (32U - codst);
    renex  = 1U;                     /* Ask exit from render at end */
   }else{
    codst -= 32U;
   }
  }

  /* Source preparation and pixel counting. This stage produces the initial
  ** source in sdata, which can be post-processed by the Block Blitter. */

  if ((flags & 0x0C00U) == 0x0800U){      /* Scaled blitter */

   /* If it is Scaled Mode, prepare the source data (sdata) from the
   ** appropriate number of individual source pixels. */

   sdata = 0U;

   if ((flags & 0x8000U) == 0U){          /* 4bit mode */

    i = 8U;
    while ((count != 0U) && (i != 0U)){
     count --;
     i     --;
     sdata |= ( ( rrpge_m_edat->stat.vram[((swxsc | (sfxsc >> 16)) & ( sspsc)) |
                                          ((swysc | (sfysc >> 16)) & (~sspsc))] >>
                  (28U - ((sfxsc & 0xE000U) >> 11)) ) &  0xFU ) << (i << 2);
     sfxsc  = (sfxsc + sixsc) & spart;
     sfysc  = (sfysc + siysc) & spart;
    }

   }else{                                 /* 8bit mode */

    i = 4U;
    while ((count != 0U) && (i != 0U)){
     count -= 2U;                         /* Note: count is even, so won't overflow */
     i     --;
     sdata |= ( ( rrpge_m_edat->stat.vram[((swxsc | (sfxsc >> 16)) & ( sspsc)) |
                                          ((swysc | (sfysc >> 16)) & (~sspsc))] >>
                  (24U - ((sfxsc & 0xC000U) >> 11)) ) & 0xFFU ) << (i << 3);
     sfxsc  = (sfxsc + sixsc) & spart;
     sfysc  = (sfysc + siysc) & spart;
    }

   }

  }else{

   if ((flags & 0x0C00U) == 0U){          /* Block blitter */

    /* In Block Blitter the source data is simply the next Video RAM cell. */

    if (count != 0U){                     /* Within trailing destination cell? */
     sdata = rrpge_m_edat->stat.vram[swhbb | (sfrbb >> 16)];
    }
    if (count >= 8U){
     count -= 8U;
     sfrbb = (sfrbb + sinbb) & spart;
    }else{
     count  = 0U;
    }

    /* Note that count is not needed at all for the Line filler, it can
    ** operate solely on codst. */

   }

  }

  /* Scaled and Block Blitter source operation chains are uniform from now,
  ** just perform the appropriate tasks to generate the source. */

  if ((flags & 0x0400U) == 0U){           /* Not line filler mode */
   sdata = ((sdata >> rotr) & maskr) |    /* Source read transform */
           ((sdata << rotl) & maskl);     /* (Rotate + AND mask) */
   if ((flags & 0x0100U) != 0U){          /* Pixel order swap */
    if ((flags & 0x8000U) == 0U){         /* 4 bit mode */
     sdata = ((sdata & 0xF0F0F0F0U) >> 4) | ((sdata & 0x0F0F0F0FU) << 4);
    }
    sdata = ((sdata & 0xFF00FF00U) >> 8) | ((sdata & 0x00FF00FFU) << 8);
    sdata = (sdata >> 16) | (sdata << 16);   /* Mirror source (BSWAP would be good here) */
   }
   t32   = sdata;                         /* Align to destination */
   sdata = prevs | (sdata >> dshfr);
   prevs = (t32 << dshfl) << dshfl;
  }

  /* Start destination combine */

  if ((flags & 0x4000U) != 0U){           /* Omit first combine requested */

   flags &= 0xBFFFU;                      /* Clear it, no combine */

  }else{                                  /* Destination combine proceeds */

   /* Destination combine stage begins, common for all modes. First prepare the
   ** colorkey. */

   if ((flags & 0x0200U) != 0U){          /* Has colorkey */
    t32 = sdata ^ ckey;
    if ((flags & 0x8000U) == 0U){         /* 4 bit mode */
     t32 = (((t32 & 0x77777777U) + 0x77777777U) | t32) & 0x88888888U;
     t32 = (t32 - (t32 >> 3)) + t32;      /* Colorkey mask (0: background) */
    }else{                                /* 8 bit mode */
     t32 = (((t32 & 0x7F7F7F7FU) + 0x7F7F7F7FU) | t32) & 0x80808080U;
     t32 = (t32 - (t32 >> 7)) + t32;      /* Colorkey mask (0: background) */
    }
    bmems &= t32;                         /* Add it to the write mask */
   }

   /* Load destination and perform reindexing if it was requested */

   i   = dwhol | (dfrac >> 16);
   t32 = rrpge_m_edat->stat.vram[i];
   if ((flags & 0x1000U) != 0U){          /* Reindexing is required */
    if ((flags & 0x8000U) == 0U){         /* 4 bit mode */
     sdata = rrpge_m_grop_rec4(sdata, t32 & reinm);
    }else{                                /* 8 bit mode */
     sdata = rrpge_m_grop_rec8(sdata, t32 & reinm);
    }
   }

   /* Combine source over destination */

   rrpge_m_edat->stat.vram[i] = (t32   & (~bmems)) |
                                (sdata &   bmems );

   /* The rendering loop ends when it ran out of source pixels. Reaching this
   ** everything except the new destination pointer is properly generated.
   ** Note that the destination pointer whole part has to update as many times
   ** as many completed destination writes are performed, that's why there are
   ** 2 exit points. */

   if (renex != 0U){ break; }

   dfrac = (dfrac + dincr) & dpart;

   if (codst == 0U){ break; }

   /* Update begin-mid-end mask: Just the write mask */

   bmems = wmask;

  }

 }; /* End of rendering loop */

 /* The destination pointer fractional part updates according to what was
 ** produced in the end area, to reflect the count of pixels output. */

 if ((flags & 0x8000U) == 0U){            /* 4bit mode */
  dfrac = (dfrac & 0xFFFF1FFFU) | ((codst << 11) & 0xE000U);
 }else{                                   /* 8bit mode */
  dfrac = (dfrac & 0xFFFF3FFFU) | ((codst << 11) & 0xC000U);
 }

 /* Finish it up. The source & destination pointers have to be written back
 ** into the ROPD. */

 rrpge_m_edat->stat.ropd[0xEE8U] = (uint16)(swxsc | (sfxsc >> 16)); /* Source whole */
 rrpge_m_edat->stat.ropd[0xEE9U] = (uint16)(sfxsc);                 /* Source fraction */
 rrpge_m_edat->stat.ropd[0xEEAU] = (uint16)(swysc | (sfysc >> 16)); /* Source whole */
 rrpge_m_edat->stat.ropd[0xEEBU] = (uint16)(sfysc);                 /* Source fraction */
 rrpge_m_edat->stat.ropd[0xEF0U] = (uint16)(swhbb | (sfrbb >> 16)); /* Source whole */
 rrpge_m_edat->stat.ropd[0xEF1U] = (uint16)(sfrbb);                 /* Source fraction */
 rrpge_m_edat->stat.ropd[0xEF2U] = (uint16)(dwhol | (dfrac >> 16)); /* Destination whole */
 rrpge_m_edat->stat.ropd[0xEF3U] = (uint16)(dfrac);                 /* Destination fraction */

}
