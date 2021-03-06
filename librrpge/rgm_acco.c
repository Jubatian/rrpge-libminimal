/**
**  \file
**  \brief     Graphics accelerator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.15
*/


#include "rgm_acco.h"


/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* Cell cycle multiplier table.
** bit0: accelerated, bit1,2: blit mode, bit3,4: reindex */
static uint8 const rrpge_m_acco_tc[32] = {
 6, 4, 4, 2, 4, 2, 0, 0, 8, 8, 8, 8, 4, 2, 0, 0,
 6, 6, 4, 4, 4, 4, 0, 0, 8, 8, 8, 8, 4, 4, 0, 0};



/* Internal: Calculates reindex 32bit chunk */
RRPGE_M_FASTCALL static auint rrpge_m_acco_rec(uint8 const* reb, auint ps, auint pd)
{
 auint t0 = (ps & 0x0F0F0F0FU) | ((pd << 4) & 0xF0F0F0F0U);
 auint t1 = ((ps >> 4) & 0x0F0F0F0FU) | (pd & 0xF0F0F0F0U);
 return (( ((auint)(reb[(t0      ) & 0xFFU])      ) |
           ((auint)(reb[(t0 >>  8) & 0xFFU]) <<  8) |
           ((auint)(reb[(t0 >> 16) & 0xFFU]) << 16) |
           ((auint)(reb[(t0 >> 24)        ]) << 24) ) & 0x0F0F0F0FU) |
        (( ((auint)(reb[(t1      ) & 0xFFU]) <<  4) |
           ((auint)(reb[(t1 >>  8) & 0xFFU]) << 12) |
           ((auint)(reb[(t1 >> 16) & 0xFFU]) << 20) |
           ((auint)(reb[(t1 >> 24)        ]) << 28) ) & 0xF0F0F0F0U);
}



/* Executes a Graphic accelerator operation. Returns the number of cycles the
** accelerator operation takes. */
auint rrpge_m_acco(rrpge_object_t* hnd)
{
 /* Peripheral RAM shortcut */
 uint32* pram = &(hnd->st.pram[0]);

 /* Reindex bank select */
 uint8 const* reb;

 /* Destination fraction (incrementing), whole (stationary) and post-add */
 auint  dsfrap = hnd->acc.dof;
 auint  dswhol = ((hnd->acc.dbp) << 16) + (hnd->acc.dps);
 auint  dspadd = hnd->acc.dad;
 auint  dsfrac;

 /* Source (Pointer) X fraction (incrementing), whole (stationary), increment and post-add */
 auint  sxfrap = hnd->acc.xof;
 auint  sxwhol = ((hnd->acc.sbn) << 16) + (hnd->acc.spt);
 auint  sxincr = hnd->acc.xin;
 auint  sxpadd = hnd->acc.xad;
 auint  sxfrac;

 /* Source (Pointer) Y fraction (incrementing), increment and post-add */
 auint  syfrap = hnd->acc.yof;
 auint  syincr = hnd->acc.yin;
 auint  sypadd = hnd->acc.yad;
 auint  syfrac;

 /* Count, and it's post-add */
 auint  counb  = hnd->acc.cct;
 auint  copadd = hnd->acc.cad;
 auint  count;

 /* Partitioning & X/Y split */
 auint  ssplit = hnd->acc.sps;
 auint  srpart;      /* Partition mask for source - controls the sxwhol - sx/yfrac split */
 auint  dspart;      /* Partition mask for destination - controls the dswhol - dsfrac split */

 auint  wrmask = hnd->acc.rwm;

 auint  counr  = hnd->acc.rct; /* Count of rows to copy */
 auint  codst;       /* Destination oriented (bit) count */
 auint  ckey;
 auint  flags  = hnd->acc.rpm; /* VMR, Reindex & Read OR mask */
 auint  mandr  = hnd->acc.sms; /* Read AND mask, and colorkey exported later */
 auint  mandl;
 auint  mskor;       /* Read OR mask */
 auint  rotr   = hnd->acc.bfl; /* Read rotation & some flags */
 auint  rotl;
 auint  dshfr;       /* Destination alignment shifts (Block Blitter) */
 auint  dshfl;
 auint  prevs  = 0U; /* Source -> destination aligning shifter memory */
 auint  bmems;       /* Begin / Mid / End mask */
 auint  reinm;       /* Reindex mask */
 auint  sbase  = hnd->acc.pat; /* Source data preparation - line mode pattern */
 auint  sdata  = 0U; /* !!! Only eliminates a bogus GCC warning, see line 353 !!! */
 auint  bmode;       /* Blit mode (BB / FL / SC / LI) */
 auint  cyr;         /* Return cycle count */
 auint  cyf;         /* Flags affecting the cycle count */
 auint  i;
 auint  lflp   = 0U; /* !!! Only eliminates a bogus GCC warning, see line 353 !!! */
 auint  lpat   = 0U; /* !!! Only eliminates a bogus GCC warning, see line 353 !!! */
 auint  t;
 auint  u;

 /* Pre-calculate reindex bank pointer for reindex modes */

 reb = &(hnd->acc.grb[0]);
 if ((flags & 0x6000U) != 0x6000U){   /* Not blending mode: init to requested bank */
  reb += (flags & 0x1F00U) >> 4;
  reinm = 0x00000000U;                /* Normal reindex mode masks destination */
 }else{
  reinm = wrmask;                     /* In blending reindex mode the destination passes the write mask */
 }

 /* Calculate source & destination splits based on partition settings */

 srpart = (ssplit >> 12);
 ssplit = (ssplit >>  8) & 0xFU;
 dspart = (dswhol >> 28);
 srpart = (((auint)(2U)) << srpart) - 1U;
 ssplit = (((auint)(2U)) << ssplit) - 1U;
 dspart = (((auint)(2U)) << dspart) - 1U;

 /* Mask source & destination to video RAM size (power of 2 multiple of 64K) */

 dswhol &= PRAMS - 1U;
 sxwhol &= PRAMS - 1U;

 /* Prepare OR mask */

 mskor = flags & 0xFFU;

 /* Assemble blit control flag set.
 ** bit 14: VMR
 ** bit 13: VDR
 ** bit 12: VRE
 ** bit 11: VMD (h)
 ** bit 10: VMD (l)
 ** bit  0: VCK */

 flags = ((flags & 0xE000U) >> 1) +        /* VMR, VDR, VRE */
         ((rotr  & 0x0060U) << 5) +        /* VMD (hl) */
         ((rotr  & 0x0008U) >> 3);         /* VCK */
 bmode = (flags & 0x0C00U) >> 10;          /* 0: BB, 1: FL, 2: SC, 3: LI */

 /* Prepare colorkey */

 ckey   = mandr & 0xFU;
 ckey  |= ckey << 4;
 mandr  = (mandr >> 8) & 0x0FU;

 /* Source read mask & barrel rotate calculation. In C a right and a left
 ** shift has to be combined for the effect (no rotate operation). Used in
 ** BB and SC modes. */

 rotr  &= 0x3U;
 rotl   = 4U - rotr;
 mandl  = mandr & (0xFU << rotl);
 mandr  = mandr & (0xFU >> rotr);
 mandl |= mandl << 4;
 mandr |= mandr << 4;
 mskor &= 0xFU;
 mskor |= mskor << 4;

 /* Replicate colorkey & source read mask to the whole 32bit unit */

 ckey  |= ckey  << 8;
 mandl |= mandl << 8;
 mandr |= mandr << 8;
 mskor |= mskor << 8;
 ckey  |= ckey  << 16;
 mandl |= mandl << 16;
 mandr |= mandr << 16;
 mskor |= mskor << 16;

 /* Prepare pattern for Line & Filler modes */

 sbase |= sbase << 16;

 /* Prepare the cycle count flags. bit 0: accelerated combine (will be
 ** generated in combine), bits 1-2: blit mode; bit 3: reindex, bit 4: by
 ** destination. The accelerated combine starts set since it is easier to
 ** clear it with a xor at the combine. */

 cyf   = ((flags & 0x3C00U) >> 9) | 1U;
 cyr   = 20U + ((flags & 0x1000U) >> 9); /* Initial cycles: 20 or 28 depending on reindexing */

 /* Blit mode specific initializations */

 switch (bmode){

  case 0U:                            /* Block Blitter (BB) */

   if ((flags & 0x4000U) != 0U){ sxincr = 0xFFFF0000U; } /* Pixel order swap (VMR) */
   else{                         sxincr = 0x00010000U; } /* Normal blit */
   counb &= 0xFFFF0000U;              /* Only cell boundaries */
   copadd = 0U;                       /* No count post-add */
   break;

  case 1U:                            /* Filler (FL) */

   break;

  case 2U:                            /* Scaled Blitter (SC) */

   ssplit = ssplit & srpart;          /* Limit to at most source partition size */
   srpart = srpart & (~ssplit);       /* Select only the Y component with srpart */
   break;

  default:                            /* Line (LI) */

   srpart = (~ssplit);                /* Select only the Y component with srpart */
   counr  = 1U;                       /* One row */
   break;

 }

 /* Prepare row count */

 counr = ((counr - 1U) & 0x1FFU) + 1U;

 /* Enter the row render loop! */

 while (counr != 0U){
  counr --;

  /* Load the fractional parts from the appropriate sources */

  sxfrac = sxfrap;
  syfrac = syfrap;
  dsfrac = dsfrap;
  count  = counb;

  /* Row specific calculations. "codst" is the count of destination bits to
  ** produce, "count" is transformed to the count of source pixels to process.
  ** In line mode only "codst" is used as a pixel count. */

  /* Init right shift to destination. Used in Scaled & Block Blitter, and in
  ** Filler to prepare the begin cell. */

  dshfr  = (dsfrac & 0xE000U) >> 11;

  /* Calculate count & codst */

  if (bmode == 3U){                   /* Line mode: Just pixel count */
   count >>= 16;
   codst = count;
  }else{                              /* Other modes: dest. bits & pixel count */
   count = (count & 0x00FFE000U) >> 13;
   codst = (count << 2) + dshfr;
  }

  /* Render */

  if (count != 0U){                   /* Only render if there is something to render */

   /* Calculate begin mask */

   bmems  = 0xFFFFFFFFU >> dshfr;
   bmems &= wrmask;

   /* Mode specific preparations */

   /* GCC Warning notes: GCC get confused about the usage of 'lpat', 'lflp'
   ** and 'sdata', reporting them uninitialized. These variables are however
   ** initialized if the initialization tree below is combined with the render
   ** tree in the loop. */

   switch (bmode){

    case 0U:                          /* Block Blitter (BB) */

     cyr   += 2U;                     /* 2 row transition cycles */
     break;

    case 1U:                          /* Filler (FL) */

     sdata  = sbase;
     cyr   += 4U;                     /* 4 row transition cycles */
     break;

    case 2U:                          /* Scaled Blitter (SC) */

     cyr   += count << 1;             /* 2 cycles for every pixel */
     cyr   += 8U;                     /* 8 row transition cycles */
     break;

    default:                          /* Line (LI) */

     lflp   = 0U;                     /* Odd / even flip for cycling the pattern */
     lpat   = sbase;                  /* Line pattern cycle */
     codst  = count;                  /* Destination counter <= pixel count */
     cyr   += count << 2;             /* 4 cycles for every pixel */
     break;

   }

   /* Calculate source to destination shift. Used in Scaled & Block Blitter */

   dshfl = (32U - dshfr) >> 1;        /* Could be 32, so split up the shift in two parts */

   /* Run the main rendering loop. Each iteration processes one PRAM cell,
   ** while the loop mostly relies on a proper branch predictor to lock on the
   ** appropriate path through it (most of the conditionals branch by the
   ** operation mode flags which are fixed through an operation). */

   while (1){

    /* Check end of blit condition and produce an end mask if so. Note that
    ** more than 0 pixels are remaining at this point which is relied upon for
    ** generating the shift. */

    if (bmode != 3U){                        /* It is not Line mode, so cell based render rules */
     if (codst < 32U){                       /* Fewer than 8 destination pixels remaining */
      bmems &= 0xFFFFFFFFU << (32U - codst); /* Generate an end mask */
      codst  = 0U;
     }else{
      codst -= 32U;
     }
    }

    /* Source preparation and pixel counting. This stage produces the initial
    ** source in sdata, which can be sent to the combining stage. */

    if ((bmode & 1U) == 0U){          /* Block Blitter or Scaled Blitter */

     /* The Scaled Blitter's implementation differs a little from the
     ** specification: it uses the Block Blitter's shifter so a smaller amount
     ** of pixels don't need to be fetched for the first cell when starting
     ** out of cell boundary. The result of this matches the exceptations of
     ** the specification. */

     /* Source generation stage */

     if (bmode == 0U){                /* Block blitter */

      /* Note that count might be tracked (8 subtracted on every pass) here,
      ** but it is not necessary for the blit to work. */

      sdata = pram[sxwhol | ((sxfrac >> 16) & srpart)];
      sxfrac += sxincr;

      if ((flags & 0x4000U) != 0U){            /* Pixel order swap (VMR) */
       sdata = ((sdata & 0xF0F0F0F0U) >> 4) | ((sdata & 0x0F0F0F0FU) << 4);
       sdata = ((sdata & 0xFF00FF00U) >> 8) | ((sdata & 0x00FF00FFU) << 8);
       sdata = (sdata >> 16) | (sdata << 16);  /* Mirror source (BSWAP would be good here) */
      }

     }else{                           /* Scaled blitter */

      /* Prepare the source data (sdata) from the appropriate number of
      ** individual source pixels. */

      sdata = 0U;

      i = 8U;
      while ((count != 0U) && (i != 0U)){
       count --;
       i     --;
       sdata |= ( ( pram[sxwhol |
                         ((syfrac >> 16) & srpart) |
                         ((sxfrac >> 16) & ssplit)] >>
                    (28U - ((sxfrac & 0xE000U) >> 11)) ) &  0xFU ) << (i << 2);
       sxfrac += sxincr;
       syfrac += syincr;
      }

     }

     /* Common post-process stage for BB & SC */

     t     = sdata;                   /* Align to destination */
     sdata = prevs | (sdata >> dshfr);
     prevs = (t << dshfl) << dshfl;

    }else if (bmode == 3U){           /* Line mode */

     /* Rotate source pattern & create begin / mid / end mask */

     lpat >>= (lflp << 2);
     sdata  = lpat & 0xFU;
     i      = ((sxfrac & 0xE000U) >> 11);
     bmems  =  0xFU << i;

     codst--;                         /* One pixel less to go */

     /* Re-expand source pattern (rotate!), finalize data */

     lpat   = (lpat & 0xFFFFU) | (lpat << 16);
     lflp  ^= 1U;                     /* Rotate at every second pixel */
     sdata  = sdata << i;
     bmems &= wrmask;

     /* Create destination fraction & increment pointers */

     dsfrac = (syfrac & (srpart << 16)) | (sxfrac & (ssplit << 16));
     sxfrac += sxincr;
     syfrac += syincr;

    }else{                            /* Filler - do nothing */
    }

    /* Destination combine stage begins, common for all modes. Create and
    ** apply the colorkey, perform reindexing as required, then blit it. The
    ** colorkey is always calculated: it usually does not affect accelerator
    ** cycle count, and in typical blits it is necessary anyway. */

    sdata = ((sdata >> rotr) & mandr) |  /* Apply pixel rotate & AND mask */
            ((sdata << rotl) & mandl);

    i   = dswhol | ((dsfrac >> 16) & dspart); /* Destination offset */
    u   = pram[i];                       /* Load current destination */
    t   = sdata ^ ckey;                  /* Prepare for colorkey calculation */
    sdata = sdata | mskor;               /* Apply OR mask after colorkey */

    t = (((t & 0x77777777U) + 0x77777777U) | t) & 0x88888888U;
    t = (t - (t >> 3)) + t;             /* Colorkey mask (0: background) */
    if ((flags & 0x1000U) != 0U){       /* Reindexing is required */
     sdata = rrpge_m_acco_rec(reb, sdata, u & reinm);
    }
    bmems &= t | (~(0U - (flags & 1U))); /* Add colorkey to write mask if enabled */

    /* Combine source over destination */

    bmems = ~bmems;                      /* Leave zero in mask where destination was dropped */
    pram[i] = ( (u & bmems ) | (sdata & (~bmems)) ) & 0xFFFFFFFFU;
    dsfrac += 0x10000U;

    /* Calculate combine cycle count. If bmems is zero, then may accelerate */

    bmems = (((bmems + 0x7FFFFFFFU) | bmems) >> 31) & 1U; /* becomes set if (low 32 bits) nonzero */
    cyr  += rrpge_m_acco_tc[cyf ^ bmems];   /* Clears "accelerated" if it was nonzero */

    /* The rendering loop ends when it ran out of destination to render. */

    if (codst == 0U){ break; }

    /* Update begin-mid-end mask: Just the write mask */

    bmems = wrmask;

   } /* End of line rendering loop */

  }  /* End of count != 0 if */

  /* Finalize the row: post-add, and rotate the pattern for FL mode. */

  dsfrap += dspadd;
  sxfrap += sxpadd;
  syfrap += sypadd;
  counb  += copadd;
  sbase   = (sbase >> 4) | (sbase << 28);

 } /* End of row rendering loop */

 return cyr;

}
