/**
**  \file
**  \brief     Graphics timing calculator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.26
*/


#ifndef RRPGE_M_GRCY_H
#define RRPGE_M_GRCY_H

#include "rgm_info.h"


/* Calculates cycles taken by an accelerator function according to the current
** accelerator register contents. Call it before actually executing the
** operation. */
auint rrpge_m_graccy(void);


#endif
