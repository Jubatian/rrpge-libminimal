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


#include "render.h"
#include "../host/screen.h"



/* Palette in -RGB format suitable for the display */
static uint32 render_col[256U];



/*
** Line callback service routine.
*/
void render_line(rrpge_object_t* hnd, rrpge_iuint ln, rrpge_uint8 const* buf)
{
 uint32* sln;
 auint   j;

 if (ln  >  399U){ return; }  /* Should not happen */

 sln = screen_lock();
 if (sln == NULL){ return; }  /* Display not available for rendering */

 sln += screen_pitch() * ln;  /* Position to the appropriate line */

 for (j = 0U; j < 640U; j += 8U){
  sln[j + 0U] = render_col[buf[j + 0U]];
  sln[j + 1U] = render_col[buf[j + 1U]];
  sln[j + 2U] = render_col[buf[j + 2U]];
  sln[j + 3U] = render_col[buf[j + 3U]];
  sln[j + 4U] = render_col[buf[j + 4U]];
  sln[j + 5U] = render_col[buf[j + 5U]];
  sln[j + 6U] = render_col[buf[j + 6U]];
  sln[j + 7U] = render_col[buf[j + 7U]];
 }

 screen_unlock();

 if (ln == 399U){             /* Last line - update display too */
  screen_update(0, 0, 640, 400);
 }
}



/*
** Internal: convert palette entry
** Treats the RRPGE source as an 5R-5G-1x-5B color
*/
static auint render_palconv(auint scol)
{
 return ((scol & 0x0F00U) << 12) |
        ((scol & 0x0F00U) <<  8) |
        ((scol & 0x00F0U) <<  8) |
        ((scol & 0x00F0U) <<  4) |
        ((scol & 0x000FU) <<  4) |
        ((scol & 0x000FU)      );
}



/*
** Palette callback service routine.
*/
void render_pal(rrpge_object_t* hnd, const void* par)
{
 rrpge_cbp_setpal_t const* col = (rrpge_cbp_setpal_t const*)(par);
 render_col[(col->id) & 0xFFU] = render_palconv(col->col);
}



/*
** Initializes or resets rendering subsystem by the given emulator object.
** This sets the initial palette.
*/
void render_reset(rrpge_object_t* hnd)
{
 auint i;

 for (i = 0U; i < 256U; i++){
  render_col[i] = render_palconv(rrpge_getpalentry(hnd, i));
 }
}
