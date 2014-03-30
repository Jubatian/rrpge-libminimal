/**
**  \file
**  \brief     File load / save functionality.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#ifndef FILELS_H
#define FILELS_H


#include "types.h"
#include <stdio.h>



/* Loads a page (8192 bytes) form the given file. The target is always filled,
** if the file can not be read, then with zeroes. Returns the number of bytes
** read from the file (0-8192), the rest of the page is zero filled. */
auint  filels_readpage(FILE* f, auint pg, uint8* buf);


#endif
