/**
**  \file
**  \brief     Graphics accelerator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.21
*/


#ifndef RRPGE_M_GROP_H
#define RRPGE_M_GROP_H

#include "rgm_info.h"


/* Executes a Graphic accelerator operation. Rebuilds the recolor cache if
** necessary (clearing rrpge_m_info.grr). Returns the number of cycles the
** accelerator operation takes. */
auint rrpge_m_grop_accel(void);


#endif
