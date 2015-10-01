/**
**  \file
**  \brief     Mixer emulation main
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.15
*/


#ifndef RRPGE_M_MIX_H
#define RRPGE_M_MIX_H


#include "rgm_info.h"


/* Initializes mixer emulation adding the appropriate handlers to the state
** manager. */
void rrpge_m_mix_init(void);


/* Execute mixer operation, whatever is in the current set of mixer
** registers. Returns number of cycles the operation takes. */
auint rrpge_m_mix_op(rrpge_object_t* hnd);


#endif
