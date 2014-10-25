/**
**  \file
**  \brief     Initialization resources & preparation
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
*/


#ifndef RRPGE_M_IRES_H
#define RRPGE_M_IRES_H

#include "rgm_info.h"


/* Initializes the CPU data memory into the 'dini' member of the object. This
** should be called before loading the application binary so data memory
** portions not loaded from the binary are adequately initialized */
void rrpge_m_ires_initdata(rrpge_object_t* obj);


/* Initializes state after the application header and descriptor were loaded.
** This should be used in initialization, so the complete header may be
** checked at once. */
void rrpge_m_ires_initstat(rrpge_object_t* obj);


/* Initializes starting resources for an RRPGE emulator object after an
** application was loaded. This should be used before starting emulation or
** when resetting it. */
void rrpge_m_ires_init(rrpge_object_t* obj);


#endif
