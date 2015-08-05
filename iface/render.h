/**
**  \file
**  \brief     Graphics rendering
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.05
**
**
** Graphics rendering: produces the graphics output from the lines provided by
** the emulator. It also provides the palette callback to set the colors. When
** resetting or starting, the palette has to be filled. It also automatically
** updates display when reaching line 399 after outputting it.
*/


#ifndef RENDER_H
#define RENDER_H


#include "../host/types.h"
#include "../librrpge/rrpge.h"



/*
** Line callback service routine.
*/
void render_line(rrpge_object_t* hnd, rrpge_iuint ln, rrpge_uint8 const* buf);

/*
** Palette callback service routine.
*/
void render_pal(rrpge_object_t* hnd, const void* par);

/*
** Initializes or resets rendering subsystem by the given emulator object.
** This sets the initial palette.
*/
void render_reset(rrpge_object_t* hnd);


#endif
