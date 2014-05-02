/**
**  \file
**  \brief     CPU addressing unit
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#ifndef RRPGE_M_CPUA_H
#define RRPGE_M_CPUA_H


#include "rgm_info.h"

/* Read function type for the read func. table */
typedef RRPGE_M_FASTCALL auint (rrpge_m_addr_read_t)(void);

/* Addressing mode specific read, function table by opcode bits 0-5. Uses
** rrpge_m_info.opc for the addressing mode further on, assumes PC points here
** too for fetching the second opcode word as needed. Sets rrpge_m_info.ocy
** and rrpge_m_info.oaw (this latter to 1 or 2) depending on the requirements
** of the addressing operation. Only low 16 bits of the return value may be
** set. The result is returned in rrpge_m_info.rwt. */
extern rrpge_m_addr_read_t* const rrpge_m_addr_read_table[64];

/* Function type of an addressing mode specific write. */
typedef RRPGE_M_FASTCALL void (rrpge_m_addr_write_t)(auint);

/* Addressing mode specific write. Must be used with a corresponding
** rrpgem_addr_read() to complete an R-M-W cycle. The read function sets this
** function pointer to the appropriate write function so it can be called.
** Increments rrpge_m_info.ocy if necessary. The value to be written is taken
** from rrpge_m_info.rwt, only low 16 bits are used. */
extern rrpge_m_addr_write_t* rrpge_m_addr_write;

/* Pops off a value from the stack, sets halt cause if stack pointer is out of
** bounds (low). Returning to supervisor must be handled externally (this
** won't do that). Only low 16 bits of the return value may be set. */
RRPGE_M_FASTCALL auint rrpge_m_stk_pop(void);

/* Pushes a value on the stack, sets halt cause if stack pointer is out of
** bounds (high). */
RRPGE_M_FASTCALL void  rrpge_m_stk_push(auint val);


#endif
