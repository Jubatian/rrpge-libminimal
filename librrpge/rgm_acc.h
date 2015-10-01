/**
**  \file
**  \brief     Graphic Accelerator emulation main
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.15
*/


#ifndef RRPGE_M_ACC_H
#define RRPGE_M_ACC_H


#include "rgm_info.h"


/* Initializes accelerator emulation adding the appropriate handlers to the
** state manager. */
void rrpge_m_acc_init(void);


/* Execute accelerator operation, whatever is in the current set of
** accelerator registers. Returns number of cycles the operation takes. */
auint rrpge_m_acc_op(rrpge_object_t* hnd);


#endif
