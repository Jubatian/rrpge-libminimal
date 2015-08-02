/**
**  \file
**  \brief     Kernel functionality
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
*/


#ifndef RRPGE_M_KRNM_H
#define RRPGE_M_KRNM_H

#include "rgm_info.h"


/* Processes a supervisor call. The first element of the parameter list
** selects the call. The number of parameters are given in the second
** parameter: the array must be at least this long. The function sets the
** appropriate halt cause if necessary (as defined in the host callbacks of
** the RRPGE library interface). Returns the number of cycles which were
** necessary for performing the call. The return value is produced in
** resh:resl (16 bits each), the original value retained if the specification
** does not specify a return. */
auint rrpge_m_kcall(uint16 const* par, auint n, auint* resh, auint* resl);


#endif
