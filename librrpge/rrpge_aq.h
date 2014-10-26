/**
**  \file
**  \brief     LibRRPGE standard header package - application query helpers
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.26
*/


#ifndef RRPGE_AQ_H
#define RRPGE_AQ_H


#include "rrpge_tp.h"



/**
**  \brief     Get network availability status.
**
**  Extracts the network availability bit from the state. Returns zero if the
**  user should not be available for new connections, nonzero (one) otherwise.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            Nonzero if user is available for new connections.
*/
rrpge_uint32 rrpge_getnetavail(rrpge_object_t* hnd);



/**
**  \brief     Get device exceptation for a device ID.
**
**  Extracts the device type exceptation for a given device ID from the state.
**  The return is zero if the application does not except a device at the
**  given ID (so the ID can be reused for any device), otherwise the return
**  accords to the last return of 0x0410: "Get device properties" kernel call
**  for the given ID.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   dev   The device ID to query.
**  \return            The device type excepted at the given ID.
*/
rrpge_uint32 rrpge_getlastdev(rrpge_object_t* hnd, rrpge_uint32 dev);



/**
**  \brief     Get allowed device types.
**
**  Returns the device types allowed by the application as found in the
**  application descriptor. Returns 16 bits, the bit's index representing the
**  device type, if the bit set, the device being allowed.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            The device types allowed by the application.
*/
rrpge_uint32 rrpge_getalloweddevs(rrpge_object_t* hnd);



/**
**  \brief     Get current video mode.
**
**  Returns current video mode. 0: 640x400, 4bit; 1: 320x400, 8bit. Double
**  scanning is handled by the emulation library, so those are not returned
**  (640x200 returns 0, 320x200 returns 1).
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            Video mode.
*/
rrpge_uint32 rrpge_getvidmode(rrpge_object_t* hnd);



/**
**  \brief     Get stereoscopic 3D output properties.
**
**  Returns current stereoscopic 3D output properties, indicating whether the
**  application is generating such content, and if so, the content's
**  dimensions. Bit 0 of the return value is set if the application is
**  currently generating stereoscopic 3D. Bits 1-2 indicate the vertical used
**  pixels for this content: 0: 400 pixels, 1: 320 pixels, 2: 240 pixels, 3:
**  200 pixels. The stereoscopic 3D content is for cross-eye (left half of the
**  output image for the right eye, right half of the output image for the
**  left eye), vertically centered.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            Stereoscopic 3D output properties.
*/
rrpge_uint32 rrpge_getst3dprops(rrpge_object_t* hnd);



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
