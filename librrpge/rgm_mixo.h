/**
**  \file
**  \brief     Mixer DMA peripheral
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.15
*/


#ifndef RRPGE_M_MIXO_H
#define RRPGE_M_MIXO_H

#include "rgm_info.h"


/* Initializes Mixer DMA, populating some accelerator tables. */
void  rrpge_m_mixo_init(void);


/* Performs a Mixer DMA operation using the parameters in the application
** state (area RRPGE_STA_MIXER). Returns the number of cycles the operation
** takes. */
auint rrpge_m_mixo(rrpge_object_t* hnd);


#endif
