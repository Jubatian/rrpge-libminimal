/**
**  \file
**  \brief     Peripheral RAM interface.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
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
RRPGE_M_FASTCALL auint rrpge_m_pramread(rrpge_object_t* hnd, auint adr, auint rmw);


/* Operates the memory mapped Peripheral RAM interface for Writes. Only the
** low 5 bits of the address are used. Generates Peripheral bus stalls if
** necessary. Note that it assumes a Read (rrpge_m_pramread() call) happened
** before. */
RRPGE_M_FASTCALL void  rrpge_m_pramwrite(rrpge_object_t* hnd, auint adr, auint val);


#endif
