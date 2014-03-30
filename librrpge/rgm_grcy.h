/**
**  \file
**  \brief     Graphics timing calculator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#ifndef RRPGE_M_GRCY_H
#define RRPGE_M_GRCY_H

#include "rgm_info.h"


/* Calculates stall mode (number of layers enabled) for a given line by the
** current display list. Returns the number of layers displayed on the given
** line. */
auint rrpge_m_grlsm(auint lin);

/* Calculates cycles taken by an accelerator function according to the current
** display list contents. Stores the result in rrpge_m_info.vac. */
void rrpge_m_graccy(void);


#endif
