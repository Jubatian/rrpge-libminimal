/**
**  \file
**  \brief     Kernel functionality
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#ifndef RRPGE_M_KRNM_H
#define RRPGE_M_KRNM_H

#include "rgm_info.h"


/* Processes a supervisor call. The first element of the parameter list
** selects the call. The number of parameters are given in the second
** parameter: the array must be at least this long. The function sets the
** appropriate halt cause if necessary (as defined in the host callbacks of
** the RRPGE library interface). Returns the number of cycles which were
** necessary for performing the call. */
auint rrpge_m_kcall(uint16 const* par, auint n);


#endif
