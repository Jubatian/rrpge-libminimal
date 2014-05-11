/**
**  \file
**  \brief     LibRRPGE standard header package - application query helpers
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.11
*/


#ifndef RRPGE_AQ_H
#define RRPGE_AQ_H


#include "rrpge_tp.h"



/**
**  \brief     Get network availability status.
**
**  Extracts the network availability bit from the Read Only Process
**  Descriptor. Returns zero if the user should not be available for new
**  connections, nonzero (one) otherwise.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            Nonzero if user is available for new connections.
*/
rrpge_uint32 rrpge_getnetavail(rrpge_object_t* hnd);



/**
**  \brief     Get device exceptation for a device ID.
**
**  Extracts the device type exceptation for a given device ID from the Read
**  Only Process Descriptor. The return is zero if the application does not
**  except a device at the given ID (so the ID can be reused for any device),
**  otherwise the return accords to the last return of 0x0410: "Get device
**  properties" for the given ID.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   dev   The device ID to query.
**  \return            The device type excepted at the given ID.
*/
rrpge_uint32 rrpge_getlastdev(rrpge_object_t* hnd, rrpge_uint32 dev);



/**
**  \brief     Get touch sensitive area definition.
**
**  Extracts a touch sensitive area's bounding box from the Read Only Process
**  Descriptor. Returns zero if the area is not defined (width or height is
**  zero). Otherwise returns nonzero and fills the provided array with the
**  coordinates.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   ari   Area ID to query.
**  \param[out]  cor   Coordinates in x, y, width, height order (4 elements).
**  \return            Nonzero if the area is available.
*/
rrpge_uint32 rrpge_gettoucharea(rrpge_object_t* hnd, rrpge_uint32 ari, rrpge_uint32* cor);



/**
**  \brief     Get current video mode.
**
**  Returns current video mode. 0: 640x400, 4bit; 1: 320x400, 8bit.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            Video mode.
*/
rrpge_uint32 rrpge_getvidmode(rrpge_object_t* hnd);



/**
**  \brief     Get palette entry.
**
**  Returns palette entry for a given color index. Note that there are always
**  256 color indices, even in 4bit mode. The color is in 4-4-4 RGB format on
**  the low 12 bits of the return.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   cid   Color index (0 - 255).
**  \return            Palette entry (color) in 4-4-4 RGB.
*/
rrpge_uint32 rrpge_getpalentry(rrpge_object_t* hnd, rrpge_uint32 cid);



#endif
