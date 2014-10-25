/**
**  \file
**  \brief     Mixer DMA peripheral
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
*/


#include "rgm_mix.h"


/* Performs a Mixer DMA operation using the parameters in the application
** state (area RRPGE_STA_MIXER). Returns the number of cycles the operation
** takes. */
auint rrpge_m_mixerop(void)
{
 uint32* pram = &(rrpge_m_edat->st.pram[0]); /* Address of RW data RAM */
 auint ssoh = rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0xAU]; /* Sample source, whole */
 auint ssol = ((auint)(rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0xBU]) << 16) + (auint)(rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0xCU]); /* Sample source, fraction */
 auint asoh = rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x0U]; /* Amplitude source, whole */
 auint asol = ((auint)(rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x1U]) << 16) + (auint)(rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x2U]); /* Amplitude source, fraction */
 auint sdoh = rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x7U]; /* Sample destination, high part */
 auint sdol = rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x8U]; /* Sample destination, low part */
 auint sfrq = ((auint)(rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0xDU]) << 16) + (auint)(rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0xEU]); /* Sample frequency */
 auint afrq = ((auint)(rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x3U]) << 16) + (auint)(rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x4U]); /* Amplitudo read (AM) frequency */
 auint ampl = rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x9U]; /* Amplitudo multiplier */
 auint clno = ((rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0xFU] - 1U) & 0xFFFU) + 1U; /* Number of cells to process */
 auint sdpr;
 auint sspr;
 auint aspr;
 auint i;
 auint m0;
 auint m1;
 auint flgs = rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0xFU]; /* Processing mode flags */
 auint s0;
 auint s1;
 auint ret;

 /* Apply partitioning */
 i = rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x5U];          /* Partitioning bits */
 sdpr  = (2U <<  (i & 0x000FU)       ) - 1U;
 sdoh &= ~sdpr;
 sspr  = (2U << ((i & 0x00F0U) >>  4)) - 1U;
 ssoh &= ~sspr;
 aspr  = (2U << ((i & 0x0F00U) >>  8)) - 1U;
 asoh &= ~aspr;

 /* Apply bank selection on whole parts */
 i = rrpge_m_edat->st.stat[RRPGE_STA_MIXER + 0x6U];          /* Bank select bits */
 sdoh |= (i & 0x000FU) << 16;
 ssoh |= (i & 0x00F0U) << 12;
 asoh |= (i & 0x0F00U) <<  8;

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
