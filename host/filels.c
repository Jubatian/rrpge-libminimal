/**
**  \file
**  \brief     File load / save functionality.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.10.04
*/



#include "filels.h"



/* Loads an area form the given file. The target is always filled, if the file
** can not be read, then with zeroes. Returns the number of bytes read from
** the file, the rest of the area is zero filled. */
auint  filels_read(FILE* f, auint off, auint len, uint8* buf)
{
 auint r;

 /* No error handling yet, just attemt to read & forget it */

 fseek(f, off, SEEK_SET);

 r = fread(buf, 1U, len, f);

 if (r < len){
  memset(buf + r, 0U, len - r);
 }

 return r;
}
