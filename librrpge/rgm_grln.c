/**
**  \file
**  \brief     Graphics line renderer
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.04
*/


#include "rgm_grln.h"


/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS


/* Local buffer for rendering lines. Only first 80 cells are used for output,
** however the full range is used for render. */
static uint32 rrpge_m_grln_buf[128U];



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
 auint  cky;                   /* Colorkey value extended */
 auint  csd;                   /* Current source read */
 auint  psd;                   /* Previous source in the alignment shift register */
 auint  dshr;                  /* Destination alignment shift */
 auint  dshl;
 auint  spos;                  /* Position in source during blit */
 auint  spms;                  /* Position mask in shift mode */
 auint  i;
 auint  t;
 auint  d;
 auint  m;

 /* Only draw line if within display area */

 if (rrpge_m_info.vln >= 400U){ return; }

 /* Read display list offset & entry size */

 i = rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x15U]; /* Display list definition */
 dlin = &(rrpge_m_edat->st.pram[((i & 0x000FU) << 16) & (PRAMS - 1U)]);
 doff = i & 0xFFC0U;
 dsiz = ((i >> 4) & 3U) + 2U;

 /* Read output width & begin positions */

 for (i = 0U; i < 2U; i++){
  opb[i] = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x4U + i];
  opw[i] = (opb[i] >> 8) & 0x7FU;
  opb[i] = opb[i] & 0x7FU;
 }

 /* Double scan check and line offset calculation */

 i = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x2U] & 0xFFU;
 if (rrpge_m_info.vln <= (i << 1)){
  dbl = 1U;
  cyr = 392U;
  doff = (doff + ((rrpge_m_info.vln >> 1) << dsiz)) & 0xFFFFU;
 }else{
  dbl = 0U;
  cyr = 192U;
  doff = (doff + ((rrpge_m_info.vln  - i) << dsiz)) & 0xFFFFU;
 }

 /* Rebase display list offset (no risk of crossing out of bank from now) */

 dlin += doff;

 /* Render line if possible */

 if ( (dbl == 0U) ||
      (rrpge_m_info.vln & 1U) == 0U){ /* Odd lines in double scan have no render */

  /* Convert display list size to cells */

  dsiz = (auint)(1U) << dsiz;

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
   csr  = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x8U + ((cmd >> 12) & 7U)] & 0xFFFFU; /* Current source to use */
   sbnk = &(rrpge_m_edat->st.pram[((csr & 0xF000U) << 4) & (PRAMS - 1U)]);
   soff = (cmd >> 16) & 0xFFFFU;
   if       ((cmd & 0x8000U) == 0U){ /* Render command inactive */
    scy = 0U;
   }else if ((csr & 0x0080U) != 0U){ /* Shift source */
    scy = opw[(cmd >> 14) & 1U] + 1U;
   }else{                            /* Positioned source (X expansion doubles width!) */
    scy = (((csr - 1U) & 0x7FU) + 1U) << ((cmd >> 11) & 1U);
   }
   cyr--;                         /* Cycle taken for display list entry fetch (cyr certain nonzero here) */
   if (scy > cyr){ scy = cyr; }
   cyr -= scy;                    /* Width to render & remaining cycles done */

   if ((cmd & 0x8000U) != 0U){    /* Render command not inactive */

    /* Calculate colorkey */

    cky = (csr >> 8) & 0xFU;
    cky |= cky << 4;
    cky |= cky << 8;
    cky |= cky << 16;

    /* Do the blit */

    if ((csr & 0x0080U) != 0U){   /* Shift source */

     /* Will shift to the left, based on low 3 bits of command. Initial source
     ** is always fetched. */

     dshl = (cmd & 0x7U) << 2;
     dshr = (32U - dshl) >> 1;    /* Could be 32, so split into two shifts */

     /* Source position (X expansion doubles width!) */

     spms = (1U << (csr & 7U)) - 1U;
     soff = soff & (~spms);       /* Source offset base must be masked by nonexpanded */
     if ((cmd & 0x0800U) != 0U){ spms = (spms << 1) | 1U; }
     spos = ((cmd >> 3) & 0x7FU) & spms;

     /* Fetch first source */

     if ((cmd & 0x0800U) == 0U){  /* Normal load */
      csd = sbnk[soff + spos];
     }else{                       /* X expanded load */
      csd = sbnk[soff + (spos >> 1)];
      csd = (csd >> ((spos & 1U) << 4)) & 0xFFFFU; /* Select high / low half */
      csd = ((csd & 0x000FU)      ) |
            ((csd & 0x00F0U) <<  4) |
            ((csd & 0x0F00U) <<  8) |
            ((csd & 0xF000U) << 12);
      csd = csd | (csd << 4);
     }
     spos = (spos + 1U) & spms;
     psd  = csd << dshl;

     /* Do the blitting loop */

     i    = opb[(cmd >> 14) & 1U];
     while (scy != 0U){

      if ((cmd & 0x0800U) == 0U){    /* Normal load */
       csd = sbnk[soff + spos];
      }else{                         /* X expanded load */
       csd = sbnk[soff + (spos >> 1)];
       csd = (csd >> ((spos & 1U) << 4)) & 0xFFFFU; /* Select high / low half */
       csd = ((csd & 0x000FU)      ) |
             ((csd & 0x00F0U) <<  4) |
             ((csd & 0x0F00U) <<  8) |
             ((csd & 0xF000U) << 12);
       csd = csd | (csd << 4);
      }

      spos = (spos + 1U) & spms;
      psd |= (csd >> dshr) >> dshr;  /* Now 'psd' contains the destination aligned source */

      d    = rrpge_m_grln_buf[i];

      /* Create mask from colorkey */

      m    = psd ^ cky;              /* Prepare for colorkey */
      m    = (((m & 0x77777777U) + 0x77777777U) | m); /* Colorkey mask on the highest bit of pixel */
      m   &= 0x88888888U;            /* Mask out lower pixel bits */
      m    = (m - (m >> 3)) + t;     /* Expand to lower pixels */

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

     m    = 0xFFFFFFFFU >> dshr;

     /* Do the blitting loop */

     spos = 0U;
     i    = (cmd >> 3) & 0x7FU;
     psd  = 0U;
     while (1){

      /* Determine if end cell, if so, do an end mask, otherwise fetch source */

      if (scy == 0U){                /* End cell */
       csd  = 0U;
       m   &= (0xFFFFFFFFU << dshl) << dshl; /* End mask */
      }else{                         /* Not an end cell */
       if ((cmd & 0x0800U) == 0U){   /* Normal load */
        csd = sbnk[(soff + spos) & 0xFFFFU];
       }else{                        /* X expanded load */
        csd = sbnk[(soff + (spos >> 1)) & 0xFFFFU];
        csd = (csd >> ((spos & 1U) << 4)) & 0xFFFFU; /* Select high / low half */
        csd = ((csd & 0x000FU)      ) |
              ((csd & 0x00F0U) <<  4) |
              ((csd & 0x0F00U) <<  8) |
              ((csd & 0xF000U) << 12);
        csd = csd | (csd << 4);
       }
       spos ++;
      }

      /* Prepare source in psd */

      psd |= csd >> dshr;

      /* Load destination for masks */

      d    = rrpge_m_grln_buf[i];

      /* Create masks */

      t    = psd ^ cky;              /* Prepare for colorkey */
      t    = (((t & 0x77777777U) + 0x77777777U) | t); /* Colorkey mask on the highest bit of pixel */
      t   &= 0x88888888U;            /* Mask out lower pixel bits */
      t    = (t - (t >> 3)) + t;     /* Expand to lower pixels */
      m   &= t;                      /* Add to combined mask */

      /* Combine destination */

      rrpge_m_grln_buf[i] = ((psd & m) | (d & (~m))) & 0xFFFFFFFFU;
      i    = (i + 1U) & 0x7FU;

      /* Done, finalize */

      if (scy == 0U){ break; }    /* End of render */
      m    = 0xFFFFFFFFU;         /* Reinit mask */
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
