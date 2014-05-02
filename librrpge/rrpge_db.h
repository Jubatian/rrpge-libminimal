/**
**  \file
**  \brief     LibRRPGE standard header package - debugging related methods
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#ifndef RRPGE_H
#define RRPGE_H


#include "rrpge_tp.h"



/**
**  \brief     Uploads a page into the emulator.
**
**  Uploads a page (4096 words) into the emulator. All the pages except one
**  are ordinary memory pages (even the Code ROM) without any content checks.
**  The exception is the ROPD (Read Only Process Descriptor) containing the
**  application header as well. Uploading this page may fail if it is
**  improperly formatted or contains such changes to the current state which
**  the library does not support. Moreover it might trigger events when the
**  emulation is started, like starting newly occurred kernel tasks (note that
**  previously running unchanged tasks are not restarted!).
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   pg    The page to upload (see defines in rrpge_tp.h).
**  \param[in]   src   The data to be loaded into the page (4096 elements).
**  \return            0 on success, failure code otherwise.
*/
rrpge_uint32 rrpge_loadpage(rrpge_object_t* hnd, rrpge_uint32 pg, rrpge_uint16 const* src);



/**
**  \brief     Downloads a page from the emulator.
**
**  Downloads a page (4096 words) from the emulator. This always succeeds if
**  the page selection is valid.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   pg    The page to download (see defines in rrpge_tp.h).
**  \param[out]  dst   The data area to be filled (4096 elements).
**  \return            0 on success, failure code otherwise.
*/
rrpge_uint32 rrpge_savepage(rrpge_object_t* hnd, rrpge_uint32 pg, rrpge_uint16* dst);



/**
**  \brief     Sets a breakpoint.
**
**  Sets a breakpoint at the given Code ROM address. The emulation will halt
**  with an RRPGE_HLT_BREAK cause before executing such an instruction.
**  Calling rrpge_run() again will step over this breakpoint continuing the
**  emulation. Any number of breakpoints may be set. The library might only
**  respect these if they are first words of an opcode.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   adr   Address to set as breakpoint (only low 16 bits used).
*/
void rrpge_setbreak(rrpge_object_t* hnd, rrpge_uint32 adr);



/**
**  \brief     Queries if a given address is a breakpoint.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   adr   Address to query (only low 16 bits used).
**  \return            Nonzero if a breakpoint was set at the given address.
*/
rrpge_uint32 rrpge_isbreak(rrpge_object_t* hnd, rrpge_uint32 adr);



/**
**  \brief     Removes a breakpoint.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   adr   Address to remove break from (only low 16 bits used).
*/
void rrpge_rembreak(rrpge_object_t* hnd, rrpge_uint32 adr);



/**
**  \brief     Requests kernel call parameters after a kernel call.
**
**  If the current halt cause is an invalid kernel call (RRPGE_HLT_INVKCALL),
**  then this method can be used to check the kernel call & parameters which
**  failed. This may be used to report these parameters or to emulate extra
**  kernel calls by the host. Returns zero if not after an invalid kernel
**  call.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[out]  buf   Up to 16 elements: the parameters of the kernel call.
**  \return            Number of elements the kernel call has or 0.
*/
rrpge_uint32 rrpge_getkcallpar(rrpge_object_t* hnd, rrpge_uint16* buf);



#endif
