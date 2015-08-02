/**
**  \file
**  \brief     CPU emulation main
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
*/


#ifndef RRPGE_M_CPU_H
#define RRPGE_M_CPU_H


#include "rgm_info.h"


/* Initializes CPU emulation adding the appropriate handlers to the state
** manager. */
void rrpge_m_cpu_init(void);


/* Run CPU emulation for up to a given amount of cycles using the given mode.
** Running may finish prematurely if hitting a halt cause. Returns the number
** of cycles emulated. The "rmod" parameter is the run mode passed to
** rrpge_run(). */
auint rrpge_m_cpu_run(rrpge_object_t* hnd, auint rmod, auint cymax);


#endif
