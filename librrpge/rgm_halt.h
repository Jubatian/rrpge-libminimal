/**
**  \file
**  \brief     Manages halt causes.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
**
**
**  Uses the hlt member of the emulator object. It shouldn't be accessed
**  directly by any other component.
**
**  Note: uses static functions so they may be substituted like macros in the
**  appropriate places.
*/


#ifndef RRPGE_M_HALT_H
#define RRPGE_M_HALT_H


#include "rgm_info.h"


/* Sets a halt cause or a combination */
static void  rrpge_m_halt_set(rrpge_object_t* hnd, auint hlt)    { hnd->hlt |= hlt; }

/* Clears a halt cause or a combination */
static void  rrpge_m_halt_clr(rrpge_object_t* hnd, auint hlt)    { hnd->hlt &= ~hlt; }

/* Gets current set of halt causes */
static auint rrpge_m_halt_get(rrpge_object_t* hnd)               { return hnd->hlt; }

/* Retrieves whether a given halt cause or either member of a combination is
** set. Nonzero if so, zero otherwise. */
static auint rrpge_m_halt_isset(rrpge_object_t* hnd, auint hlt)  { return (((hnd->hlt) & hlt) != 0U); }

/* Retrieves whether any halt cause is set. Nonzero if so, zero otherwise. */
static auint rrpge_m_halt_isany(rrpge_object_t* hnd)             { return ((hnd->hlt) != 0U); }

/* Clears all halt causes */
static void  rrpge_m_halt_clrall(rrpge_object_t* hnd)            { hnd->hlt = 0U; }


#endif
