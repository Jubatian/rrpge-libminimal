/**
**  \file
**  \brief     Mixer DMA peripheral
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.15
*/


#include "rgm_mixo.h"


/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* Accelerator tables for 16 bit expansion */
static auint rrpge_m_mixo_tb_8[256];
static auint rrpge_m_mixo_tb_7[128];
static auint rrpge_m_mixo_tb_6[64];
static auint rrpge_m_mixo_tb_5[32];
static auint rrpge_m_mixo_tb_4[16];
static auint rrpge_m_mixo_tb_3[8];
static const auint rrpge_m_mixo_tb_2[4] = {0x0000U, 0x5555U, 0xAAAAU, 0xFFFFU};
static const auint rrpge_m_mixo_tb_1[2] = {0x0000U, 0xFFFFU};
static const auint* const rrpge_m_mixo_tb[8] = {
 &rrpge_m_mixo_tb_1[0], &rrpge_m_mixo_tb_2[0],
 &rrpge_m_mixo_tb_3[0], &rrpge_m_mixo_tb_4[0],
 &rrpge_m_mixo_tb_5[0], &rrpge_m_mixo_tb_6[0],
 &rrpge_m_mixo_tb_7[0], &rrpge_m_mixo_tb_8[0]
};



/* Initializes Mixer DMA, populating some accelerator tables. */
void  rrpge_m_mixo_init(void)
{
 auint i;

 for (i = 0U; i < 8U; i++){
  rrpge_m_mixo_tb_3[i] = (i << 13) | (i << 10) | (i << 7) | (i << 4) | (i << 1) | (i >> 2);
 }

 for (i = 0U; i < 16U; i++){
  rrpge_m_mixo_tb_4[i] = (i << 12) | (i <<  8) | (i << 4) |  i;
 }

 for (i = 0U; i < 32U; i++){
  rrpge_m_mixo_tb_5[i] = (i << 11) | (i <<  6) | (i << 1) | (i >> 4);
 }

 for (i = 0U; i < 64U; i++){
  rrpge_m_mixo_tb_6[i] = (i << 10) | (i <<  4) | (i >> 2);
 }

 for (i = 0U; i < 128U; i++){
  rrpge_m_mixo_tb_7[i] = (i <<  9) | (i <<  2) | (i >> 5);
 }

 for (i = 0U; i < 256U; i++){
  rrpge_m_mixo_tb_8[i] = (i <<  8) |  i;
 }
}



/* Performs a Mixer DMA operation using the parameters in the application
** state (area RRPGE_STA_MIXER). Returns the number of cycles the operation
** takes. */
auint rrpge_m_mixo(rrpge_object_t* hnd)
{
 uint32* pram = &(hnd->st.pram[0]); /* Address of Peripheral RAM */
 auint soff;                        /* Source bit offset (incrementing part) */
 auint spar;                        /* Source partition select bits */
 auint spms;                        /* Source partition mask (applied on source bit offset) */
 auint swdt;                        /* Sample bit width (increment for source bit offset) */
 auint swdm;                        /* Sample bit width mask (for fetching samples) */
 auint scfg = hnd->mix.scf;         /* Source configuration */
 auint sfad;                        /* Sample pointer fraction add value */
 auint sfrc = hnd->mix.sfr;         /* Sample pointer fraction */
 auint ampl;                        /* Current amplitudo */
 auint ampa;                        /* Amplitudo add value */
 auint ilth;                        /* 64 bit source input register, high (left) */
 auint iltl;                        /* 64 bit source input register, low (right) */
 auint smpc;                        /* Current sample */
 auint smpp;                        /* Previous sample */
 auint dofh = hnd->mix.dbk << 16;   /* Destination offset high */
 auint dofl = hnd->mix.dof;         /* Destination offset low (incrementing part) */
 auint dcnt;                        /* Destination count */
 auint rsm0;                        /* First result sample */
 auint rsm1;                        /* Second result sample */
 auint i;
 auint t;
 auint ret;

 /* Prepare destination */

 dofh = dofh & (PRAMS - 1U);        /* Keep within PRAM */
 dcnt = ((hnd->mix.dct - 1U) & 0xFFFU) + 1U; /* Count of cells to do */

 /* Prepare source offsets */

 if ((scfg & 0x0010U) != 0U){       /* Use full PRAM */
  spms = (PRAMS << 5) - 1U;
  soff = (((hnd->mix.soh) << 16) | (hnd->mix.sol)) & spms;
  spar = 0U;
 }else{                             /* Apply partitioning */
  i = scfg & 0x000FU;
  if (i < 4U){ i = 4U; }
  spms = (2U << i) - 1U;
  soff = hnd->mix.sol;
  spar = (((hnd->mix.soh) << 16) | ((hnd->mix.spr) & (~spms))) & ((PRAMS << 5) - 1U);
 }

 /* Prepare source parameters */

 sfad = ((hnd->mix.sfa - 1U) & 0xFFFFU) + 1U;
 swdt = (scfg >> 12) & 0xFU;
 if ((swdt & 0x8U) != 0U){ swdt = 15U; }
 swdt ++;
 swdm = (1 << swdt) - 1U;
 scfg = (scfg & 0xF01FU) | ((hnd->mix.dct & 0x8000U) >> 4); /* Saturated add necessary flag */

 /* Prepare amplitudo */

 ampa = hnd->mix.ama;
 ampl = ((hnd->mix.ami - 1U) & 0xFFFFU) + 1U;

 /* Initialize memories */

 ilth = pram[(spar | ( soff        & spms)) >> 5];
 iltl = pram[(spar | ((soff + 32U) & spms)) >> 5];
 smpp = (ilth << (soff & 0x10U)) |
        (((iltl >> 16) & (0U - ((soff >> 4) & 1U))) & 0xFFFFU);
 smpp = ((smpp << (soff & 0xFU)) >> (32U - swdt)) & swdm;
 if (swdt <= 8U){ smpp = rrpge_m_mixo_tb[swdt - 1U][smpp]; }
 t     = soff | 0x1FU;
 soff += swdt;
 if (t != (soff | 0x1FU)){ ilth = iltl; }
 smpc = (ilth << (soff & 0x10U)) |
        (((iltl >> 16) & (0U - ((soff >> 4) & 1U))) & 0xFFFFU);
 smpc = ((smpc << (soff & 0xFU)) >> (32U - swdt)) & swdm;
 if (swdt <= 8U){ smpc = rrpge_m_mixo_tb[swdt - 1U][smpc]; }
 t     = soff | 0x1FU;
 soff += swdt;
 if (t != (soff | 0x1FU)){ ilth = iltl; }

 /* Calculate return value (cycles consumed) */

 ret = (dcnt * 6U) + 30U;

 /* In the main loop the scfg variable is used for conditional processing.
 ** This variable is not changed in the loop, so branch prediction works just
 ** fine there, giving reduced code size. */

 /* Note: dcnt is at least 1, so a postconditional loop is appropriate */

 do{

  /* Fetch next source & place it in the 64 bit source input register */

  iltl = pram[(spar | ((soff + 32U) & spms)) >> 5];

  /* Interpolate first result sample */

  rsm0 = (smpp + (((smpc - smpp) * (sfrc & 0xE000U)) >> 16)) & 0xFFFFU;

  /* Increase sample pointer fraction, doing a sample fetch if wrapped */

  sfrc += sfad;
  if ((sfrc & 0x10000U) != 0U){
   sfrc &= 0xFFFFU;
   smpp = smpc;
   smpc = (ilth << (soff & 0x10U)) |
          (((iltl >> 16) & (0U - ((soff >> 4) & 1U))) & 0xFFFFU);
   smpc = ((smpc << (soff & 0xFU)) >> (32U - swdt)) & swdm;
   if (swdt <= 8U){ smpc = rrpge_m_mixo_tb[swdt - 1U][smpc]; }
   t     = soff | 0x1FU;
   soff += swdt;
   if (t != (soff | 0x1FU)){ ilth = iltl; }
  }

  /* Interpolate second result sample */

  rsm1 = (smpp + (((smpc - smpp) * (sfrc & 0xE000U)) >> 16)) & 0xFFFFU;

  /* Increase sample pointer fraction, doing a sample fetch if wrapped */

  sfrc += sfad;
  if ((sfrc & 0x10000U) != 0U){
   sfrc &= 0xFFFFU;
   smpp = smpc;
   smpc = (ilth << (soff & 0x10U)) |
          (((iltl >> 16) & (0U - ((soff >> 4) & 1U))) & 0xFFFFU);
   smpc = ((smpc << (soff & 0xFU)) >> (32U - swdt)) & swdm;
   if (swdt <= 8U){ smpc = rrpge_m_mixo_tb[swdt - 1U][smpc]; }
   t     = soff | 0x1FU;
   soff += swdt;
   if (t != (soff | 0x1FU)){ ilth = iltl; }
  }

  /* Apply amplitude on the samples */

  rsm0 = (((rsm0 * ampl) >> 15) + 0x10000U - ampl) >> 1;
  rsm1 = (((rsm1 * ampl) >> 15) + 0x10000U - ampl) >> 1;

  /* Add amplitude add value to amplitude */

  ampl = ampl + ampa;
  if ((ampa & 0x8000U) == 0U){ /* Increment */
   ampl = ampl & ( 0xFFFFU + (ampl >> 16)); /* Saturate at 0x10000U */
  }else{                       /* Decrement */
   ampl = ampl & (0x10000U - (ampl >> 16)); /* Saturate at 0 */
   ampl = ampl + ((0x10000U - ampl) >> 16); /* Make it 1 if it was 0 */
  }

  /* Add to destination if requested */

  if ((scfg & 0x0800U) != 0U){
   t     = pram[dofh | (dofl & 0xFFFFU)];
   rsm0 += (t >> 16) & 0xFFFFU;
   rsm0 -= 0x8000U;
   rsm0  = rsm0 & (0x20000U - (rsm0 >> 17)); /* Saturate low end at 0x0000 */
   rsm0  = rsm0 | (0x10000U - (rsm0 >> 16));
   rsm0 &= 0xFFFFU;                          /* Saturate high end at 0xFFFF */
   rsm1 += t & 0xFFFFU;
   rsm1 -= 0x8000U;
   rsm1  = rsm0 & (0x20000U - (rsm0 >> 17)); /* Saturate low end at 0x0000 */
   rsm1  = rsm0 | (0x10000U - (rsm0 >> 16));
   rsm1 &= 0xFFFFU;                          /* Saturate high end at 0xFFFF */
  }

  /* Write out destination */

  pram[dofh | (dofl & 0xFFFFU)] = (rsm0 << 16) | rsm1;

  /* Increment destination offset */

  dofl ++;

  /* Loop ends */

  dcnt --;
 }while (dcnt != 0U);

 /* Set saturated add for next time */

 hnd->mix.scf |= 0x8000U;

 /* Done */

 return ret;
}
