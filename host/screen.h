/**
**  \file
**  \brief     SDL screen handling functions
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
*/


#ifndef SCREEN_H
#define SCREEN_H


#include "types.h"


#define SCREEN_WIDTH   640U
#define SCREEN_HEIGHT  400U



/* Sets up the screen to be a 640x480 mode with 32bit colors
** Returns 0 on success, 1 on failure */
auint   screen_set();

/* Cleans up the previously set up screen mode */
void    screen_free();

/* Locks the screen for drawing. Returns a pointer to the screen buffer,
** or NULL on failure */
uint32* screen_lock();

/* Returns the virtual width of the screen (This may be more than the defined
** screen width). Necessary for appropriate drawing! */
auint   screen_pitch();

/* Releases the previously set lock. */
void    screen_unlock();

/* Updates a region of the screen (makes the contents visible) */
void    screen_update(asint x, asint y, asint w, asint h);


#endif
