/**
**  \file
**  \brief     Peripheral RAM interface.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.26
**
**
**  Realizes the Peripheral RAM interface: the 4 pointers.
*/


#ifndef RRPGE_M_PRAM_H
#define RRPGE_M_PRAM_H


#include "rgm_info.h"


/* Operates the memory mapped Peripheral RAM interface for Reads. Only the
** low 5 bits of the address are used. Generates Peripheral bus stalls if
** necessary. */
RRPGE_M_FASTCALL auint rrpge_m_pramread(auint adr, auint rmw);


/* Operates the memory mapped Peripheral RAM interface for Writes. Only the
** low 5 bits of the address are used. Generates Peripheral bus stalls if
** necessary. Note that it assumes a Read (rrpge_m_pramread() call) happened
** before. */
RRPGE_M_FASTCALL void  rrpge_m_pramwrite(auint adr, auint val);


#endif
