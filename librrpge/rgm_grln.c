/**
**  \file
**  \brief     Graphics line renderer
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.10.02
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
 auint  csr;                   /* Current source */
 auint  scy;                   /* Number of output cycles to produce */
 uint32 msk;                   /* Mask / colorkey value extended */
 uint32 msb;                   /* Mask base value (0xFFFFFFFF or msk value) */
 uint32 csd;                   /* Current source read */
 uint32 psd;                   /* Previous source in the alignment shift register */
 auint  dshr;                  /* Destination alignment shift */
 auint  dshl;
 auint  spos;                  /* Position in source during blit */
 auint  spms;                  /* Position mask in shift mode */
 auint  i;
 uint32 t32;
 uint32 u32;
 uint32 d32;
 uint32 m32;
 auint  bs;
 auint  bm;
 auint  bc;
 uint32 dcm;
 uint32 dpm;

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

 dbl  = (rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x12U] >> 1) & 1U;
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

  if ((rrpge_m_info.vbm & 0x80U) != 0U){ /* 8 bit mode */
   bc = 0x7F7F7F7FU;              /* Colorkey calculation mask */
   bm = 0x80808080U;              /* Mask to select the effective mask bit */
   bs = 7U;                       /* Shift count in the expansion */
  }else{                          /* 4 bit mode */
   bc = 0x77777777U;              /* Colorkey calculation mask */
   bm = 0x88888888U;              /* Mask to select the effective mask bit */
   bs = 3U;                       /* Shift count in the expansion */
  }

  /* Reset line buffer */

  t32 = dlin[0];
  for (i = 0U; i < 80U; i++){
   rrpge_m_grln_buf[i] = t32;
  }
  doff = 1U;

  /* Process display list. There are two exit conditions: either draining the
  ** display list, or exhausting the cycle budget. */

  while (1){

   cmd  = dlin[doff];             /* Current command */
   doff ++;
   csr  = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x8U + ((cmd >> 28) & 7U)]; /* Current source to use */
   sbnk = &(rrpge_m_edat->st.pram[((csr & 0x0F00U) << 8) & (PRAMS - 1U)]);
   soff = (csr & 0xF000U) | ((((cmd >> 16) & 0xFFFU) << ((csr >> 5) & 7U)) & 0xFFFFU);
   if       ((cmd & 0xBC00U) == 0U){ /* Render command inactive */
    scy = 0U;
   }else if ((csr & 0x0080U) != 0U){ /* Shift source */
    scy = opw[(cmd >> 30) & 1U];
   }else{                            /* Positioned source */
    scy = (((csr & 0xFU) << 1) + 1U) << ((csr >> 5) & 7U);
   }
   cyr--;                         /* Cycle taken for display list entry fetch (cyr certain nonzero here) */
   if (scy > cyr){ scy = cyr; }
   cyr -= scy;                    /* Width to render & remaining cycles done */

   if ((cmd & 0xBC00U) != 0U){    /* Render command not inactive */

    /* Source line select in cmd is no longer used. Substitute with the
    ** current graphics mode for accessing the latter faster later on */

    cmd  = (cmd & 0xF000FFFFU) | ((rrpge_m_info.vbm & 0x80U) << 9); /* bit 16 set in 8bit mode */

    /* Calculate mask / colorkey */

    i    = (cmd >> 10) & 0xFU;    /* Colorkey / Mask selector */
    if (i > 8U){                  /* Load user masks */
     msk = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + ((i >> 1) & 3U)] >> (((i & 1U) ^ 1U) << 3);
     msk = msk & 0xFFU;
    }else{                        /* Load one of the pre-defined masks */
     msk = rrpge_m_grln_msk[i];
    }
    if ((cmd & 0x10000) == 0U){   /* 4 bit mode */
     msk &= 0x0FU;
     msk |= msk << 4;
    }
    msk |= msk << 8;
    msk |= msk << 16;

    msb = msk | ((uint32)(0U) - ((cmd >> 15) & 1U)); /* Mask base is either 0xFFFFFFFF or the mask */

    /* Precalculate colorkey & priority selector mask drops (0xFFFFFFFFU if
    ** the mask is to be dropped, 0x00000000U otherwise). */

    dcm = ((uint32)(0U) - ((cmd >> 14) & 1U)); /* Drop colorkey unless enabled */
    dpm = (0xFFFFFFFFU  + ((cmd >> 31) & 1U)); /* Drop priority selector unless enabled */

    /* Drop lowest bit (position / shift) of command in 8 bit mode */

    cmd &= ~((cmd >> 16) & 1U);

    /* Do the blit */

    if ((csr & 0x0080U) != 0U){   /* Shift source */

     /* Will shift to the left, based on low 3 bits of command. Initial source
     ** is always fetched. */

     dshl = (cmd & 0x7U) << 2;
     dshr = (32U - dshl) >> 1;    /* Could be 32, so split into two shifts */

     /* Source position */

     spms = (1U << ((csr >> 8) & 7U)) - 1U;
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

      d32  = rrpge_m_grln_buf[i];

      /* Create masks */

      m32  = msb;                    /* Combined mask */

      t32  = psd ^ msk;                    /* Prepare for colorkey */
      t32  = (((t32 & bc) + bc) | t32);    /* Colorkey mask on the highest bit of pixel */
      u32  = ~(d32 << ((cmd >> 15) & 2U)); /* Priority mask on the highest bit of pixel */
      t32 |= dcm;                          /* Drop colorkey unless enabled */
      u32 |= dpm;                          /* Drop priority selector unless enabled */
      t32 &= u32;                          /* Combine the two masks */
      t32 &= bm;                           /* Mask out lower pixel bits */
      t32  = (t32 - (t32 >> bs)) + t32;    /* Expand to lower pixels */

      m32 &= t32;                    /* Add to combined mask */

      /* Combine destination */

      rrpge_m_grln_buf[i] = (psd & m32) | (d32 & (~m32));
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

     m32  = msb >> dshr;

     /* Do the blitting loop */

     spos = 0U;
     i    = (cmd >> 3) & 0x7FU;
     psd  = 0U;
     while (1){

      /* Determine if end cell, if so, do an end mask, otherwise fetch source */

      if (scy == 0U){ /* End cell */
       csd  = 0U;
       m32 &= (0xFFFFFFFFU << dshl) << dshl; /* End mask */
      }else{          /* Not an end cell */
       csd  = sbnk[(soff + spos) & 0xFFFFU];
       spos ++;
      }

      /* Prepare source in psd */

      psd |= csd >> dshr;

      /* Load destination for masks */

      d32  = rrpge_m_grln_buf[i];

      /* Create masks */

      t32  = psd ^ msk;                    /* Prepare for colorkey */
      t32  = (((t32 & bc) + bc) | t32);    /* Colorkey mask on the highest bit of pixel */
      u32  = ~(d32 << ((cmd >> 15) & 2U)); /* Priority mask on the highest bit of pixel */
      t32 |= dcm;                          /* Drop colorkey unless enabled */
      u32 |= dpm;                          /* Drop priority selector unless enabled */
      t32 &= u32;                          /* Combine the two masks */
      t32 &= bm;                           /* Mask out lower pixel bits */
      t32  = (t32 - (t32 >> bs)) + t32;    /* Expand to lower pixels */

      m32 &= t32;                    /* Add to combined mask */

      /* Combine destination */

      rrpge_m_grln_buf[i] = (psd & m32) | (d32 & (~m32));
      i    = (i + 1U) & 0x7FU;

      /* Done, finalize */

      if (scy == 0U){ break; }    /* End of render */
      m32  = msb;                 /* Reinit mask */
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
