/**
**  \file
**  \brief     Audio buffer pointer calculation
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#ifndef RRPGE_M_ABUF_H
#define RRPGE_M_ABUF_H


#include "rgm_info.h"


/* Calculates audio buffer pointers from the given configuration word (use
** ROPD: 0xBC2), and given half (0: first half; 1: second half; 0xD56 in
** ROPD; only uses bit 0 of this parameter!). Returns left and right sample
** offsets. */
void rrpge_m_abuf_getptr(auint cfg, auint hlf, auint* lpt, auint* rpt);


#endif
