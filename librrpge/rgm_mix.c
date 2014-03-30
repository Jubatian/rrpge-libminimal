/**
**  \file
**  \brief     Mixer DMA peripheral
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.28
*/


#include "rgm_mix.h"


/* Performs a Mixer DMA operation using the parameters in the ROPD (area
** 0xED0 - 0xEDF). Updates the Mixer area in the ROPD according to the result
** of the operation. Returns the number of cycles the operation takes. */
auint rrpge_m_mixerop(void)
{
 uint16* dram = &(rrpge_m_edat->stat.dram[0]); /* Address of RW data RAM */
 auint ssoh = rrpge_m_edat->stat.ropd[0xED0U]; /* Sample source, whole */
 auint ssol = rrpge_m_edat->stat.ropd[0xED1U]; /* Sample source, fraction */
 auint asoh = rrpge_m_edat->stat.ropd[0xED6U]; /* Amplitudo source, whole */
 auint asol = rrpge_m_edat->stat.ropd[0xED7U]; /* Amplitudo source, fraction */
 auint fsoh = rrpge_m_edat->stat.ropd[0xED4U]; /* Frequency source, whole */
 auint fsol = rrpge_m_edat->stat.ropd[0xED5U]; /* Frequency source, fraction */
 auint sdoh = rrpge_m_edat->stat.ropd[0xED8U]; /* Sample destination, whole */
 auint sdol = 0U;                              /* Sample destination, fraction */
 auint sfri = rrpge_m_edat->stat.ropd[0xED2U] & 0xFFU; /* Sample frequency index */
 auint ampl = rrpge_m_edat->stat.ropd[0xED3U]; /* Amplitudo multiplier */
 auint afri = rrpge_m_edat->stat.ropd[0xED9U] & 0xFFU; /* Amplitudo read (AM) frequency index */
 auint ffri = rrpge_m_edat->stat.ropd[0xED9U] >> 8;    /* Frequency read (FM) frequency index */
 auint fthi = (auint)(rrpge_m_edat->stat.ropd[0xEDAU] & 0x7FFU) << 8; /* Frequency table, whole */
 auint ftli = (auint)(rrpge_m_edat->stat.ropd[0xEDBU] & 0x7FFU) << 8; /* Frequency table, fraction */
 uint16* fthp = &(rrpge_m_edat->stat.dram[fthi]);      /* Pointer to freq. table high */
 uint16* ftlp = &(rrpge_m_edat->stat.dram[ftli]);      /* Pointer to freq. table low */
 auint wdno = ((rrpge_m_edat->stat.ropd[0xEDFU] - 1U) & 0x1FFU) + 1U; /* Number of words to process */
 auint sdpr;
 auint sspr;
 auint aspr;
 auint fspr;
 auint i;
 auint m;
 auint flgs = rrpge_m_edat->stat.ropd[0xEDFU]; /* Processing mode flags */
 auint s;
 auint ret;

 /* Copy high parts to low part high words, for partitioning */
 sdol |= sdoh << 16;
 ssol |= ssoh << 16;
 asol |= asoh << 16;
 fsol |= fsoh << 16;

 /* Apply partitioning */
 i = rrpge_m_edat->stat.ropd[0xEDCU];          /* Partitioning bits */
 sdpr  = (2U <<  (i & 0x000FU)       ) - 1U;
 sdoh &= ~sdpr;
 sdpr  = (sdpr << 16) | 0xFFFFU;
 sdol &=  sdpr;
 sspr  = (2U << ((i & 0x00F0U) >>  4)) - 1U;
 ssoh &= ~sspr;
 sspr  = (sspr << 16) | 0xFFFFU;
 ssol &=  sspr;
 aspr  = (2U << ((i & 0x0F00U) >>  8)) - 1U;
 asoh &= ~aspr;
 aspr  = (aspr << 16) | 0xFFFFU;
 asol &=  aspr;
 fspr  = (2U << ((i & 0xF000U) >> 12)) - 1U;
 fsoh &= ~fspr;
 fspr  = (fspr << 16) | 0xFFFFU;
 fsol &=  fspr;

 /* Apply bank selection on whole parts */
 i = rrpge_m_edat->stat.ropd[0xEDDU];          /* Bank select bits */
 sdoh |= (i & 0x0007U) << 16;
 ssoh |= (i & 0x0070U) << 12;
 asoh |= (i & 0x0700U) <<  8;
 fsoh |= (i & 0x7000U) <<  4;

 /* Fix amplitudo multiplier (for non-AM modes) */
 if ((ampl & 0x0100U) != 0U){ ampl = 0x100U; }
 else                       { ampl &= 0xFFU; }

 /* Calculate return value */
 if ((flgs & 0x8000U) != 0U){ ret = (wdno * 10U) + 20U; }
 else                       { ret = (wdno *  6U) + 18U; }

 /* In the main loop the flgs variable is used for conditional processing.
 ** This variable is not changed in the loop, so branch prediction works just
 ** fine there, giving reduced code size. No other conditionals are used
 ** (especially not ones relying on data), so the performance of the loop is
 ** uniform for any type of input data. */

 /* Note: wdno is at least 1, so a postconditional loop is appropriate */
 do{

  /* Perform FM if appropriate: first load the frequency value then increment
  ** the frequency source. */
  if ((flgs & 0x8000U)!=0){
   sfri  = (dram[fsoh | (fsol >> 16)] >> (((fsol ^ 0x8000U) & 0x8000U) >> 12)) & 0xFFU;
   fsol += ((auint)(fthp[ffri]) << 16) + (auint)(ftlp[ffri]);
   fsol &= fspr;
  }

  /* Perform AM if appropriate: first load the amplitudo value then increment
  ** the amplitudo source. */
  if ((flgs & 0x4000U)!=0){
   ampl  = (dram[asoh | (asol >> 16)] >> (((asol ^ 0x8000U) & 0x8000U) >> 12)) & 0xFFU;
   asol += ((auint)(fthp[afri]) << 16) + (auint)(ftlp[afri]);
   asol &= aspr;
  }

  /* Load the two samples to process in this iteration. For the second sample
  ** use a temporary half add method. */
  s     = ((dram[ssoh | (ssol >> 16)] >> (((ssol ^ 0x8000U) & 0x8000U) >> 12)) & 0xFFU) << 16;
  i     = ((auint)(fthp[sfri]) << 16) + (auint)(ftlp[sfri]);
  m     = (ssol + (i >> 1)) & sspr;
  s    |= ((dram[ssoh | (   m >> 16)] >> (((   m ^ 0x8000U) & 0x8000U) >> 12)) & 0xFFU);
  ssol += i;
  ssol &= sspr;

  /* Calculate amplitude multiplication:
  ** s =   ((s - 128) * ampl) / 256) + 128 with unsigned arithmetic:
  ** s =  (((s * ampl) - (128 * ampl)) / 256) + 128
  ** s = ((((s * ampl) / 128) - ((128 * ampl) / 128)) / 2) + 128
  ** s = ((((s * ampl) / 128) - ampl) / 2) + 128
  ** s =  (((s * ampl) / 128) + (256 - ampl)) / 2
  ** This is also packed, so two 16 bit operations are performed in one */
  m = (ampl << 16) + ampl;
  i = 0x01000100U - m;
  s = ((((s * ampl) >> 7) + i) >> 1) & 0x00FF00FFU;

  /* Calculate destination address */
  i = sdoh + (sdol >> 16);

  /* Do saturated addition with the destination if requested */
  if ((flgs & 0x2000U) == 0U){

   m  = dram[i];
   m  = (m & 0xFFU) + ((m & 0xFF00U) << 8);

   s += m;                   /* Ranges from 0 to 511 (9 bits) per element */

   /* If the destination value is <128, then need to limit at 128 */
   m  = (s & 0x01000100U) | ((s & 0x00800080U) << 1); /* Zero if both bits clear: <128 */
   m |= m - (m >> 8);                                 /* Create mask (9 bit) out of it */
   s  = (s & m) | (0x00800080U & (~m));               /* Truncated to 128 at low end */

   s  = s - 0x00800080U;     /* Align it to zero (-128) */

   /* If the destination value is >255, then need to limit at 255 */
   m  = (s & 0x01000100U);                            /* Above 255 if set */
   m  = m - (m >> 8);                                 /* Create mask (8 bit) out of it */
   s  = (s | m) & 0x00FF00FFU;                        /* Truncated to 255 at high end */
  }

  /* Write out destination and increment it's pointer */
  s = (s & 0xFFU) + (s >> 8);
  dram[i] = s;
  sdol += 0x10000U;
  sdol &= sdpr;

  /* One less pair of samples to go */
  wdno--;
 }while (wdno != 0);

 /* Undo partitioning (apply whole bits to the whole parts) */
 sdoh |= sdol >> 16;
 ssoh |= ssol >> 16;
 asoh |= asol >> 16;
 fsoh |= fsol >> 16;

 /* Write backs registers which have to be written back. All of them were
 ** updated right, so it is only necessary to send them back in the
 ** appropriate ROPD cells. */
 rrpge_m_edat->stat.ropd[0xED0U] = (uint16)(ssoh);
 rrpge_m_edat->stat.ropd[0xED1U] = (uint16)(ssol);
 rrpge_m_edat->stat.ropd[0xED2U] = (uint16)(sfri);
 rrpge_m_edat->stat.ropd[0xED3U] = (uint16)(ampl);
 rrpge_m_edat->stat.ropd[0xED4U] = (uint16)(fsoh);
 rrpge_m_edat->stat.ropd[0xED5U] = (uint16)(fsol);
 rrpge_m_edat->stat.ropd[0xED6U] = (uint16)(asoh);
 rrpge_m_edat->stat.ropd[0xED7U] = (uint16)(asol);
 rrpge_m_edat->stat.ropd[0xED8U] = (uint16)(sdoh);

 return ret;
}
