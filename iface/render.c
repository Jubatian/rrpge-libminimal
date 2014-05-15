/**
**  \file
**  \brief     Graphics rendering
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.15
**
**
** Graphics rendering: produces the graphics output from the lines provided by
** the emulator. It also provides the palette callback to set the colors. When
** resetting or starting, the palette has to be filled. It also automatically
** updates display when reaching line 399 after outputting it.
*/


#include "render.h"
#include "../host/screen.h"


/* Display mode: 0: 640x480x16; 1: 320x400x256. */
static auint  render_dm = 0U;

/* Palette in -RGB format suitable for the display */
static uint32 render_col[256U];



/*
** Line callback service routine.
*/
void render_line(rrpge_object_t* hnd, rrpge_uint32 ln, rrpge_uint32 const* buf)
{
 uint32* sln;
 uint32  t;
 auint   i;
 auint   j;

 if (ln  >  399U){ return; }  /* Should not happen */

 sln = screen_lock();
 if (sln == NULL){ return; }  /* Display not available for rendering */

 sln += screen_pitch() * ln;  /* Position to the appropriate line */

 if (render_dm == 0U){        /* 16 color mode */

  j = 0U;
  for (i = 0U; i < 80U; i++){
   sln[j + 0U] = render_col[(buf[i] >> 28) & 0xFU];
   sln[j + 1U] = render_col[(buf[i] >> 24) & 0xFU];
   sln[j + 2U] = render_col[(buf[i] >> 20) & 0xFU];
   sln[j + 3U] = render_col[(buf[i] >> 16) & 0xFU];
   sln[j + 4U] = render_col[(buf[i] >> 12) & 0xFU];
   sln[j + 5U] = render_col[(buf[i] >>  8) & 0xFU];
   sln[j + 6U] = render_col[(buf[i] >>  4) & 0xFU];
   sln[j + 7U] = render_col[(buf[i]      ) & 0xFU];
   j += 8U;
  }

 }else{                       /* 256 color mode */

  j = 0U;
  for (i = 0U; i < 80U; i++){
   t = render_col[(buf[i] >> 24) & 0xFFU];
   sln[j + 0U] = t;
   sln[j + 1U] = t;
   t = render_col[(buf[i] >> 16) & 0xFFU];
   sln[j + 2U] = t;
   sln[j + 3U] = t;
   t = render_col[(buf[i] >>  8) & 0xFFU];
   sln[j + 4U] = t;
   sln[j + 5U] = t;
   t = render_col[(buf[i]      ) & 0xFFU];
   sln[j + 6U] = t;
   sln[j + 7U] = t;
   j += 8U;
  }

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
** Mode change callback service routine.
*/
void render_mode(rrpge_object_t* hnd, const void* par)
{
 rrpge_cbp_mode_t const* mod = (rrpge_cbp_mode_t const*)(par);
 render_dm = (mod->mod) & 1U;
}



/*
** Initializes or resets rendering subsystem by the given emulator object.
** This sets the display mode (640x400x16 or 320x400x256) and the initial
** palette.
*/
void render_reset(rrpge_object_t* hnd)
{
 auint i;

 render_dm = rrpge_getvidmode(hnd);

 for (i = 0U; i < 256U; i++){
  render_col[i] = render_palconv(rrpge_getpalentry(hnd, i));
 }
}
