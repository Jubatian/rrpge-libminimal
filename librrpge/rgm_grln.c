/**
**  \file
**  \brief     Graphics line renderer
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.12
*/


#include "rgm_grln.h"


/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS


/* 4 bit to 32 bit expansion table */
static const uint32 rrpge_m_grln_ex32[16] = {
 0x00000000U, 0x11111111U, 0x22222222U, 0x33333333U,
 0x44444444U, 0x55555555U, 0x66666666U, 0x77777777U,
 0x88888888U, 0x99999999U, 0xAAAAAAAAU, 0xBBBBBBBBU,
 0xCCCCCCCCU, 0xDDDDDDDDU, 0xEEEEEEEEU, 0xFFFFFFFFU
};


/* !!! These should be moved into the emulator object !!! */

/* Local buffer for rendering lines. Only first 80 cells are used for output,
** however the full range is used for render. The low half is used to store
** the low 3 bits of each pixel, the high half is for the high 3 bits, these
** are combined together once the line is rendered. */
static uint32 rrpge_m_grln_bufl[128];
static uint32 rrpge_m_grln_bufh[128];
static uint8  rrpge_m_grln_buf [640];

/* Clipping buffers */
static uint32 rrpge_m_grln_cbuf[3][128];
static auint  rrpge_m_grln_cini[3] = {0x10000U, 0x10000U, 0x10000U};



/* Updates a clipping buffer as needed. "tg" is the target state. */
void rrpge_m_grln_cbup(auint id, auint tg)
{
 auint i;
 auint beg;
 auint owd;
 auint end;

 tg = tg & 0x7F7FU;

 if (tg == rrpge_m_grln_cini[id]){ return; }

 beg = tg & 0x7FU;
 owd = (tg >> 8) & 0x7FU;
 end = (beg + owd) & 0x7FU;

 for (i = 0U; i < 128U; i++){
  rrpge_m_grln_cbuf[id][i] = 0x00000000U;
 }
 for (i = beg; i != end; i = (i + 1U) & 0x7FU){
  rrpge_m_grln_cbuf[id][i] = 0xFFFFFFFFU;
 }

 rrpge_m_grln_cini[id] = tg;
}



/* Renders current graphics line. Also performs callback to host. */
void rrpge_m_grln(void)
{
 uint32 const* dlin;           /* Display list line */
 uint32 const* sbnk;           /* Source PRAM bank */
 uint32 const* clpb;           /* Clipping buffer */
 auint  soff;                  /* Source base offset within PRAM bank */
 auint  doff;                  /* Offset within display list */
 auint  dsiz;                  /* Size of display list line */
 auint  opws[8];               /* Output widths */
 auint  opbs[8];               /* Output begins */
 auint  clps;                  /* Clipping settings */
 auint  opw;                   /* Output width for current render (shift source) */
 auint  opb;                   /* Output begin for current render (shift source) */
 auint  dbl;                   /* Double scan flag */
 auint  cyr;                   /* Cycles remaining */
 auint  cmd;                   /* Current display list command */
 auint  ssl;                   /* Source select from command */
 auint  csr;                   /* Current source */
 auint  scy;                   /* Number of output cycles to produce */
 auint  cky;                   /* Colorkey value extended */
 auint  csd;                   /* Current source read */
 auint  psd;                   /* Previous source in the alignment shift register */
 auint  dshr;                  /* Destination alignment shift */
 auint  dshl;
 auint  spos;                  /* Position in source during blit */
 auint  spms;                  /* Position mask in shift mode */
 auint  plh;                   /* High half-palette select expanded */
 auint  pll;                   /* Low half-palette select expanded */
 auint  i;
 auint  t;
 auint  h;
 auint  m;

 /* Only draw line if within display area */

 if (rrpge_m_info.vln >= 400U){ return; }

 /* Read display list offset & entry size */

 i = rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x15U]; /* Display list definition */
 dlin = &(rrpge_m_edat->st.pram[((i & 0xF000U) << 4) & (PRAMS - 1U)]);
 doff = (i & 0x0FFCU) << 4;
 dsiz = (i & 3U) + 2U;

 /* Read output width & begin positions and clipping settings */

 clps = 0U;
 rrpge_m_grln_cbup(2U, 0x5000U);

 for (i = 0U; i < 2U; i++){

  t = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x4U + i];
  rrpge_m_grln_cbup(i, t);

  m = (t >> 8) & 0x7FU;
  opws[(i << 2) + 0U] = m;
  opws[(i << 2) + 1U] = m;
  opws[(i << 2) + 2U] = m;
  opws[(i << 2) + 3U] = m;

  m = t & 0x7FU;
  opbs[(i << 2) + 0U] = m;
  opbs[(i << 2) + 1U] = m;
  opbs[(i << 2) + 2U] = m;
  opbs[(i << 2) + 3U] = m;

  m = ((t & 0x8000U) >> 12) |
      ((t & 0x8000U) >> 13) |
      ((t & 0x0080U) >>  6) |
      ((t & 0x0080U) >>  7);
  clps |= m << (i << 2);

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

  t    = dlin[0];
  h    = dlin[0] & 0x88888888U;
  h    = h - (h >> 3);
  plh  = (rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x2U] >> 12) & 0x7U;
  pll  = (rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x2U] >>  8) & 0x7U;
  plh  = rrpge_m_grln_ex32[plh];
  pll  = rrpge_m_grln_ex32[pll];
  h    = (plh & h) | (pll & (~h));
  for (i = 0U; i < 80U; i++){
   rrpge_m_grln_bufl[i] = t;
   rrpge_m_grln_bufh[i] = h;
  }
  doff = 1U;

  /* Process display list. There are two exit conditions: either draining the
  ** display list, or exhausting the cycle budget. */

  while (1){

   cmd  = dlin[doff] & 0xFFFFFFFFU;  /* Current command */
   doff ++;
   ssl  = (cmd >> 13) & 7U;          /* Source select */

   csr  = rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x8U + ssl] & 0xFFFFU; /* Current source to use */
   csr |= ( rrpge_m_edat->st.stat[RRPGE_STA_UPA_G + 0x0U + (ssl >> 2)] <<
            (((ssl & 3U) << 2) + 16U) ) & 0xF0000000U; /* Colorkey */
   sbnk = &(rrpge_m_edat->st.pram[((csr & 0xF000U) << 4) & (PRAMS - 1U)]);
   soff = (cmd >> 16) & 0xFFFFU;
   opw  = opws[ssl];
   opb  = opbs[ssl];
   if (((clps >> ssl) & 1U) != 0U){  /* Clipping requested */
    clpb = &(rrpge_m_grln_cbuf[ssl >> 2][0]);
   }else{                            /* No clipping, use permissive clipping buffer */
    clpb = &(rrpge_m_grln_cbuf[2][0]);
   }

   if       ((cmd & 0x1C00U) == 0U){ /* Render command inactive */
    scy = 0U;
   }else if ((csr & 0x0080U) != 0U){ /* Shift source */
    scy = opw + 1U;
   }else{                            /* Positioned source (X expansion doubles width!) */
    scy = (((csr - 1U) & 0x7FU) + 1U) << ((csr >> 11) & 1U);
   }
   cyr--;                         /* Cycle taken for display list entry fetch (cyr certain nonzero here) */
   if (scy > cyr){ scy = cyr; }
   cyr -= scy;                    /* Width to render & remaining cycles done */

   if ((cmd & 0x1C00U) != 0U){    /* Render command not inactive */

    /* Calculate colorkey */

    cky = (csr >> 28) & 0xFU;
    cky = rrpge_m_grln_ex32[cky];

    /* Calculate half-palettes */

    plh = (cmd >> 10) & 0x7U;
    pll = (csr >>  8) & 0x7U;
    plh  = rrpge_m_grln_ex32[plh];
    pll  = rrpge_m_grln_ex32[pll];

    /* Do the blit */

    if ((csr & 0x0080U) != 0U){   /* Shift source */

     /* Will shift to the left, based on low 3 bits of command. Initial source
     ** is always fetched. */

     dshl = (cmd & 0x7U) << 2;
     dshr = (32U - dshl) >> 1;    /* Could be 32, so split into two shifts */

     /* Source position (X expansion doubles width!) */

     spms = (1U << (csr & 7U)) - 1U;
     soff = soff & (~spms);       /* Source offset base must be masked by nonexpanded */
     if ((csr & 0x0800U) != 0U){ spms = (spms << 1) | 1U; }
     spos = ((cmd >> 3) & 0x7FU) & spms;

     /* Fetch first source */

     if ((csr & 0x0800U) == 0U){  /* Normal load */
      csd = sbnk[soff + spos];
     }else{                       /* X expanded load */
      csd = sbnk[soff + (spos >> 1)];
      csd = (csd >> (((spos & 1U) ^ 1U) << 4)) & 0xFFFFU; /* Select high / low half */
      csd = csd | (csd << 8);
      csd = ((csd & 0x00FF00FFU) << 4) |
            ((csd & 0x00F000F0U) << 8) |
            ((csd & 0x000F000FU));
     }
     spos = (spos + 1U) & spms;
     psd  = csd << dshl;
     scy  --;                     /* (Note: scy was at least one at this point) */

     /* Do the blitting loop */

     i    = opb;
     while (scy != 0U){

      if ((csr & 0x0800U) == 0U){ /* Normal load */
       csd = sbnk[soff + spos];
      }else{                      /* X expanded load */
       csd = sbnk[soff + (spos >> 1)];
       csd = (csd >> (((spos & 1U) ^ 1U) << 4)) & 0xFFFFU; /* Select high / low half */
       csd = csd | (csd << 8);
       csd = ((csd & 0x00FF00FFU) << 4) |
             ((csd & 0x00F000F0U) << 8) |
             ((csd & 0x000F000FU));
      }

      spos = (spos + 1U) & spms;
      psd |= (csd >> dshr) >> dshr;  /* Now 'psd' contains the destination aligned source */

      /* Create mask from colorkey */

      m    = psd ^ cky;              /* Prepare for colorkey */
      m    = (((m & 0x77777777U) + 0x77777777U) | m); /* Colorkey mask on the highest bit of pixel */
      m   &= 0x88888888U;            /* Mask out lower pixel bits */
      m    = (m - (m >> 3)) + m;     /* Expand to lower pixels */

      /* Calculate low and high halves */

      t    = psd;
      h    = psd & 0x88888888U;
      h    = h - (h >> 3);
      h    = (plh & h) | (pll & (~h));

      /* Combine destination */

      rrpge_m_grln_bufl[i] = (t & m) | (rrpge_m_grln_bufl[i] & (~m));
      rrpge_m_grln_bufh[i] = (h & m) | (rrpge_m_grln_bufh[i] & (~m));
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
       if ((csr & 0x0800U) == 0U){   /* Normal load */
        csd = sbnk[(soff + spos) & 0xFFFFU];
       }else{                        /* X expanded load */
        csd = sbnk[(soff + (spos >> 1)) & 0xFFFFU];
        csd = (csd >> (((spos & 1U) ^ 1U) << 4)) & 0xFFFFU; /* Select high / low half */
        csd = csd | (csd << 8);
        csd = ((csd & 0x00FF00FFU) << 4) |
              ((csd & 0x00F000F0U) << 8) |
              ((csd & 0x000F000FU));
       }
       spos ++;
      }

      /* Prepare source in psd */

      psd |= csd >> dshr;

      /* Create masks */

      t    = psd ^ cky;              /* Prepare for colorkey */
      t    = (((t & 0x77777777U) + 0x77777777U) | t); /* Colorkey mask on the highest bit of pixel */
      t   &= 0x88888888U;            /* Mask out lower pixel bits */
      t    = (t - (t >> 3)) + t;     /* Expand to lower pixels */
      m   &= t;                      /* Add to combined mask */
      m   &= clpb[i];                /* Add clipping mask to combined mask */

      /* Calculate low and high halves */

      t    = psd;
      h    = psd & 0x88888888U;
      h    = h - (h >> 3);
      h    = (plh & h) | (pll & (~h));

      /* Combine destination */

      rrpge_m_grln_bufl[i] = (t & m) | (rrpge_m_grln_bufl[i] & (~m));
      rrpge_m_grln_bufh[i] = (h & m) | (rrpge_m_grln_bufh[i] & (~m));
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

  /* Line rendered in bufl & bufh, now combine the result to produce 6 bit
  ** pixels. */

  for (i = 0U; i < 80U; i++){
   t = rrpge_m_grln_bufl[i];
   h = rrpge_m_grln_bufh[i];
   m = t;
   t = ((t & 0x07070707U)     ) | ((h & 0x07070707U) << 3);
   h = ((m & 0x70707070U) >> 4) | ((h & 0x70707070U) >> 1);
   m = i << 3;
   rrpge_m_grln_buf[m + 0U] = (uint8)((h >> 24)        );
   rrpge_m_grln_buf[m + 1U] = (uint8)((t >> 24)        );
   rrpge_m_grln_buf[m + 2U] = (uint8)((h >> 16) & 0xFFU);
   rrpge_m_grln_buf[m + 3U] = (uint8)((t >> 16) & 0xFFU);
   rrpge_m_grln_buf[m + 4U] = (uint8)((h >>  8) & 0xFFU);
   rrpge_m_grln_buf[m + 5U] = (uint8)((t >>  8) & 0xFFU);
   rrpge_m_grln_buf[m + 6U] = (uint8)((h      ) & 0xFFU);
   rrpge_m_grln_buf[m + 7U] = (uint8)((t      ) & 0xFFU);
  }

 }

 /* Display list completed, the line is ready to be rendered */

 rrpge_m_edat->cb_lin(rrpge_m_edat, rrpge_m_info.vln, &rrpge_m_grln_buf[0]);
}
