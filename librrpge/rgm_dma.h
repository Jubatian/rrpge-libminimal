/**
**  \file
**  \brief     DMA functions
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.26
**
**
**  Implementations of the CPU RAM DMA's and the Video RAM <=> CPU RAM DMA.
*/


#ifndef RRPGE_M_DMA_H
#define RRPGE_M_DMA_H

#include "rgm_info.h"


/* Executes a CPU Fill DMA by the contents of the ROPD. May generate a DMA
** halt cause. Returns consumed cycles. */
auint rrpge_m_dma_fill(void);

/* Executes a CPU <=> CPU DMA by the contents of the ROPD. May generate a DMA
** halt cause. Returns consumed cycles. */
auint rrpge_m_dma_copy(void);

/* Executes a CPU <=> VRAM DMA by the contents of the ROPD. May generate a DMA
** or a GRAPHICS halt cause. Returns consumed cycles. */
auint rrpge_m_dma_vram(void);


#endif
