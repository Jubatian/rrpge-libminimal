/**
**  \file
**  \brief     Mixer DMA peripheral
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.03.09
*/


#include "rgm_mix.h"


/* Performs a Mixer DMA operation using the parameters in the application
** state (area RRPGE_STA_MIXER). Returns the number of cycles the operation
** takes. */
auint rrpge_m_mixerop(void)
{
 uint32* pram = &(rrpge_m_edat->st.pram[0]);     /* Address of RW data RAM */
 uint16* stat = &(rrpge_m_edat->st.stat[RRPGE_STA_MIXER]);
 auint ssoh = stat[0xAU] & 0xFFFFU; /* Sample source, whole */
 auint ssol = ((stat[0xBU] & 0xFFFFU) << 16) + (stat[0xCU] & 0xFFFFU); /* Sample source, fraction */
 auint asoh = stat[0x0U] & 0xFFFFU; /* Amplitude source, whole */
 auint asol = ((stat[0x1U] & 0xFFFFU) << 16) + (stat[0x2U] & 0xFFFFU); /* Amplitude source, fraction */
 auint sdoh = stat[0x6U] & 0xFFFFU; /* Sample destination, high part */
 auint sdol = stat[0x7U] & 0xFFFFU; /* Sample destination, low part */
 auint sfrq = ((stat[0xDU] & 0xFFFFU) << 16) + (stat[0xEU] & 0xFFFFU); /* Sample frequency */
 auint afrq = ((stat[0x3U] & 0xFFFFU) << 16) + (stat[0x4U] & 0xFFFFU); /* Amplitudo read (AM) frequency */
 auint ampl = stat[0x9U] & 0xFFFFU; /* Amplitudo multiplier */
 auint clno = ((stat[0xFU] - 1U) & 0xFFFU) + 1U; /* Number of cells to process */
 auint sdpr;
 auint sspr;
 auint aspr;
 auint i;
 auint m0;
 auint m1;
 auint flgs = stat[0xFU]; /* Processing mode flags */
 auint s0;
 auint s1;
 auint ret;

 /* Apply partitioning */
 i = stat[0x5U];          /* For destination partitioning bits */
 sdpr  = (2U << ((i & 0x0F00U) >>  8)) - 1U;
 sdoh &= ~sdpr;
 i = stat[0x8U];          /* For source partitioning bits */
 sspr  = (2U << ((i & 0x0F00U) >>  8)) - 1U;
 ssoh &= ~sspr;
 aspr  = (2U << ((i & 0xF000U) >> 12)) - 1U;
 asoh &= ~aspr;

 /* Apply bank selection on whole parts */
 i = stat[0x5U];          /* For destination bank select bits */
 sdoh |= (i & 0x000FU) << 16;
 i = stat[0x8U];          /* For source bank select bits */
 ssoh |= (i & 0x000FU) << 16;
 asoh |= (i & 0x00F0U) << 12;

 /* Fix amplitudo multiplier (for non-AM modes) */
 if ((ampl & 0x0100U) != 0U){ ampl = 0x100U; }
 else                       { ampl &= 0xFFU; }

 /* AM mode's frequency is 4 times the sample fetch frequency */
 afrq = afrq << 2;

 /* Calculate return value */
 ret = (clno * 14U) + 16U;

 /* In the main loop the flgs variable is used for conditional processing.
 ** This variable is not changed in the loop, so branch prediction works just
 ** fine there, giving reduced code size. No other conditionals are used
 ** (especially not ones relying on data), so the performance of the loop is
 ** uniform for any type of input data. */

 /* Note: clno is at least 1, so a postconditional loop is appropriate */
 do{

  /* Perform AM if appropriate: first load the amplitudo value then increment
  ** the amplitudo source. */
  if ((flgs & 0x4000U)!=0){
   ampl  = (pram[asoh | ((asol >> 16) & aspr)] >> (((asol ^ 0xC000U) & 0xC000U) >> 11)) & 0xFFU;
   asol += afrq;
  }

  /* Load the four samples to process in this iteration. */
  s0    = ((pram[ssoh | ((ssol >> 16) & sspr)] >> (((ssol ^ 0xC000U) & 0xC000U) >> 11)) & 0xFFU) << 16;
  ssol += sfrq;
  s0   |= ((pram[ssoh | ((ssol >> 16) & sspr)] >> (((ssol ^ 0xC000U) & 0xC000U) >> 11)) & 0xFFU);
  ssol += sfrq;
  s1    = ((pram[ssoh | ((ssol >> 16) & sspr)] >> (((ssol ^ 0xC000U) & 0xC000U) >> 11)) & 0xFFU) << 16;
  ssol += sfrq;
  s1   |= ((pram[ssoh | ((ssol >> 16) & sspr)] >> (((ssol ^ 0xC000U) & 0xC000U) >> 11)) & 0xFFU);
  ssol += sfrq;

  /* Calculate amplitude multiplication:
  ** s =   ((s - 128) * ampl) / 256) + 128 with unsigned arithmetic:
  ** s =  (((s * ampl) - (128 * ampl)) / 256) + 128
  ** s = ((((s * ampl) / 128) - ((128 * ampl) / 128)) / 2) + 128
  ** s = ((((s * ampl) / 128) - ampl) / 2) + 128
  ** s =  (((s * ampl) / 128) + (256 - ampl)) / 2
  ** This is also packed, so two 16 bit operations are performed in one */
  m0 = (ampl << 16) + ampl;
  i  = 0x01000100U - m0;
  s0 = ((((s0 * ampl) >> 7) + i) >> 1) & 0x00FF00FFU;
  s1 = ((((s1 * ampl) >> 7) + i) >> 1) & 0x00FF00FFU;

  /* Calculate destination address */
  i  = sdoh | (sdol & sdpr);

  /* Do saturated addition with the destination if requested */
  if ((flgs & 0x8000U) == 0U){

   m1  = pram[i];
   m0  = m1 >> 16;
   m1  = (m1 & 0xFFU) + ((m1 & 0xFF00U) << 8);
   m0  = (m0 & 0xFFU) + ((m0 & 0xFF00U) << 8);

   s0 += m0;                 /* Ranges from 0 to 511 (9 bits) per element */
   s1 += m1;

   /* If the destination value is <128, then need to limit at 128 */
   m0  = (s0 & 0x01000100U) | ((s0 & 0x00800080U) << 1); /* Zero if both bits clear: <128 */
   m1  = (s1 & 0x01000100U) | ((s1 & 0x00800080U) << 1);
   m0 |= m0 - (m0 >> 8);                                 /* Create mask (9 bit) out of it */
   m1 |= m1 - (m1 >> 8);
   s0  = (s0 & m0) | (0x00800080U & (~m0));              /* Truncated to 128 at low end */
   s1  = (s1 & m1) | (0x00800080U & (~m1));

   s0  = s0 - 0x00800080U;   /* Align it to zero (-128) */
   s1  = s1 - 0x00800080U;

   /* If the destination value is >255, then need to limit at 255 */
   m0  = (s0 & 0x01000100U);                            /* Above 255 if set */
   m1  = (s1 & 0x01000100U);
   m0  = m0 - (m0 >> 8);                                /* Create mask (8 bit) out of it */
   m1  = m1 - (m1 >> 8);
   s0  = (s0 | m0) & 0x00FF00FFU;                       /* Truncated to 255 at high end */
   s1  = (s1 | m1) & 0x00FF00FFU;
  }

  /* Write out destination and increment it's pointer */
  s0 = (s0 & 0xFFU) + (s0 >> 8);
  s1 = (s1 & 0xFFU) + (s1 >> 8);
  pram[i] = (s0 << 16) | s1;
  sdol ++;

  /* One less pair of samples to go */
  clno--;
 }while (clno != 0);

 return ret;
}
