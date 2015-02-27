/**
**  \file
**  \brief     RRPGE User Library binary
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.02.27
**
**
**  The User Library included is taken from the RRPGE User Library project.
*/


#ifndef RRPGE_M_ULIB_H
#define RRPGE_M_ULIB_H

#include "rgm_info.h"


/* Size of the RRPGE User Library */
#define RRPGE_M_ULIB_SIZE (6708U / 2U)

/* The RRPGE User Library */
extern const uint16 rrpge_m_ulib[RRPGE_M_ULIB_SIZE];

/* Count of elements in the 0xFA00 - 0xFAFF nonzero data initializers */
#define RRPGE_M_ULIB_ICNT 43U

/* 0xFA00 - 0xFAFF initializers. First byte is offset low, then high & low of data */
extern const uint8  rrpge_m_ulib_idat[RRPGE_M_ULIB_ICNT * 3U];


#endif
