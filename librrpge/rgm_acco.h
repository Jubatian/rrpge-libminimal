/**
**  \file
**  \brief     Graphics accelerator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.15
*/


#ifndef RRPGE_M_GROP_H
#define RRPGE_M_GROP_H

#include "rgm_info.h"


/* Executes a Graphic accelerator operation. Returns the number of cycles the
** accelerator operation takes. */
auint rrpge_m_acco(rrpge_object_t* hnd);


#endif
