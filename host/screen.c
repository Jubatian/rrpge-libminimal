/**
**  \file
**  \brief     SDL screen handling module
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
**
**
** Set up a 32bit 640x480 mode, lock, unlock, and update routines.
*/


#include "screen.h"
#include <SDL/SDL.h>



SDL_Surface*  screen_memory; /* The memory on which drawing is done */
SDL_Surface*  screen_video;  /* The actual display surface */



auint screen_set()
{
 auint i = 32U;

 /* Try to set up the mode in the highest possible bit depth
 ** (No problem if not 32 as SDL can do the job when copiing the software
 ** surface */
 while(i > 0){
  screen_video = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, i, SDL_HWSURFACE);
  if (screen_video != NULL) break; /* OK! Proper mode found, halt here! */
  i -= 8U;
 }
 if (i == 0) return 1U;
 /* If i could reach 0, it means nothing could be set up... Trouble! */

 /* Allocate the software surface in traditional 0RGB format */
 screen_memory = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32U, 0xFF0000U, 0xFF00U, 0xFFU, 0U);
 if (screen_memory == NULL){ /* Problem! */
  SDL_FreeSurface(screen_video);
  return 1U;
 }

 SDL_ShowCursor(SDL_DISABLE); /* No need of mouse cursor */

 return 0; /* Everything went fine! */
}



void screen_free()
{
 SDL_FreeSurface(screen_video);
 SDL_FreeSurface(screen_memory);
}



uint32* screen_lock()
{
 if (SDL_MUSTLOCK(screen_memory)){
  if (SDL_LockSurface(screen_memory) < 0) return NULL;
 }
 /* Do the lock if it is necessary on the surface */
 return (uint32*)(screen_memory->pixels);
}



auint screen_pitch()
{
 return ((auint)(screen_memory->pitch)) >> 2;
}



void screen_unlock()
{
 if (SDL_MUSTLOCK(screen_memory)) SDL_UnlockSurface(screen_memory);
}



void screen_update(asint x, asint y, asint w, asint h)
{
 SDL_Rect urect;
 /* Do value - checks, and fixes. SDL seems to not check them */
 if (x < 0){ w += x; x=0; }
 if (y < 0){ h += y; y=0; }
 if (x + w > SCREEN_WIDTH)  w = SCREEN_WIDTH - x;
 if (y + h > SCREEN_HEIGHT) h = SCREEN_HEIGHT - y;
 if ((w <= 0) || (h <= 0)) return;
 /* They are OK, it can go */
 urect.x = x;
 urect.y = y;
 urect.w = w;
 urect.h = h;
 SDL_BlitSurface(screen_memory, &urect, screen_video, &urect);
 SDL_UpdateRect(screen_video, x, y, w, h);
}
