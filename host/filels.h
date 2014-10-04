/**
**  \file
**  \brief     File load / save functionality.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.10.04
*/


#ifndef FILELS_H
#define FILELS_H


#include "types.h"
#include <stdio.h>



/* Loads an area form the given file. The target is always filled, if the file
** can not be read, then with zeroes. Returns the number of bytes read from
** the file, the rest of the area is zero filled. */
auint  filels_read(FILE* f, auint off, auint len, uint8* buf);


#endif
