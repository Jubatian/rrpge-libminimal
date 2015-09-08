/**
**  \file
**  \brief     Graphics line renderer
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.08
*/


#include "rgm_vidl.h"


/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS


/* 4 bit to 32 bit expansion table */
static const uint32 rrpge_m_vidl_ex32[16] = {
 0x00000000U, 0x11111111U, 0x22222222U, 0x33333333U,
 0x44444444U, 0x55555555U, 0x66666666U, 0x77777777U,
 0x88888888U, 0x99999999U, 0xAAAAAAAAU, 0xBBBBBBBBU,
 0xCCCCCCCCU, 0xDDDDDDDDU, 0xEEEEEEEEU, 0xFFFFFFFFU
};



/* Updates a clipping buffer as needed. "tg" is the target state.
** Note that the clipping buffers are in cell pairs (both cell having the same
** clipping mask). */
static void rrpge_m_vidl_cbup(rrpge_object_t* hnd, auint id, auint tg)
{
 auint i;
 auint beg;
 auint owd;
 auint end;

 tg = tg & 0x3F3FU;

 if (tg == hnd->vid.cini[id]){ return; }

 beg = tg & 0x3FU;
 owd = (tg >> 8) & 0x3FU;
 end = (beg + owd) & 0x3FU;

 for (i = 0U; i < 64U; i++){
  hnd->vid.cbuf[id][i] = 0x00000000U;
 }
 for (i = beg; i != end; i = (i + 1U) & 0x3FU){
  hnd->vid.cbuf[id][i] = 0xFFFFFFFFU;
 }

 hnd->vid.cini[id] = tg;
}



/* Renders current graphics line. Also performs callback to host. */
void rrpge_m_vidl(rrpge_object_t* hnd)
{
 uint32 bufl[128];             /* Render buffer, low half */
 uint32 bufh[128];             /* Render buffer, high half */
 uint8  buf [640];             /* Render buffer, output */
 uint32 const* dlin;           /* Display list line */
 uint32 const* sbnk;           /* Source PRAM bank */
 uint32 const* tbnk;           /* Tileset PRAM bank */
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
 auint  ccy;                   /* Cycles to produce one cell pair */
 auint  cnt;                   /* Remaining output cell pairs to produce */
 auint  cky;                   /* Colorkey value extended */
 auint  csd[2];                /* Current source read (cell pair) */
 auint  shr[4];                /* Alignment shift register */
 auint  dshr;                  /* Destination alignment shift */
 auint  dshl;
 auint  bit3;                  /* Destination alignment shift, bit3 of pixel shift (for 64 bit shift) */
 auint  spos;                  /* Position in source during blit */
 auint  sadd;                  /* Add after each cell pair to source. 1 or 2 */
 auint  spms;                  /* Position mask in shift mode */
 auint  plh;                   /* High half-palette select expanded */
 auint  pll;                   /* Low half-palette select expanded */
 auint  tds;                   /* Tile descriptor */
 auint  trow;                  /* Tile row XOR value for source offset generation */
 auint  i;
 auint  t0;
 auint  t1;
 auint  m0;
 auint  m1;

 /* Only draw line if within display area */

 if (hnd->vid.vln >= 400U){ return; }

 /* Read display list offset & entry size */

 t0   = hnd->vid.dlat; /* Display list definition */
 dlin = &(hnd->st.pram[((t0 & 0xF000U) << 4) & (PRAMS - 1U)]);
 doff = (t0 & 0x0FFCU) << 4;
 dsiz = (t0 & 3U) + 2U;

 /* Read output width & begin positions and clipping settings */

 clps = 0U;
 rrpge_m_vidl_cbup(hnd, 2U, 0x5000U);

 for (i = 0U; i < 2U; i++){

  t0 = hnd->vid.smrr[i];
  rrpge_m_vidl_cbup(hnd, i, t0);

  t1 = (t0 >> 8) & 0x3FU;
  opws[(i << 2) + 0U] = t1;
  opws[(i << 2) + 1U] = t1;
  opws[(i << 2) + 2U] = t1;
  opws[(i << 2) + 3U] = t1;

  t1 = t0 & 0x3FU;
  opbs[(i << 2) + 0U] = t1;
  opbs[(i << 2) + 1U] = t1;
  opbs[(i << 2) + 2U] = t1;
  opbs[(i << 2) + 3U] = t1;

  t1 = ((t0 & 0xC000U) >> 12) |
       ((t0 & 0x00C0U) >>  6);
  clps |= t1 << (i << 2);

 }

 /* Double scan check and line offset calculation */

 t0 = (hnd->vid.dscn) & 0xFFU;
 if ((hnd->vid.vln) <= (t0 << 1)){
  dbl = 1U;
  cyr = 392U;
  doff = (doff + (((hnd->vid.vln) >> 1) << dsiz)) & 0xFFFFU;
 }else{
  dbl = 0U;
  cyr = 192U;
  doff = (doff + (((hnd->vid.vln)  - i) << dsiz)) & 0xFFFFU;
 }

 /* Rebase display list offset (no risk of crossing out of bank from now) */

 dlin += doff;

 /* Render line if possible */

 if ( (dbl == 0U) ||
      ((hnd->vid.vln) & 1U) == 0U){ /* Odd lines in double scan have no render */

  /* Clear the alignemnt shift register. This is only necessary since some
  ** static analysis tools may complain otherwise for using uninitialized
  ** variables, which is true, sine without this, the alignemnt shift register
  ** would only have those elements initialized which would affect the
  ** display. Tools may not be able to determine this. */

  shr[0] = 0U; shr[1] = 0U; shr[2] = 0U; shr[3] = 0U;

  /* Convert display list size to cells */

  dsiz = (auint)(1U) << dsiz;

  /* Reset line buffer */

  t0   = dlin[0];
  t1   = dlin[0] & 0x88888888U;
  t1   = t1 - (t1 >> 3);
  plh  = ((hnd->vid.dscn) >> 12) & 0x7U;
  pll  = ((hnd->vid.dscn) >>  8) & 0x7U;
  plh  = rrpge_m_vidl_ex32[plh];
  pll  = rrpge_m_vidl_ex32[pll];
  t1   = (plh & t1) | (pll & (~t1));
  for (i = 0U; i < 80U; i++){
   bufl[i] = t0;
   bufh[i] = t1;
  }
  doff = 1U;

  /* Process display list. There are two exit conditions: either draining the
  ** display list, or exhausting the cycle budget. */

  while (1){

   cmd  = dlin[doff] & 0xFFFFFFFFU;  /* Current command */
   doff ++;
   ssl  = (cmd >> 13) & 7U;          /* Source select */

   csr  = (hnd->vid.sdef[ssl]) & 0xFFFFU; /* Current source to use */
   csr |= ( (hnd->vid.ckey[ssl >> 2]) <<
            (((ssl & 3U) << 2) + 16U) ) & 0xF0000000U; /* Colorkey */
   sbnk = &(hnd->st.pram[((csr & 0xF000U) << 4) & (PRAMS - 1U)]);
   soff = (cmd >> 16) & 0xFFFFU;
   opw  = opws[ssl];
   opb  = opbs[ssl];
   if (((clps >> ssl) & 1U) != 0U){  /* Clipping requested */
    clpb = &(hnd->vid.cbuf[ssl >> 2][0]);
   }else{                            /* No clipping, use permissive clipping buffer */
    clpb = &(hnd->vid.cbuf[2][0]);
   }

   cyr --;                           /* Cycle taken for display list entry fetch (cyr certain nonzero here) */
   if ((csr & 0x0800) != 0U){        /* X expansion set */
    sadd = 1U;
   }else{                            /* No X expansion */
    sadd = 2U;                       /* Normally (not in Tiled mode) this case source increments by 2 */
   }
   if       ((cmd & 0x1C00U) == 0U){ /* Render command inactive */
    cnt = 0U;
   }else if ((csr & 0x0040U) != 0U){ /* Tiled mode */
    cnt = (((csr - 1U) & 0x7FU) + 1U);
    if (cyr != 0U){ cyr --; }        /* 1 "overhead" cycle */
    if ((csr & 0x0800) == 0U){
     ccy = 3U;                       /* 3 cycles for a cell pair if no X expansion */
    }else{
     ccy = 2U;                       /* 2 cycles for a cell pair if X expanded */
    }
    tbnk = sbnk;                     /* In Tiled mode, the normal source becomes tile descriptors */
    sbnk = &(hnd->st.pram[((cmd & 0x0F00U) << 8) & (PRAMS - 1U)]); /* Bank select for pseudo 6 bit mode */
    sadd = 1U;                       /* Source (tile descriptor) add is always one */
   }else if ((csr & 0x0080U) != 0U){ /* Shift source */
    cnt = opw;
    if (cyr != 0U){ cyr --; }        /* 2 "overhead" cycles */
    if (cyr != 0U){ cyr --; }
    ccy = 2U;                        /* 2 cycles for a cell pair */
   }else{                            /* Positioned source */
    cnt = (((csr - 1U) & 0x7FU) + 1U);
    if (cyr != 0U){ cyr --; }        /* 1 "overhead" cycle */
    ccy = 2U;                        /* 2 cycles for a cell pair */
   }

   if (cnt != 0U){                   /* Clip count of cell pairs to render by remaining cycles */
    t0 = cyr / ccy;
    if (t0 <= cnt){                  /* This command will exhaust the remaining cycles */
     cnt = t0;
     cyr = 0U;
    }else{                           /* Cycles will remain for further commands */
     cyr -= cnt * ccy;
    }
   }

   if (cnt != 0U){                   /* There is something to render */

    /* Calculate colorkey */

    cky = (csr >> 28) & 0xFU;
    cky = rrpge_m_vidl_ex32[cky];

    /* Calculate half-palettes (only used if not in Tiled mode) */

    plh = (cmd >> 10) & 0x7U;
    pll = (csr >>  8) & 0x7U;
    plh = rrpge_m_vidl_ex32[plh];
    pll = rrpge_m_vidl_ex32[pll];

    /* Calculate shifts to the right, used to align the source cells with the
    ** destination. Note that a 64 bit (16 pixel) shift is required, so the
    ** need for the extra bit, which will be used to address the shift
    ** register array. */

    dshr = (cmd & 0x7U) << 2;
    dshl = (32U - dshr) >> 1;     /* Could be 32, so split into two shifts */
    bit3 = (cmd & 0x8U) >> 3;

    /* Do the blit */

    if ((csr & 0x00C0U) == 0x0080U){ /* Shift source */

     /* Get source start offset. */

     spms = csr & 7U;             /* Position mask as cell pair count. OK for X expanded */
     if ((csr & 0x0800U) == 0U){  /* No X expansion: */
      spms ++;                    /* One more bits of source offset is necessary */
     }
     spms = (1U << spms) - 1U;    /* Generated the bit mask */
     soff = soff & (~spms);       /* Remove masked out bits from source offset */
     spos = (((cmd >> 4) & 0x3FU) ^ 0x3FU);
     if ((csr & 0x0800U) == 0U){  /* No X expansion: */
      spos <<= 1U;                /* Address cell pair instead of cell */
     }
     spos &= spms;

     /* Fetch initial source to fill in the alignment shift register. */

     if ((csr & 0x0800U) == 0U){  /* No X expansion */
      csd[0] = sbnk[(soff + spos)     ];
      csd[1] = sbnk[(soff + spos) | 1U];
     }else{                       /* X expansion enabled */
      t0     = sbnk[(soff + spos)];
      t1     = t0 & 0xFFFF0000U;
      t1     = t1 | (t1 >> 8);
      csd[0] = ((t1 & 0xFF00FF00U) >> 4) |
               ((t1 & 0x0F000F00U) >> 8) |
               ((t1 & 0xF000F000U));
      t1     = t0 & 0x0000FFFFU;
      t1     = t1 | (t1 << 8);
      csd[1] = ((t1 & 0x00FF00FFU) << 4) |
               ((t1 & 0x00F000F0U) << 8) |
               ((t1 & 0x000F000FU));
     }
     spos   = (spos + sadd) & spms;
     shr[bit3 + 1U]  = ((csd[0] << dshl) << dshl) | (csd[1] >> dshr);
     shr[bit3 + 2U]  = ((csd[1] << dshl) << dshl);
     shr[0] = shr[2];
     shr[1] = shr[3];

     /* Do the blitting loop */

     i = opb;
     do{

      if ((csr & 0x0800U) == 0U){    /* No X expansion */
       csd[0] = sbnk[(soff + spos)     ];
       csd[1] = sbnk[(soff + spos) | 1U];
      }else{                         /* X expansion enabled */
       t0     = sbnk[(soff + spos)];
       t1     = t0 & 0xFFFF0000U;
       t1     = t1 | (t1 >> 8);
       csd[0] = ((t1 & 0xFF00FF00U) >> 4) |
                ((t1 & 0x0F000F00U) >> 8) |
                ((t1 & 0xF000F000U));
       t1     = t0 & 0x0000FFFFU;
       t1     = t1 | (t1 << 8);
       csd[1] = ((t1 & 0x00FF00FFU) << 4) |
                ((t1 & 0x00F000F0U) << 8) |
                ((t1 & 0x000F000FU));
      }
      spos   = (spos + sadd) & spms;
      shr[bit3 + 0U] |= (csd[0] >> dshr);
      shr[bit3 + 1U]  = ((csd[0] << dshl) << dshl) | (csd[1] >> dshr);
      shr[bit3 + 2U]  = ((csd[1] << dshl) << dshl);

      /* Left cell: Create mask from colorkey */

      m0   = shr[0] ^ cky;           /* Prepare for colorkey */
      m0   = (((m0 & 0x77777777U) + 0x77777777U) | m0); /* Colorkey mask on the highest bit of pixel */
      m0  &= 0x88888888U;            /* Mask out lower pixel bits */
      m0   = (m0 - (m0 >> 3)) + m0;  /* Expand to lower pixels */

      /* Left cell: Calculate low and high halves */

      t0   = shr[0];
      t1   = shr[0] & 0x88888888U;
      t1   = t1 - (t1 >> 3);
      t1   = (plh & t1) | (pll & (~t1));

      /* Left cell: Combine destination */

      bufl[(i << 1) + 0U] = (t0 & m0) | (bufl[(i << 1) + 0U] & (~m0));
      bufh[(i << 1) + 0U] = (t1 & m0) | (bufh[(i << 1) + 0U] & (~m0));

      /* Right cell: Create mask from colorkey */

      m1   = shr[1] ^ cky;           /* Prepare for colorkey */
      m1   = (((m1 & 0x77777777U) + 0x77777777U) | m1); /* Colorkey mask on the highest bit of pixel */
      m1  &= 0x88888888U;            /* Mask out lower pixel bits */
      m1   = (m1 - (m1 >> 3)) + m1;  /* Expand to lower pixels */

      /* Right cell: Calculate low and high halves */

      t0   = shr[1];
      t1   = shr[1] & 0x88888888U;
      t1   = t1 - (t1 >> 3);
      t1   = (plh & t1) | (pll & (~t1));

      /* Right cell: Combine destination */

      bufl[(i << 1) + 1U] = (t0 & m1) | (bufl[(i << 1) + 1U] & (~m1));
      bufh[(i << 1) + 1U] = (t1 & m1) | (bufh[(i << 1) + 1U] & (~m1));

      /* Done, finalize */

      i    = (i + 1U) & 0x3FU;
      shr[0] = shr[2];
      shr[1] = shr[3];
      cnt --;

     }while(cnt != 0U);

    }else{                        /* Position and Tiled source */

     /* Initial (begin) mask */

     if (bit3 == 0U){
      m0 = 0xFFFFFFFFU >> dshr;
      m1 = 0xFFFFFFFFU;
     }else{
      m0 = 0x00000000U;
      m1 = 0xFFFFFFFFU >> dshr;
     }

     /* Calculate Tiled mode row XOR value */

     if ((csr & 0x0800U) == 0U){  /* No X expansion */
      trow = (cmd >> 3) & 0x1EU;  /* Add appropriate Tile row select */
     }else{                       /* X expanded */
      trow = (cmd >> 4) & 0x0FU;  /* Add appropriate Tile row select */
     }

     /* Do the blitting loop */

     spos = 0U;
     i    = (cmd >> 4) & 0x3FU;   /* Cell pair offset */
     tds  = 0U;                   /* (Just shuts up a bogus uninitialized warning, GCC doesn't see that cnt is nonzero here) */
     while (1){

      /* Determine if end cell pair, if so, do an end mask, otherwise fetch
      ** source */

      if (cnt == 0U){                /* End cell */
       if (bit3 == 0U){
        m0 = (0xFFFFFFFFU << dshl) << dshl;
        m1 =  0x00000000U;
       }else{
        m0 =  0xFFFFFFFFU;
        m1 = (0xFFFFFFFFU << dshl) << dshl;
       }
      }else{                         /* Not an end cell */
       t0 = (soff + spos) & 0xFFFFU;
       if ((csr & 0x0040U) != 0U){   /* Tiled mode */
        tds = tbnk[t0];              /* Load tile descriptor */
        if ((cmd & 0x1000U) == 0U){  /* Normal mode (No pseudo 6 bit) */
         sbnk = &(hnd->st.pram[(tds & 0xF0000U) & (PRAMS - 1U)]); /* Bank select for tile */
         plh = (tds >> 28) & 0x7U;   /* Half-palettes */
         pll = (tds >> 24) & 0x7U;
         plh = rrpge_m_vidl_ex32[plh];
         pll = rrpge_m_vidl_ex32[pll];
         cky = (tds >> 20) & 0xFU;   /* Colorkey */
         cky = rrpge_m_vidl_ex32[cky];
        }
        t0  = (tds >> 16) & 0xFFFFU; /* Source offset */
        t0 ^= trow;
       }
       if ((csr & 0x0800U) == 0U){   /* No X expansion */
        csd[0] = sbnk[t0     ];
        csd[1] = sbnk[t0 | 1U];
       }else{                        /* X expansion enabled */
        t0     = sbnk[t0];
        t1     = t0 & 0xFFFF0000U;
        t1     = t1 | (t1 >> 8);
        csd[0] = ((t1 & 0xFF00FF00U) >> 4) |
                 ((t1 & 0x0F000F00U) >> 8) |
                 ((t1 & 0xF000F000U));
        t1     = t0 & 0x0000FFFFU;
        t1     = t1 | (t1 << 8);
        csd[1] = ((t1 & 0x00FF00FFU) << 4) |
                 ((t1 & 0x00F000F0U) << 8) |
                 ((t1 & 0x000F000FU));
       }
       spos += sadd;
       shr[bit3 + 0U] |= (csd[0] >> dshr);
       shr[bit3 + 1U]  = ((csd[0] << dshl) << dshl) | (csd[1] >> dshr);
       shr[bit3 + 2U]  = ((csd[1] << dshl) << dshl);
      }

      /* Left cell: Create masks */

      t0   = shr[0] ^ cky;           /* Prepare for colorkey */
      t0   = (((t0 & 0x77777777U) + 0x77777777U) | t0); /* Colorkey mask on the highest bit of pixel */
      t0  &= 0x88888888U;            /* Mask out lower pixel bits */
      t0   = (t0 - (t0 >> 3)) + t0;  /* Expand to lower pixels */
      m0  &= t0;                     /* Add to combined mask */
      m0  &= clpb[i];                /* Add clipping mask to combined mask */

      /* Left cell: Calculate low and high halves */

      t0   = shr[0];
      if ( ((csr & 0x0040U) == 0U) ||
           ((cmd & 0x1000U) == 0U) ){   /* Not Pseudo 6 bit mode in Tiled mode */
       t1   = shr[0] & 0x88888888U;
       t1   = t1 - (t1 >> 3);
       t1   = (plh & t1) | (pll & (~t1));
      }else{                         /* Pseudo 6 bit mode */
       t1   = tds & 0xFF000000U;
       t1   = (t1 | (t1 >> 6) | (t1 >> 12) | (t1 >> 20)) & 0xC0C0C0C0U;
       t1   = t1 | (t1 >> 4);
       t1   = ((shr[0] & 0x88888888U) >> 3) | (t1 >> 1);
      }

      /* Left cell: Combine destination */

      bufl[(i << 1) + 0U] = (t0 & m0) | (bufl[(i << 1) + 0U] & (~m0));
      bufh[(i << 1) + 0U] = (t1 & m0) | (bufh[(i << 1) + 0U] & (~m0));

      /* Right cell: Create masks */

      t0   = shr[1] ^ cky;           /* Prepare for colorkey */
      t0   = (((t0 & 0x77777777U) + 0x77777777U) | t0); /* Colorkey mask on the highest bit of pixel */
      t0  &= 0x88888888U;            /* Mask out lower pixel bits */
      t0   = (t0 - (t0 >> 3)) + t0;  /* Expand to lower pixels */
      m1  &= t0;                     /* Add to combined mask */
      m1  &= clpb[i];                /* Add clipping mask to combined mask */

      /* Right cell: Calculate low and high halves */

      t0   = shr[1];
      if ( ((csr & 0x0040U) == 0U) ||
           ((cmd & 0x1000U) == 0U) ){   /* Not Pseudo 6 bit mode in Tiled mode */
       t1   = shr[1] & 0x88888888U;
       t1   = t1 - (t1 >> 3);
       t1   = (plh & t1) | (pll & (~t1));
      }else{                         /* Pseudo 6 bit mode */
       t1   = tds & 0x00FF0000U;
       t1   = ((t1 << 6) | t1 | (t1 >> 6) | (t1 >> 12)) & 0x30303030U;
       t1   = t1 | (t1 >> 4);
       t1   = ((shr[1] & 0x88888888U) >> 3) | (t1 << 1);
      }

      /* Right cell: Combine destination */

      bufl[(i << 1) + 1U] = (t0 & m1) | (bufl[(i << 1) + 1U] & (~m1));
      bufh[(i << 1) + 1U] = (t1 & m1) | (bufh[(i << 1) + 1U] & (~m1));

      /* Done, finalize */

      if (cnt == 0U){ break; }       /* End of render */
      m0   = 0xFFFFFFFFU;            /* Clear (all enabled) mask */
      m1   = 0xFFFFFFFFU;
      i    = (i + 1U) & 0x7FU;
      shr[0] = shr[2];
      shr[1] = shr[3];
      cnt --;

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
   m0 = bufl[i];
   t1 = bufh[i];
   t0 = ((m0 & 0x07070707U)     ) | ((t1 & 0x07070707U) << 3);
   t1 = ((m0 & 0x70707070U) >> 4) | ((t1 & 0x70707070U) >> 1);
   m0 = i << 3;
   buf[m0 + 0U] = (uint8)((t1 >> 24)        );
   buf[m0 + 1U] = (uint8)((t0 >> 24)        );
   buf[m0 + 2U] = (uint8)((t1 >> 16) & 0xFFU);
   buf[m0 + 3U] = (uint8)((t0 >> 16) & 0xFFU);
   buf[m0 + 4U] = (uint8)((t1 >>  8) & 0xFFU);
   buf[m0 + 5U] = (uint8)((t0 >>  8) & 0xFFU);
   buf[m0 + 6U] = (uint8)((t1      ) & 0xFFU);
   buf[m0 + 7U] = (uint8)((t0      ) & 0xFFU);
  }

 }

 /* Display list completed, the line is ready to be rendered */

 hnd->cb_lin(hnd, hnd->vid.vln, &buf[0]);
}
