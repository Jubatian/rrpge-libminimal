/**
**  \file
**  \brief     Display generator.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.13
**
**
**  Generates the graphics, outputting lines as required if the rendering of
**  graphics is enabled. Also implements rrpge_enarender().
*/


#ifndef RRPGE_M_VID_H
#define RRPGE_M_VID_H


#include "rgm_info.h"
#include "rrpge.h"


/* Initializes Video (GDG) emulation adding the appropriate handlers to the
** state manager. */
void  rrpge_m_vid_init(void);


/* Initializes Video (GDG) emulation internal resources */
void  rrpge_m_vid_initres(rrpge_object_t* hnd);


/* Based on the cycles needing emulation, process the video, and the Graphics
** Display Generator's Display List clear function. Also calls back the line
** renderer as needed. */
void  rrpge_m_vid_proc(rrpge_object_t* hnd, auint cy);


/* Requests current status from the Status register, that is, whether a frame
** is waiting for completion (the Graphics FIFO uses it to suspend) */
auint rrpge_m_vid_getstat(rrpge_object_t* hnd);


/* Requests remaining cycles from the current Video line. Used for generating
** bursts of emulation passes (line-oriented emulation) */
auint rrpge_m_vid_getremcy(rrpge_object_t* hnd);


#endif
