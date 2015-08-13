/**
**  \file
**  \brief     Video output (Graphics Display Generator) types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.13
*/


#ifndef RRPGE_M_VIDT_H
#define RRPGE_M_VIDT_H


#include "rgm_type.h"


/* Video output (Graphics Display Generator) structure. Components defined
** here are private to the Video output emulation, only used by the
** rgm_vid*.c sources. */
typedef struct{

 uint32 cbuf[3][128];    /* Clipping buffers */

 auint sdef[8];          /* Source definitions (0x0018 - 0x001F) */
 auint ckey[2];          /* Colorkey registers (0x0010 - 0x0011) */
 auint smrr[2];          /* Shift mode region registers (0x0014 - 0x0015) */
 auint dscn;             /* Double scan split register (0x0012) */
 auint dlcl;             /* Display list clear register (0x0013) */
 auint dldf;             /* Display list definition register (0x0016) */
 auint stat;             /* Status register (0x0017) */

 auint cini[3];          /* Clipping buffer initializators */

 auint dlat;             /* Display list definition latch (State: 0x055) */

 auint vln;              /* Video line count (State: 0x050) */
 auint vlc;              /* Video remaining cycles within line (State: 0x051) */

 auint rena;             /* Render Enable flags.
                         ** bit0: Requested state
                         ** bit1: Current state
                         ** The current state copies the requested state when
                         ** passing frame boundary. */

}rrpge_m_vid_t;


#endif
