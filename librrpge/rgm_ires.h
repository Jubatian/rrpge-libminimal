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


/* Initializes library specific portions of the emulation instance, also used
** when loading state. */
void rrpge_m_ires_initl(rrpge_object_t* obj);


/* Initializes starting resources for an RRPGE emulator object. This includes
** areas of the application state and data memory areas. It does not depend on
** the application loaded or to be loaded. */
void rrpge_m_ires_init(rrpge_object_t* obj);


#endif
