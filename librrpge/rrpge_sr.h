/**
**  \file
**  \brief     LibRRPGE standard header package - serialization helpers
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#ifndef RRPGE_SR_H
#define RRPGE_SR_H


#include "rrpge_tp.h"



/**
**  \brief     Byte -> Word conversion.
**
**  Converts Big Endian ordered bytes into words (16 bit units), suitable for
**  loading binary data.
**
**  \param[in]   src   Source data (bct bytes).
**  \param[out]  dst   Destination data (((bct + 1) >> 1) words).
**  \param[in]   bct   Count of bytes to process.
*/
void rrpge_conv_b2w(rrpge_uint8 const* src, rrpge_uint16* dst, rrpge_iuint bct);



/**
**  \brief     Word -> Byte conversion.
**
**  Converts words (16 it units) into Big Endian ordered bytes, suitable for
**  saving data.
**
**  \param[in]   src   Source data (((bct + 1) >> 1) words).
**  \param[out]  dst   Destination data (bct bytes).
**  \param[in]   bct   Count of bytes to process.
*/
void rrpge_conv_w2b(rrpge_uint16 const* src, rrpge_uint8* dst, rrpge_iuint bct);



/**
**  \brief     Serializes state.
**
**  Complements rrpge_peekstate() producing a raw binary representation of the
**  state data which may be dumped to a file for loading later using
**  rrpge_detachstate().
**
**  \param[in]   src   Emulator state.
**  \param[out]  dst   Raw binary state (must be at least 4290 * 1024 bytes).
*/
void rrpge_state2raw(rrpge_state_t const* src, rrpge_uint8* dst);



/**
**  \brief     Deserializes state.
**
**  Deserializes raw state filling a state data structure. To load into an
**  emulator instance, do an rrpge_detachstate() first, then use this function
**  to fill the state up.
**
**  \param[in]   src   Raw binary state (must be at least 4290 * 1024 bytes).
**  \param[out]  dst   Emulator state.
*/
void rrpge_raw2state(rrpge_uint8 const * src, rrpge_state_t* dst);



#endif
