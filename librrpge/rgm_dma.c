/**
**  \file
**  \brief     DMA functions
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.27
**
**
**  Implementations of the CPU RAM DMA's and the Video RAM <=> CPU RAM DMA.
*/


#include "rgm_dma.h"



/* Executes a CPU Fill DMA by the contents of the ROPD. May generate a DMA
** halt cause. Returns consumed cycles. */
auint rrpge_m_dma_fill(void)
{
 auint i;
 auint s;
 auint d;
 uint16* dp;

 s = rrpge_m_edat->stat.ropd[0xEC0U];
 d = rrpge_m_edat->stat.ropd[0xEC1U];

 if (d >= 0x1C00U){ /* Not user RAM area */
  rrpge_m_info.hlt |= RRPGE_HLT_DMA;
  return 0U;
 }

 dp = &(rrpge_m_edat->stat.dram[d << 8]);

 for (i = 0U; i < 256U; i++){
  dp[i] = s;
 }

 return 272U;
}



/* Executes a CPU <=> CPU DMA by the contents of the ROPD. May generate a DMA
** halt cause. Returns consumed cycles. */
auint rrpge_m_dma_copy(void)
{
 auint i;
 auint s;
 auint d;
 uint16* sp;
 uint16* dp;

 s = rrpge_m_edat->stat.ropd[0xEC0U];
 d = rrpge_m_edat->stat.ropd[0xEC2U];

 if ( (d >= 0x1C00U) || /* Not user RAM area */
      (s >= 0x1C00U) ){
  rrpge_m_info.hlt |= RRPGE_HLT_DMA;
  return 0U;
 }

 sp = &(rrpge_m_edat->stat.dram[s << 8]);
 dp = &(rrpge_m_edat->stat.dram[d << 8]);

 for (i = 0U; i < 256U; i++){
  dp[i] = sp[i];
 }

 return 528U;
}



/* Executes a CPU <=> VRAM DMA by the contents of the ROPD. May generate a DMA
** or a GRAPHICS halt cause. Returns consumed cycles. */
auint rrpge_m_dma_vram(void)
{
 auint i;
 auint c;
 auint v;
 uint16* cp;
 uint32* vp;

 c = rrpge_m_edat->stat.ropd[0xEC0U];
 v = rrpge_m_edat->stat.ropd[0xEC3U];

 if (c >= 0x1C00U){ /* Not user RAM area */
  rrpge_m_info.hlt |= RRPGE_HLT_DMA;
  return 0U;
 }
 if ((rrpge_m_edat->stat.ropd[0xEC5U] & 1U) != 0U){
  rrpge_m_info.hlt |= RRPGE_HLT_DMA | RRPGE_HLT_GRAPHICS; /* Graphics FIFO not empty */
  return 0U;
 }

 cp = &(rrpge_m_edat->stat.dram[c << 8]);
 vp = &(rrpge_m_edat->stat.vram[(v & 0x7FFU) << 7]);

 if ((v & 0x8000U) == 0U){    /* CPU => VRAM */
  for (i = 0U; i < 128U; i++){
   vp[i] = ((auint)(cp[(i << 1) + 0U]) << 16) | (auint)(cp[(i << 1) + 1U]);
  }
 }else{                       /* VRAM => CPU */
  for (i = 0U; i < 128U; i++){
   cp[(i << 1) + 0U] = (uint16)(vp[i] >> 16);
   cp[(i << 1) + 1U] = (uint16)(vp[i]);
  }
 }

 return 272U;
}
