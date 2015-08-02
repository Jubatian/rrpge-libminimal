/**
**  \file
**  \brief     LibRRPGE standard header package - debugging related methods
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
*/


#ifndef RRPGE_DB_H
#define RRPGE_DB_H


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
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address to set as breakpoint (only low 16 bits used).
*/
void rrpge_setbreak(rrpge_object_t* hnd, rrpge_iuint adr);



/**
**  \brief     Queries if a given address is a breakpoint.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address to query (only low 16 bits used).
**  \return            Nonzero if a breakpoint was set at the given address.
*/
rrpge_ibool rrpge_isbreak(rrpge_object_t* hnd, rrpge_iuint adr);



/**
**  \brief     Removes a breakpoint.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address to remove break from (only low 16 bits used).
*/
void rrpge_rembreak(rrpge_object_t* hnd, rrpge_iuint adr);



/**
**  \brief     Gets a value from the PRAM.
**
**  Gets a 32 bit value from the Peripheral RAM. If the address is out of
**  range (out of the 1M * 32bits PRAM), then the return value is zero.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address of cell to retrieve.
**  \return            Cell value (32 bits).
*/
rrpge_iuint rrpge_get_pram(rrpge_object_t* hnd, rrpge_iuint adr);



/**
**  \brief     Sets a value in the PRAM.
**
**  Sets a 32 bit value in the Peripheral RAM. If the address is out of range,
**  no effect. The value is truncated to 32 bits (if applicable).
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address of cell to change.
**  \param[in]   val   Value to set.
**  \return            Resulting cell value (32 bits).
*/
rrpge_iuint rrpge_set_pram(rrpge_object_t* hnd, rrpge_iuint adr, rrpge_iuint val);



/**
**  \brief     Gets a value from the DRAM.
**
**  Gets a 16 bit value from the Data RAM. If the address is out of range (out of
**  the 64K * 16bits DRAM), then the return value is zero.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address of cell to retrieve.
**  \return            Cell value (16 bits).
*/
rrpge_iuint rrpge_get_dram(rrpge_object_t* hnd, rrpge_iuint adr);



/**
**  \brief     Sets a value in the DRAM.
**
**  Sets a 16 bit value in the Data RAM. If the address is out of range, no
**  effect. The value is truncated to 16 bits.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address of cell to change.
**  \param[in]   val   Value to set.
**  \return            Resulting cell value (16 bits).
*/
rrpge_iuint rrpge_set_dram(rrpge_object_t* hnd, rrpge_iuint adr, rrpge_iuint val);



/**
**  \brief     Gets a value from the State.
**
**  Gets a 16 bit value from the State. The range 0x040 - 0x3FF is valid, for
**  other regions the return value is zero. The State is populated according
**  to the RRPGE specification.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address of cell to retrieve.
**  \return            Cell value (16 bits).
*/
rrpge_iuint rrpge_get_state(rrpge_object_t* hnd, rrpge_iuint adr);



/**
**  \brief     Sets a value in the State.
**
**  Sets a 16 bit value in the State. If the address is out of range (not
**  within 0x040 - 0x3FF), no effect. The value is truncated to 16 bits, then
**  it may also be limited according to the given State cell's constraints,
**  see the RRPGE specification for details.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address of cell to change.
**  \param[in]   val   Value to set.
**  \return            Resulting cell value (16 bits).
*/
rrpge_iuint rrpge_set_state(rrpge_object_t* hnd, rrpge_iuint adr, rrpge_iuint val);



/**
**  \brief     Gets a value from the Stack.
**
**  Gets a 16 bit value from the Stack. If the address is out of range, then
**  the return value is zero. The Stack range is interpreted as the
**  application requests it (it may be within the Data RAM or outside it,
**  address 0 always referring the stack bottom).
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address of cell to retrieve.
**  \return            Cell value (16 bits).
*/
rrpge_iuint rrpge_get_stack(rrpge_object_t* hnd, rrpge_iuint adr);



/**
**  \brief     Sets a value in the Stack.
**
**  Sets a 16 bit value in the Stack. If the address is out of range, no
**  effect. The value is truncated to 16 bits.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   adr   Address of cell to change.
**  \param[in]   val   Value to set.
**  \return            Resulting cell value (16 bits).
*/
rrpge_iuint rrpge_set_stack(rrpge_object_t* hnd, rrpge_iuint adr, rrpge_iuint val);



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
