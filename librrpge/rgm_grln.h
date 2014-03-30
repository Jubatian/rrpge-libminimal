/**
**  \file
**  \brief     Graphics line renderer
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#ifndef RRPGE_M_GRLN_H
#define RRPGE_M_GRLN_H

#include "rgm_info.h"


/* Calculates display list pointers into frep[] of rrpge_m_edat. Increments
** the line counter afterwards, also working through VBlank into the next
** frame. Transfers render request (rena) flag on frame end and sets frame
** end halt cause. Sets the frld flag of rrpge_m_edat in display lines,
** clears it in VBlank lines. */
void rrpge_m_grpr(void);

/* Renders current graphics line. This must be called after an rrpge_m_grpr()
** call. Clears the frld flag of rrpge_m_edat. Also performs callback to
** host. */
void rrpge_m_grln(void);


#endif
