/**
**  \file
**  \brief     Mixer DMA peripheral
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.21
*/


#ifndef RRPGE_M_MIX_H
#define RRPGE_M_MIX_H

#include "rgm_info.h"


/* Performs a Mixer DMA operation using the parameters in the application
** state (area RRPGE_STA_MIXER). Returns the number of cycles the operation
** takes. */
auint rrpge_m_mixerop(void);


#endif
