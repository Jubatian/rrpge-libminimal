/**
**  \file
**  \brief     Peripheral RAM interface.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.03
**
**
**  Realizes the Peripheral RAM interface: the 4 pointers. Note that it is
**  necessary to access the PRAM through special routines since it depends on
**  a read-modify-write signal (so using the state directly is not possible).
*/


#ifndef RRPGE_M_PRAM_H
#define RRPGE_M_PRAM_H


#include "rgm_info.h"


/* Initializes PRAM emulation adding the appropriate handlers to the state
** manager. */
void rrpge_m_pram_init(void);


/* Operates the memory mapped Peripheral RAM interface for Reads. Only the
** low 5 bits of the address are used. Generates Peripheral bus stalls if
** necessary. */
RRPGE_M_FASTCALL auint rrpge_m_pram_read(rrpge_object_t* hnd, auint adr, auint rmw);


/* Operates the memory mapped Peripheral RAM interface for Writes. Only the
** low 5 bits of the address are used. Generates Peripheral bus stalls if
** necessary. Note that it assumes a Read (rrpge_m_pramread() call) happened
** before. */
RRPGE_M_FASTCALL void  rrpge_m_pram_write(rrpge_object_t* hnd, auint adr, auint val);


/* Peripheral bus: Add stall cycles */
static void rrpge_m_pram_cys_add(rrpge_object_t* hnd, auint cy)       { hnd->prm.cys += cy; }

/* Peripheral bus: Consume stall cycles, returns remaining cycles of cy */
auint rrpge_m_pram_cys_cons(rrpge_object_t* hnd, auint cy);

/* Peripheral bus: Clear all stall cycles */
void  rrpge_m_pram_cys_clr(rrpge_object_t* hnd);


#endif
