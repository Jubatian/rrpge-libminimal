/**
**  \file
**  \brief     CPU addressing unit
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.24
*/


#ifndef RRPGE_M_CPUA_H
#define RRPGE_M_CPUA_H


#include "rgm_info.h"


/* Addressing mode specific read, function table by opcode bits 0-5. Uses
** rrpge_m_info.opc for the addressing mode further on, assumes PC points here
** too for fetching the second opcode word as needed. Sets rrpge_m_info.ocy
** and rrpge_m_info.oaw (this latter to 1 or 2) depending on the requirements
** of the addressing operation. Only low 16 bits of the return value may be
** set. The corresponding write is to be called by rrpge_m_info.awf. */
extern rrpge_m_addr_read_t* const rrpge_m_addr_read_table[64];


/* Pops off a value from the stack, sets halt cause if stack pointer is out of
** bounds (low). Returning to supervisor must be handled externally (this
** won't do that). Only low 16 bits of the return value may be set. */
RRPGE_M_FASTCALL auint rrpge_m_stk_pop(void);


/* Pushes a value on the stack, sets halt cause if stack pointer is out of
** bounds (high). */
RRPGE_M_FASTCALL void  rrpge_m_stk_push(auint val);


#endif
