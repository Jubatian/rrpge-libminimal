/**
**  \file
**  \brief     Display generator.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.23
**
**
**  Generates the graphics, outputting lines as required if the rendering of
**  graphics is enabled.
*/


#ifndef RRPGE_M_VID_H
#define RRPGE_M_VID_H


#include "rgm_info.h"


/* Based on the cycles needing emulation, process the video. Also calls back
** the line renderer as needed. */
void  rrpge_m_vidproc(auint cy);


#endif
