/**
**  \file
**  \brief     Graphics FIFO and display manager.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.29
**
**
**  Contains the Graphics FIFO's logic, and manages the graphics display,
**  processing lines, calling the appropriate components. The Graphics FIFO
**  and the display generation so processes in line sync with each other, as
**  required by the specification.
*/


#ifndef RRPGE_M_VID_H
#define RRPGE_M_VID_H


#include "rgm_info.h"


/* Based on the cycles needing emulation, process the video. Also calls back
** the line renderer as needed. */
void  rrpge_m_vidproc(auint cy);


/* Triggers a Graphics FIFO start. */
void  rrpge_m_vidfifost(void);


/* Performs a Graphics FIFO store using the parameters in the ROPD. It may
** flag a FIFO start if necessary. Returns number of cycles the store takes */
auint rrpge_m_vidfifoop(void);


#endif
