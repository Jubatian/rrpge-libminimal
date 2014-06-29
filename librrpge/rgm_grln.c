/**
**  \file
**  \brief     Graphics line renderer
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.29
*/


#include "rgm_grln.h"


/* Video memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  VRAMS  RRPGE_M_VRAMS


/* Local buffer for rendering lines. Only first 80 cells are used for output,
** however the full range is used for render. */
static uint32 rrpge_m_grln_buf[128U];


/* Mask / colorkey values */
static const uint8 rrpge_m_grln_msk[32U] = {
 0x00U, 0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U,
 0x08U, 0x09U, 0x0AU, 0x0BU, 0x0CU, 0x0DU, 0x0EU, 0x0FU,
 0x00U, 0x10U, 0x20U, 0x30U, 0x40U, 0x1FU, 0x60U, 0x70U,
 0x80U, 0x90U, 0x3FU, 0x7FU, 0xC0U, 0xFFU, 0xE0U, 0xF0U};



/* Renders current graphics line. Also performs callback to host. */
void rrpge_m_grln(void)
{
 uint32 const* dlin;           /* Display list line */
 uint32 const* sbnk;           /* Source VRAM bank */
 auint  soff;                  /* Source base offset within VRAM bank */
 auint  doff;                  /* Offset within display list */
 auint  dsiz;                  /* Size of display list line */
 auint  opw;                   /* Output width */
 auint  opb;                   /* Output begin */
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

 /* Only draw line if within display area */

 if (rrpge_m_info.vln >= 400U){ return; }

 /* Read display list offset & entry size */
 /* Read output width & begin position */

 i = rrpge_m_edat->stat.ropd[0xEE3U] & ((VRAMS - 1U) >> 9); /* Display list definition */
 i = i & (~(((auint)(4U) << (i & 3U)) - 4U));
 dlin = &(rrpge_m_edat->stat.vram[(i & (~(auint)(3U))) << 9]);
 dsiz = (i & 3U) + 2U;
 dlin += rrpge_m_info.vln << dsiz; /* Position on the current line in display list */
 opb  = rrpge_m_edat->stat.ropd[0xEE2U]; /* Output width, begin & double scan */
 dbl  = opb >> 15;
 opw  = (opb >> 8) & 0x7FU;
 opb  = opb & 0x7FU;
 cyr  = 192U;
 if (dbl != 0U){ /* Double scan */
  dsiz ++;
  cyr = 392U;
 }

 if ( (dbl == 0U) ||
      (rrpge_m_info.vln & 1U) == 0U){ /* Odd lines in double scan have no render */

  dsiz = (auint)(1U) << dsiz;

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
   csr  = rrpge_m_edat->stat.ropd[0xEE4U + ((cmd >> 28) & 3U)]; /* Current source to use */
   sbnk = &(rrpge_m_edat->stat.vram[((csr & 0xC0U) << 10)]);
   soff = (csr & 0xFF00U) | ((((cmd >> 16) & 0xFFFU) << (csr & 7U)) & 0xFFFFU);
   if       ((cmd & 0xBC00U) == 0U){ /* Render command inactive */
    scy = 0U;
   }else if ((csr & 0x0020U) != 0U){ /* Shift source */
    scy = opw;
   }else{                            /* Position source */
    scy = ((auint)(1U) << (csr & 7U)) * (((csr & 0x18U) >> 2) + 1U);
   }
   cyr--;                         /* Cycle taken for display list entry fetch (cyr certain nonzero here) */
   if (scy > cyr){ scy = cyr; }
   cyr -= scy;                    /* Width to render & remaining cycles done */

   if ((cmd & 0xBC00U) != 0U){    /* Render command not inactive */

    /* Source line select in cmd is no longer used. Substitute with the
    ** current graphics mode for accessing the latter faster later on */

    cmd  = (cmd & 0xC000FFFFU) | ((rrpge_m_info.vbm & 0x80U) << 9); /* bit 16 set in 8bit mode */

    /* Calculate mask / colorkey */

    msk  = rrpge_m_grln_msk[(cmd >> 10) & 0x1FU];
    if ((cmd & 0x10000) == 0U){   /* 4 bit mode */
     msk &= 0x0FU;
     msk |= msk << 4;
    }
    msk |= msk << 8;
    msk |= msk << 16;

    msb = msk | ((uint32)(0U) - ((cmd >> 15) & 1U)); /* Mask base is either 0xFFFFFFFF or the mask */

    /* Do the blit */

    if ((csr & 0x0020U) != 0U){   /* Shift source */

     /* Will shift to the left, based on low 3 bits of command. Initial source
     ** is always fetched. */

     dshl = cmd & (0x6U | ((cmd >> 16) & 1U)); /* Lowest bit only affects in 8 bit mode */
     dshl <<= 2;
     dshr = (32U - dshl) >> 1;    /* Could be 32, so split into two shifts */

     /* Source position */

     spms = (1U << (csr & 7U)) - 1U;
     spos = ((cmd >> 3) & 0x7FU) & spms;

     /* Fetch first source */

     csd  = sbnk[soff + spos];
     spos = (spos + 1U) & spms;
     psd  = csd << dshl;

     /* Do the blitting loop */

     i    = opb;
     while (scy != 0U){

      csd  = sbnk[soff + spos];
      spos = (spos + 1U) & spms;
      psd  |= (csd >> dshr) >> dshr; /* Now 'psd' contains the destination aligned source */

      d32  = rrpge_m_grln_buf[i];

      /* Create masks */

      m32  = msb;                    /* Combined mask */

      t32  = psd ^ msk;              /* Prepare for colorkey */
      if ((cmd & 0x10000U) == 0U){   /* 4 bit mode */
       t32 = (((t32 & 0x77777777U) + 0x77777777U) | t32) & 0x88888888U;
       t32 = (t32 - (t32 >> 3)) + t32;  /* Colorkey mask (0: background) */
       u32  = d32 & 0x88888888U;     /* bit 3 is priority selector */
       u32 |= u32 >> 1;
       u32 |= u32 >> 2;
      }else{                         /* 8 bit mode */
       t32 = (((t32 & 0x7F7F7F7FU) + 0x7F7F7F7FU) | t32) & 0x80808080U;
       t32 = (t32 - (t32 >> 7)) + t32;  /* Colorkey mask (0: background) */
       u32  = d32 & 0x20202020U;     /* bit 5 is priority selector */
       u32 |= u32 >> 1;
       u32 |= u32 << 2;
       u32 |= u32 >> 4;
      }
      u32 = ~u32;
      m32 &= t32 | ((uint32)(0U) - ((cmd >> 30) & 1U)); /* Add colorkey to write mask if enabled */
      m32 &= u32 | (0xFFFFFFFFU  + ((cmd >> 31) & 1U)); /* Add priority selector to write mask if enabled */

      /* Combine destination */

      rrpge_m_grln_buf[i] = (psd & m32) | (d32 & (~m32));
      i    = (i + 1U) & 0x7F;

      /* Done, finalize */

      psd  = csd << dshl;
      scy --;

     }

    }else{                        /* Position source */

     /* Will shift to the right, based on low 3 bits of command. Here one more
     ** destination needs to be generated than sources, scy represents the
     ** count of source fetches required. */

     dshr = cmd & (0x6U | ((cmd >> 16) & 1U)); /* Lowest bit only affects in 8 bit mode */
     dshr <<= 2;
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

      t32  = psd ^ msk;              /* Prepare for colorkey */
      if ((cmd & 0x10000U) == 0U){   /* 4 bit mode */
       t32 = (((t32 & 0x77777777U) + 0x77777777U) | t32) & 0x88888888U;
       t32 = (t32 - (t32 >> 3)) + t32;  /* Colorkey mask (0: background) */
       u32  = d32 & 0x88888888U;     /* bit 3 is priority selector */
       u32 |= u32 >> 1;
       u32 |= u32 >> 2;
      }else{                         /* 8 bit mode */
       t32 = (((t32 & 0x7F7F7F7FU) + 0x7F7F7F7FU) | t32) & 0x80808080U;
       t32 = (t32 - (t32 >> 7)) + t32;  /* Colorkey mask (0: background) */
       u32  = d32 & 0x20202020U;     /* bit 5 is priority selector */
       u32 |= u32 >> 1;
       u32 |= u32 << 2;
       u32 |= u32 >> 4;
      }
      u32 = ~u32;
      m32 &= t32 | ((uint32)(0U) - ((cmd >> 30) & 1U)); /* Add colorkey to write mask if enabled */
      m32 &= u32 | (0xFFFFFFFFU  + ((cmd >> 31) & 1U)); /* Add priority selector to write mask if enabled */

      /* Combine destination */

      rrpge_m_grln_buf[i] = (psd & m32) | (d32 & (~m32));
      i    = (i + 1U) & 0x7F;

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
