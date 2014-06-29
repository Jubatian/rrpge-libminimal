/**
**  \file
**  \brief     Graphics accelerator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.29
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
 /* Destination fraction (incrementing), whole (stationary), increment and post-add */
 auint  dsfrac = ((auint)(rrpge_m_edat->stat.ropd[0xEFCU]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEFDU]);
 auint  dswhol = ((auint)(rrpge_m_edat->stat.ropd[0xEE9U]));
 auint  dsincr = ((auint)(rrpge_m_edat->stat.ropd[0xEEAU])); /* Will need post-processing */
 auint  dspadd = ((auint)(rrpge_m_edat->stat.ropd[0xEFEU]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEFFU]);
 auint  dsfrap = dsfrac;

 /* Source X fraction (incrementing), whole (stationary), increment and post-add */
 auint  sxfrac = ((auint)(rrpge_m_edat->stat.ropd[0xEF6U]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEF7U]);
 auint  sxwhol = ((auint)(rrpge_m_edat->stat.ropd[0xEE8U]));
 auint  sxincr = ((auint)(rrpge_m_edat->stat.ropd[0xEF8U]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEF9U]);
 auint  sxpadd = ((auint)(rrpge_m_edat->stat.ropd[0xEFAU]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEFBU]);
 auint  sxfrap = sxfrac;

 /* Source Y fraction (incrementing), increment and post-add */
 auint  syfrac = ((auint)(rrpge_m_edat->stat.ropd[0xEF0U]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEF1U]);
 auint  syincr = ((auint)(rrpge_m_edat->stat.ropd[0xEF2U]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEF3U]);
 auint  sypadd = ((auint)(rrpge_m_edat->stat.ropd[0xEF4U]) << 16) + (auint)(rrpge_m_edat->stat.ropd[0xEF5U]);
 auint  syfrap = syfrac;

 /* Partitioning & X/Y split */
 auint  ssplit = ((auint)(rrpge_m_edat->stat.ropd[0xEEBU]));
 auint  srpart;  /* Partition mask for source - controls the sxwhol - sx/yfrac split */
 auint  dspart;  /* Partition mask for destination - controls the dswhol - dsfrac split */

 uint32 wrmask = ((uint32)(rrpge_m_edat->stat.ropd[0xEE0U]) << 16) + (uint32)(rrpge_m_edat->stat.ropd[0xEE1U]);

 auint  count  = ((auint)(rrpge_m_edat->stat.ropd[0xEEEU])); /* Count of 4bit px units to copy */
 auint  codst;                                       /* Destination oriented (bit) count */
 uint32 ckey;
 auint  flags  = rrpge_m_edat->stat.ropd[0xEEDU];    /* Colorkey also here, exported in ckey later */
 uint32 mandr  = rrpge_m_edat->stat.ropd[0xEECU];    /* Read AND mask */
 uint32 mandl;
 uint32 mskor;                                       /* Read OR mask */
 auint  rotr;                                        /* Read rotation */
 auint  rotl;
 auint  dshfr;                                       /* Destination alignment shifts (Block Blitter) */
 auint  dshfl;
 uint32 prevs = 0U;                                  /* Source -> destination aligning shifter memory */
 uint32 bmems;                                       /* Begin / Mid / End mask */
 uint32 reinm;                                       /* Reindex mask */
 uint32 sdata = rrpge_m_edat->stat.ropd[0xEEFU];     /* Source data preparation - line mode pattern */
 auint  i;
 auint  lflp;
 auint  lpat;
 uint32 t32;
 uint32 u32;

 /* If necessary inicialize the reindex bank cache */

 rrpge_m_grop_recbinit();

 /* Calculate destination increment */

 if ((dsincr & 0x0020U) == 0U){
  dsincr = 10000U;
 }else{
  dsincr = (dsincr & 0xFF30U) << 10;
  dsincr = (0U - (dsincr & 0x02000000U)) | dsincr;
 }

 /* Prepare whole parts */

 dswhol = dswhol | (dswhol << 16);
 sxwhol = sxwhol | (sxwhol << 16);

 /* Mask source & destination to video RAM size (power of 2 multiple of 64K) */

 dswhol &= VRAMS - 1U;
 sxwhol &= VRAMS - 1U;

 /* Calculate source & destination splits based on partition settings */

 rotr   = ssplit;             /* Save loaded register for rotation */
 srpart = ssplit;
 dspart = ssplit;
 srpart = (((auint)(2U)) << ((srpart >> 12) & 0xFU)) - 1U;
 ssplit = (((auint)(2U)) << ((ssplit >>  8) & 0xFU)) - 1U;
 dspart = (((auint)(2U)) << ((dspart >>  4) & 0xFU)) - 1U;
 ssplit = ssplit & srpart;    /* Limit to at most source partition size */

 /* Finalize whole parts according to the partition sizes */

 dswhol = dswhol & (~dspart);
 sxwhol = sxwhol & (~srpart); /* X whole selects the stationary part (above Y) */
 srpart = srpart & (~ssplit); /* Select only the Y component with srpart */

 /* Add graphics mode to flags, so it is more accessible (increases locality
 ** accessing this). */

 flags |= (rrpge_m_info.vbm & 0x80U) << 9; /* bit 16 set if in 8bit mode */

 /* Pre-calculate bank pointer for reindex modes */

 rrpge_m_grop_reb = &rrpge_m_info.grb[0];
 if ((flags & 0x3000U) != 0x3000U){  /* Not blending mode: init to requested bank */
  rrpge_m_grop_reb += ((auint)(rrpge_m_edat->stat.ropd[0xEEAU] & 0x1FU) << 4);
  reinm = 0x00000000U;               /* Normal reindex mode masks destination */
 }else{
  reinm = wrmask;                    /* In blending reindex mode the destination passes the write mask */
 }

 /* Prepare OR mask */

 mskor = mandr >> 8;

 /* Mode specific calculations */

 if ((flags & 0x10000U) == 0U){      /* 4bit mode specific calculations */

  /* Calculate count */
  count = ((count - 1U) & 0x3FFU) + 1U;

  /* Prepare colorkey */
  ckey   = flags & 0xFU;
  ckey  |= ckey << 4;

  /* Source read mask & barrel rotate calculation. In C a right and a left
  ** shift has to be combined for the effect (no rotate operation). Used in
  ** BB and SC modes. */
  rotr  &= 0x3U;
  rotl   = 4U - rotr;
  mandr &= 0xFU;
  mandl  = (mandr << rotl) & 0xFU;
  mandr  = (mandr >> rotr);
  mandl |= mandl << 4;
  mandr |= mandr << 4;
  mskor &= 0xFU;
  mskor |= mskor << 4;

  /* Init right shift to destination. Used in Scaled & Block Blitter, and in
  ** Filler to prepare the begin cell. */
  dshfr  = (dsfrac & 0xE000U) >> 11;

 }else{                              /* 8bit mode specific calculations */

  /* Calculate count */
  count = ((count - 2U) & 0x3FEU) + 2U;

  /* Prepare colorkey */
  ckey   = flags & 0xFFU;

  /* Source read mask & barrel rotate calculation. In C a right and a left
  ** shift has to be combined for the effect (no rotate operation). Used in
  ** BB snd SC modes. */
  rotr  &= 0x7U;
  rotl   = 8U - rotr;
  mandr &= 0xFFU;
  mandl  = (mandr << rotl) & 0xFFU;
  mandr  = (mandr >> rotr);
  mskor &= 0xFFU;

  /* Init right shift to destination. Used in Scaled & Block Blitter, and in
  ** Filler to prepare the begin cell. */
  dshfr  = (dsfrac & 0xC000U) >> 11;

 }

 /* Update post-add values if needed */

 if ((flags & 0x8000U) != 0U){       /* Destination post-add formed from count */
  dspadd = count << 13;
 }
 if ((flags & 0x4000U) != 0U){       /* Source X post-add formed from count */
  sxpadd = count << 13;
 }

 /* Replicate colorkey & source read mask to the whole 32bit unit */

 ckey  |= ckey  << 8;
 mandl |= mandl << 8;
 mandr |= mandr << 8;
 mskor |= mskor << 8;
 ckey  |= ckey  << 16;
 mandl |= mandl << 16;
 mandr |= mandr << 16;
 mskor |= mskor << 16;

 /* Prepare pattern for Line & Filler mode */

 sdata |= sdata << 16;

 /* Calculate begin mask */

 bmems  = 0xFFFFFFFFU >> dshfr;
 bmems &= wrmask;

 /* Calculate destination oriented (bit) count (for the Filler this is the
 ** last use of count). */

 codst = (count << 2) + dshfr;

 /* Prepare for the Block Blitter: apply the source fraction, and set up the
 ** omission of the first destination combine if necessary (truly fetch an
 ** extra source first to populate the shift register proper). The shift
 ** changes as combining the source & destination fractions. */

 if ((flags & 0x0C00U) == 0U){       /* Block Blitter */
  i = (dsfrac & 0xFFFFU) - (sxfrac & 0xFFFFU);
  flags |= (i << 1) & 0x20000U;      /* Wrapped around - so omit first combine */
  if ((flags & 0x10000U) == 0U){ i &= 0xE000U; } /* 4bit mode */
  else{                          i &= 0xC000U; } /* 8bit mode */
  dshfr = i >> 11;
 }

 /* Calculate source to destination shift. Used in Scaled & Block Blitter */

 dshfl = (32U - dshfr) >> 1;         /* Could be 32, so split up the shift in two parts */

 /* Setups for Line mode */

 if ((flags & 0x0C00U) == 0x0C00U){  /* Line */
  lflp   = 0U;                       /* Odd / even flip for cycling the pattern */
  lpat   = sdata;                    /* Line pattern cycle */
  dswhol = sxwhol;
  dspart = ssplit | srpart;          /* Source pointers take the role of dest. */
  codst  = count;                    /* Destination counter - pixel count */
 }

 /* Run the main rendering loop. Each iteration processes one Video RAM cell,
 ** while the loop mostly relies on a proper branch predictor to lock on the
 ** appropriate path through it (most of the conditionals branch by the
 ** operation mode flags which are fixed through an operation). */

 while (1){

  /* Check end of blit condition and produce an end mask if so. Note that more
  ** than 0 pixels are remaining at this point which is relied upon for
  ** generating the shift. */

  if ( ((flags & 0x20000U) == 0U) &&       /* Destination combine enabled for this run */
       ((flags &  0x0C00U) != 0x0C00U) ){  /* And it is not Line mode */
   if (codst < 32U){                       /* Fewer than 8 (4bit) destination pixels remaining */
    bmems &= 0xFFFFFFFFU << (32U - codst); /* Generate an end mask */
    codst  = 0U;
   }else{
    codst -= 32U;
   }
  }

  /* Source preparation and pixel counting. This stage produces the initial
  ** source in sdata, which can be post-processed by the Block Blitter. */

  if ((flags & 0x0C00U) == 0x0800U){ /* Scaled blitter */

   /* If it is Scaled Mode, prepare the source data (sdata) from the
   ** appropriate number of individual source pixels. */

   sdata = 0U;

   if ((flags & 0x10000U) == 0U){    /* 4bit mode */

    i = 8U;
    while ((count != 0U) && (i != 0U)){
     count --;
     i     --;
     sdata |= ( ( rrpge_m_edat->stat.vram[sxwhol |
                                          ((syfrac >> 16) & srpart) |
                                          ((sxfrac >> 16) & ssplit)] >>
                  (28U - ((sxfrac & 0xE000U) >> 11)) ) &  0xFU ) << (i << 2);
     sxfrac += sxincr;
     syfrac += syincr;
    }

   }else{                            /* 8bit mode */

    i = 4U;
    while ((count != 0U) && (i != 0U)){
     count -= 2U;                    /* Note: count is even, so won't overflow */
     i     --;
     sdata |= ( ( rrpge_m_edat->stat.vram[sxwhol |
                                          ((syfrac >> 16) & srpart) |
                                          ((sxfrac >> 16) & ssplit)] >>
                  (24U - ((sxfrac & 0xC000U) >> 11)) ) & 0xFFU ) << (i << 3);
     sxfrac += sxincr;
     syfrac += syincr;
    }

   }

  }else if ((flags & 0x0C00U) == 0U){   /* Block blitter */

   /* In Block Blitter the source data is simply the next Video RAM cell.
   ** Simplified: it will fetch a trailing cell (count already depleted, but
   ** destination is still generated), which a real hardware shouldn't do. */

   sdata = rrpge_m_edat->stat.vram[sxwhol |
                                   ((syfrac >> 16) & srpart) |
                                   ((sxfrac >> 16) & ssplit)];
   sxfrac += sxincr;
   syfrac += syincr;
   count -= 8U;
   count &= ((~(auint)(0U)) + ((count & 0x80000000U) >> 31)); /* Zero saturate */

  }else if ((flags & 0x0C00U) == 0x0C00U){ /* Line */

   /* Rotate source pattern & create begin / mid / end mask */

   if ((flags & 0x10000U) == 0U){    /* 4bit mode */

    lpat >>= (lflp << 2);
    sdata  = lpat & 0xFU;
    i      = ((sxfrac & 0xE000U) >> 11);
    bmems  =  0xFU << i;
    codst --;

   }else{                            /* 8bit mode */

    lpat >>= (lflp << 3);            /* Bogus gcc warning: lpat is initialized before the render loop for Line mode */
    sdata  = lpat & 0xFFU;
    i      = ((sxfrac & 0xC000U) >> 11);
    bmems  = 0xFFU << i;
    codst -= 2U;

   }

   /* Re-expand source pattern (rotate!), finalize data */

   lpat   = (lpat & 0xFFFFU) | (lpat << 16);
   lflp  ^= 1U;                      /* Rotate at every second pixel */
   sdata  = sdata << i;
   bmems &= wrmask;

   /* Create destination fraction & increment "source" pointers */

   dsfrac = (syfrac & (srpart << 16)) | (sxfrac & (ssplit << 16));
   sxfrac += sxincr;
   syfrac += syincr;

  }else{                             /* Filler - do nothing */
  }

  /* Scaled and Block Blitter source operation chains are uniform from now,
  ** just perform the appropriate tasks to generate the source. */

  if ((flags & 0x0400U) == 0U){         /* Not Line or Filler mode */
   sdata = ((sdata >> rotr) & mandr) |  /* Source read transform */
           ((sdata << rotl) & mandl);   /* (Rotate + AND mask) */
   sdata = sdata | mskor;
   if ((flags & 0x0100U) != 0U){        /* Pixel order swap */
    if ((flags & 0x10000U) == 0U){      /* 4 bit mode */
     sdata = ((sdata & 0xF0F0F0F0U) >> 4) | ((sdata & 0x0F0F0F0FU) << 4);
    }
    sdata = ((sdata & 0xFF00FF00U) >> 8) | ((sdata & 0x00FF00FFU) << 8);
    sdata = (sdata >> 16) | (sdata << 16);   /* Mirror source (BSWAP would be good here) */
   }
   t32   = sdata;                       /* Align to destination */
   sdata = prevs | (sdata >> dshfr);
   prevs = (t32 << dshfl) << dshfl;
  }

  /* Start destination combine */

  if ((flags & 0x20000U) != 0U){        /* Omit first combine requested */

   flags &= ~(auint)(0x20000U);         /* Clear it, no combine */

  }else{                                /* Destination combine proceeds */

   /* Destination combine stage begins, common for all modes. Create and apply
   ** the colorkey, perform reindexing as required, then blit it. The colorkey
   ** is always calculated: it usually does not affect accelerator cycle
   ** count, and in typical blits it is necessary anyway. */

   i   = dswhol | ((dsfrac >> 16) & dspart); /* Destination offset */
   u32 = rrpge_m_edat->stat.vram[i];    /* Load current destination */
   t32 = sdata ^ ckey;                  /* Prepare for colorkey calculation */

   if ((flags & 0x10000U) == 0U){       /* 4 bit mode */
    t32 = (((t32 & 0x77777777U) + 0x77777777U) | t32) & 0x88888888U;
    t32 = (t32 - (t32 >> 3)) + t32;     /* Colorkey mask (0: background) */
    if ((flags & 0x1000U) != 0U){       /* Reindexing is required */
     sdata = rrpge_m_grop_rec4(sdata, u32 & reinm);
    }
   }else{                               /* 8 bit mode */
    t32 = (((t32 & 0x7F7F7F7FU) + 0x7F7F7F7FU) | t32) & 0x80808080U;
    t32 = (t32 - (t32 >> 7)) + t32;     /* Colorkey mask (0: background) */
    if ((flags & 0x1000U) != 0U){       /* Reindexing is required */
     sdata = rrpge_m_grop_rec8(sdata, u32 & reinm);
    }
   }
   bmems &= t32 | (0xFFFFFFFFU + ((flags >> 9) & 1U)); /* Add colorkey to write mask if enabled */

   /* Combine source over destination */

   rrpge_m_edat->stat.vram[i] = (u32   & (~bmems)) |
                                (sdata &   bmems );
   dsfrac += dsincr;

   /* The rendering loop ends when it ran out of destination to render. */

   if (codst == 0U){ break; }

   /* Update begin-mid-end mask: Just the write mask */

   bmems = wrmask;

  }

 }; /* End of rendering loop */

 /* Finalize by post-adding and writing back. */

 dsfrap += dspadd;
 sxfrap += sxpadd;
 syfrap += sypadd;

 rrpge_m_edat->stat.ropd[0xEF0U] = (uint16)(syfrap >> 16); /* Source whole */
 rrpge_m_edat->stat.ropd[0xEF1U] = (uint16)(syfrap);       /* Source fraction */
 rrpge_m_edat->stat.ropd[0xEF6U] = (uint16)(sxfrap >> 16); /* Source whole */
 rrpge_m_edat->stat.ropd[0xEF7U] = (uint16)(sxfrap);       /* Source fraction */
 rrpge_m_edat->stat.ropd[0xEFCU] = (uint16)(dsfrap >> 16); /* Destination whole */
 rrpge_m_edat->stat.ropd[0xEFDU] = (uint16)(dsfrap);       /* Destination fraction */

}
