/**
**  \file
**  \brief     Callback related components, default callbacks.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#ifndef RRPGE_M_CB_H
#define RRPGE_M_CB_H


#include "rgm_info.h"
#include "rrpge_cb.h"



/* Processes an rrpge_cbpack_t structure into the given emulator object for
** servicing callbacks. Entries not supplied in the structure will get the
** default empty callbacks. */
void rrpge_m_cb_process(rrpge_object_t* obj, rrpge_cbpack_t const* cbp);



#endif
