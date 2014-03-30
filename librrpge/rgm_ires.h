/**
**  \file
**  \brief     Initialization resources & preparation
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#ifndef RRPGE_M_IRES_H
#define RRPGE_M_IRES_H

#include "rgm_info.h"


/* Initializes graphics peripheral as required after arbitrary file loads and
** saves (and also as required when resetting). This includes the peripheral
** area, zeroing the first 32 video RAM pages, and resetting all cycle
** counters & display list data (0xD50 - 0xD5F area). */
void rrpge_m_ires_initg(rrpge_object_t* obj);


/* Initializes library specific portions of the emulation instance, also used
** when loading state. */
void rrpge_m_ires_initl(rrpge_object_t* obj);


/* Initializes starting resources for an RRPGE emulator object. This includes
** areas of the ROPD and data memory areas. It does not depend on the
** application loaded or to be loaded. Keeps user area of ROPD intact. */
void rrpge_m_ires_init(rrpge_object_t* obj);


#endif
