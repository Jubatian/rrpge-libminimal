/**
**  \file
**  \brief     LibRRPGE standard header package - serialization helpers
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#ifndef RRPGE_SR_H
#define RRPGE_SR_H


#include "rrpge_tp.h"



/**
**  \brief     Byte -> Word page conversion.
**
**  Converts 8192 Big Endian ordered bytes into 4096 words (16 bit units),
**  suitable for loading binary data.
**
**  \param[in]   src   Source data (8192 bytes).
**  \param[out]  dst   Destination data (4096 words).
*/
void rrpge_convpg_b2w(rrpge_uint8 const* src, rrpge_uint16* dst);



/**
**  \brief     Word -> Byte page conversion.
**
**  Converts 4096 words (16 it units) into 8192 Big Endian ordered bytes,
**  suitable for saving data.
**
**  \param[in]   src   Source data (4096 words).
**  \param[out]  dst   Destination data (8192 bytes).
*/
void rrpge_convpg_w2b(rrpge_uint16 const* src, rrpge_uint8* dst);



/**
**  \brief     Serializes state.
**
**  Complements rrpge_exportstate() producing a raw binary representation of
**  the state data which may be dumped to a file for loading later using
**  rrpge_importstate().
**
**  \param[in]   src   State data structure.
**  \param[out]  dst   Raw binary state (must be at least 8192 * 361 bytes).
*/
void rrpge_state2raw(rrpge_state_t const* src, rrpge_uint8* dst);



/**
**  \brief     Extract user data from a Read Only Process Descriptor page.
**
**  \param[in]   src   Read Only Process Descriptor page (4096 elements).
**  \param[out]  dst   Exported user data.
*/
void rrpge_ropd2udata(rrpge_uint16 const* src, rrpge_udata_t* dst);



/**
**  \brief     Write user data into a Read Only Process Descriptor page.
**
**  Writes new user data into a Read Only Process Descriptor page. It does not
**  alter any other field.
**
**  \param[in]   src   Source user data.
**  \param[out]  dst   Read Only Process Descriptor page (4096 elements).
*/
void rrpge_udata2ropd(rrpge_udata_t const* src, rrpge_uint16* dst);



#endif
