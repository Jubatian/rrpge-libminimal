/**
**  \file
**  \brief     LibRRPGE standard header package - debugging related methods
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#ifndef RRPGE_H
#define RRPGE_H


#include "rrpge_tp.h"



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
void rrpge_setbreak(rrpge_object_t* hnd, rrpge_iuint adr);



/**
**  \brief     Queries if a given address is a breakpoint.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   adr   Address to query (only low 16 bits used).
**  \return            Nonzero if a breakpoint was set at the given address.
*/
rrpge_ibool rrpge_isbreak(rrpge_object_t* hnd, rrpge_iuint adr);



/**
**  \brief     Removes a breakpoint.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   adr   Address to remove break from (only low 16 bits used).
*/
void rrpge_rembreak(rrpge_object_t* hnd, rrpge_iuint adr);



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
rrpge_iuint rrpge_getkcallpar(rrpge_object_t* hnd, rrpge_uint16* buf);



#endif
