/**
**  \file
**  \brief     Graphics line renderer
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#include "rgm_grln.h"


/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS


/* Local buffer for rendering lines. Only first 80 cells are used for output,
** however the full range is used for render. */
static uint32 rrpge_m_grln_buf[128U];


/* Mask / colorkey values */
static const uint8 rrpge_m_grln_msk[8U] = {
 0x00U, 0xFFU, 0x0FU, 0x3FU, 0x03U, 0x0CU, 0x30U, 0xC0U};



/* Renders current graphics line. Also performs callback to host. */
void rrpge_m_grln(void)
{
 uint32 const* dlin;           /* Display list line */
 uint32 const* sbnk;           /* Source PRAM bank */
 auint  soff;                  /* Source base offset within PRAM bank */
 auint  doff;                  /* Offset within display list */
 auint  dsiz;                  /* Size of display list line */
 auint  opw[2];                /* Output width */
 auint  opb[2];                /* Output begin */
 auint  dbl;                   /* Double scan flag */
 auint  cyr;                   /* Cycles remaining */
 auint  cmd;                   /* Current display list command */
 auint  dmd;                   /* Current display mode (4 bit: 0; 8 bit: 1) */
 auint  csr;                   /* Current source */
 auint  scy;                   /* Number of output cycles to produce */
 auint  msk;                   /* Mask / colorkey value extended */
 auint  msb;                   /* Mask base value (0x...FFFFFFFF or msk value) */
 auint  csd;                   /* Current source read */
 auint  psd;                   /* Previous source in the alignment shift register */
 auint  dshr;                  /* Destination alignment shift */
 auint  dshl;
 auint  spos;                  /* Position in source during blit */
 auint  spms;                  /* Position mask in shift mode */
 auint  i;
 auint  t;
 auint  u;
 auint  d;
 auint  m;
 auint  bs;
 auint  bm;
 auint  bc;
 auint  dcm;
 auint  dpm;

 /* Only draw line if within display area */

 if (rrpge_m_info.vln >= 400U){ return; }

 /* Read display list offset & entry size */

 i = rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x15U] & ((PRAMS - 1U) >> 9); /* Display list definition */
 i = i & (~(((auint)(4U) << (i & 3U)) - 4U));
 dlin = &(rrpge_m_edat->st.pram[(i & (~(auint)(3U))) << 9]);
 dsiz = (i & 3U) + 2U;
 dlin += rrpge_m_info.vln << dsiz; /* Position on the current line in display list */

 /* Read output width & begin positions */

 for (i = 0U; i < 2U; i++){
  opb[i] = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 4U + i];
  opw[i] = (opb[i] >> 8) & 0x7FU;
  opb[i] = opb[i] & 0x7FU;
 }

 /* Double scan: Bus access cycle count available & display list size adjust */

 dbl  = (rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x7U] >> 13) & 1U;
 cyr  = 192U;
 if (dbl != 0U){ /* Double scan */
  dsiz ++;
  cyr = 392U;
 }

 if ( (dbl == 0U) ||
      (rrpge_m_info.vln & 1U) == 0U){ /* Odd lines in double scan have no render */

  /* Convert display list size to cells */

  dsiz = (auint)(1U) << dsiz;

  /* Prepare shift count & mask for expanding 4 / 8 bit masks and calculating
  ** colorkey */

  dmd = (rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x7U] >> 12) & 1U; /* 0: 4 bit; 1: 8 bit mode */
  if (dmd != 0U){                 /* 8 bit mode */
   bc = 0x7F7F7F7FU;              /* Colorkey calculation mask */
   bm = 0x80808080U;              /* Mask to select the effective mask bit */
   bs = 7U;                       /* Shift count in the expansion */
  }else{                          /* 4 bit mode */
   bc = 0x77777777U;              /* Colorkey calculation mask */
   bm = 0x88888888U;              /* Mask to select the effective mask bit */
   bs = 3U;                       /* Shift count in the expansion */
  }

  /* Reset line buffer */

  t    = dlin[0] & 0xFFFFFFFFU;
  for (i = 0U; i < 80U; i++){
   rrpge_m_grln_buf[i] = t;
  }
  doff = 1U;

  /* Process display list. There are two exit conditions: either draining the
  ** display list, or exhausting the cycle budget. */

  while (1){

   cmd  = dlin[doff] & 0xFFFFFFFFU;  /* Current command */
   doff ++;
   csr  = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x8U + ((cmd >> 28) & 7U)] & 0xFFFFU; /* Current source to use */
   sbnk = &(rrpge_m_edat->st.pram[((csr & 0x0F00U) << 8) & (PRAMS - 1U)]);
   soff = (csr & 0xF000U) | ((((cmd >> 16) & 0xFFFU) << ((csr >> 5) & 7U)) & 0xFFFFU);
   if       ((cmd & 0xBC00U) == 0U){ /* Render command inactive */
    scy = 0U;
   }else if ((csr & 0x0010U) != 0U){ /* Shift source */
    scy = opw[(cmd >> 30) & 1U];
   }else{                            /* Positioned source */
    scy = (((csr & 0xFU) << 1) + 1U) << ((csr >> 5) & 7U);
   }
   cyr--;                         /* Cycle taken for display list entry fetch (cyr certain nonzero here) */
   if (scy > cyr){ scy = cyr; }
   cyr -= scy;                    /* Width to render & remaining cycles done */

   if ((cmd & 0xBC00U) != 0U){    /* Render command not inactive */

    /* Calculate mask / colorkey */

    i    = (cmd >> 10) & 0xFU;    /* Colorkey / Mask selector */
    if (i > 8U){                  /* Load user masks */
     msk = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + ((i >> 1) & 3U)] >> (((i & 1U) ^ 1U) << 3);
     msk = msk & 0xFFU;
    }else{                        /* Load one of the pre-defined masks */
     msk = rrpge_m_grln_msk[i];
    }
    if (dmd == 0U){               /* 4 bit mode */
     msk &= 0x0FU;
     msk |= msk << 4;
    }
    msk |= msk << 8;
    msk |= msk << 16;

    msb = msk | ((auint)(0U) - ((cmd >> 15) & 1U)); /* Mask base is either 0x...FFFFFFFF or the mask */

    /* Precalculate colorkey & priority selector mask drops (0x...FFFFFFFFU if
    ** the mask is to be dropped, 0U otherwise). */

    dcm = (0U - ( (cmd >> 14) & 1U)      ); /* Drop colorkey unless enabled */
    dpm = (0U - (((cmd >> 31) & 1U) ^ 1U)); /* Drop priority selector unless enabled */

    /* Drop lowest bit (position / shift) of command in 8 bit mode */

    cmd &= ~dmd;

    /* Do the blit */

    if ((csr & 0x0010U) != 0U){   /* Shift source */

     /* Will shift to the left, based on low 3 bits of command. Initial source
     ** is always fetched. */

     dshl = (cmd & 0x7U) << 2;
     dshr = (32U - dshl) >> 1;    /* Could be 32, so split into two shifts */

     /* Source position */

     spms = (1U << ((csr >> 5) & 7U)) - 1U;
     spos = ((cmd >> 3) & 0x7FU) & spms;

     /* Fetch first source */

     csd  = sbnk[soff + spos];
     spos = (spos + 1U) & spms;
     psd  = csd << dshl;

     /* Do the blitting loop */

     i    = opb[(cmd >> 30) & 1U];
     while (scy != 0U){

      csd  = sbnk[soff + spos];
      spos = (spos + 1U) & spms;
      psd  |= (csd >> dshr) >> dshr; /* Now 'psd' contains the destination aligned source */

      d    = rrpge_m_grln_buf[i];

      /* Create masks */

      m    = msb;                    /* Combined mask */

      t    = psd ^ msk;              /* Prepare for colorkey */
      t    = (((t & bc) + bc) | t);  /* Colorkey mask on the highest bit of pixel */
      u    = ~(d << (dmd << 1));     /* Priority mask on the highest bit of pixel */
      t   |= dcm;                    /* Drop colorkey unless enabled */
      u   |= dpm;                    /* Drop priority selector unless enabled */
      t   &= u;                      /* Combine the two masks */
      t   &= bm;                     /* Mask out lower pixel bits */
      t    = (t - (t >> bs)) + t;    /* Expand to lower pixels */

      m   &= t;                      /* Add to combined mask */

      /* Combine destination */

      rrpge_m_grln_buf[i] = ((psd & m) | (d & (~m))) & 0xFFFFFFFFU;
      i    = (i + 1U) & 0x7FU;

      /* Done, finalize */

      psd  = csd << dshl;
      scy --;

     }

    }else{                        /* Position source */

     /* Will shift to the right, based on low 3 bits of command. Here one more
     ** destination needs to be generated than sources, scy represents the
     ** count of source fetches required. */

     dshr = (cmd & 0x7U) << 2;
     dshl = (32U - dshr) >> 1;    /* Could be 32, so split into two shifts */

     /* Initial (begin) mask */

     m    = msb >> dshr;

     /* Do the blitting loop */

     spos = 0U;
     i    = (cmd >> 3) & 0x7FU;
     psd  = 0U;
     while (1){

      /* Determine if end cell, if so, do an end mask, otherwise fetch source */

      if (scy == 0U){ /* End cell */
       csd  = 0U;
       m   &= (0xFFFFFFFFU << dshl) << dshl; /* End mask */
      }else{          /* Not an end cell */
       csd  = sbnk[(soff + spos) & 0xFFFFU];
       spos ++;
      }

      /* Prepare source in psd */

      psd |= csd >> dshr;

      /* Load destination for masks */

      d    = rrpge_m_grln_buf[i];

      /* Create masks */

      t    = psd ^ msk;              /* Prepare for colorkey */
      t    = (((t & bc) + bc) | t);  /* Colorkey mask on the highest bit of pixel */
      u    = ~(d << (dmd << 1));     /* Priority mask on the highest bit of pixel */
      t   |= dcm;                    /* Drop colorkey unless enabled */
      u   |= dpm;                    /* Drop priority selector unless enabled */
      t   &= u;                      /* Combine the two masks */
      t   &= bm;                     /* Mask out lower pixel bits */
      t    = (t - (t >> bs)) + t;    /* Expand to lower pixels */

      m   &= t;                      /* Add to combined mask */

      /* Combine destination */

      rrpge_m_grln_buf[i] = ((psd & m) | (d & (~m))) & 0xFFFFFFFFU;
      i    = (i + 1U) & 0x7FU;

      /* Done, finalize */

      if (scy == 0U){ break; }    /* End of render */
      m    = msb;                 /* Reinit mask */
      psd  = (csd << dshl) << dshl;
      scy --;

     }

    }

   }

   /* The command was processed, may go on for next command if possible */

   if (cyr  ==   0U){ break; }    /* Cycle budget exhausted */
   if (doff == dsiz){ break; }    /* Display list completed */

  }

 }

 /* Display list completed, the line is ready to be rendered */

 rrpge_m_edat->cb_lin(rrpge_m_edat, rrpge_m_info.vln, &rrpge_m_grln_buf[0]);
}
