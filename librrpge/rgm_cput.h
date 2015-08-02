/**
**  \file
**  \brief     CPU emulation related types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
*/


#ifndef RRPGE_M_CPUT_H
#define RRPGE_M_CPUT_H


#include "rgm_type.h"


/* Addressing unit's function types. The read function's parameter is the
** R-M-W signal: nonzero for the read of a R-M-W access. */
typedef RRPGE_M_FASTCALL auint (rrpge_m_addr_read_t)(rrpge_object_t* hnd, auint rmw);
typedef RRPGE_M_FASTCALL void (rrpge_m_addr_write_t)(rrpge_object_t* hnd, auint val);


/* CPU emulation structure. Components defined here are private to the CPU
** emulation, only used by the rgm_cpu*.c sources. */
typedef struct{

 auint  ocy;         /* Opcode address extra cycles. This is used by the
                     ** addressing mode unit to mark if extra cycles were
                     ** consumed for any reason stalling the CPU. */
 auint  oaw;         /* Opcode address extra word. This is used by the
                     ** addressing mode unit to mark if an extra opcode word
                     ** was consumed. */
 auint  opc;         /* Opcode first word cache / addressing cache. Normally
                     ** the next opcode is loaded in this for faster access, in
                     ** function parameters it is also used for passing the
                     ** first word of the parameter components. */

 auint  xr[8];       /* CPU general registers (A-D, X0-X3) (State: 0x040-0x047) */
 auint  xmb[2];      /* CPU pointer mode/high registers (XM, XB) (State: 0x048-0x049) */
 auint  pc;          /* CPU program counter (State: 0x04A) */
 auint  sp;          /* CPU stack pointer (State: 0x04B) */
 auint  bp;          /* CPU base pointer (State: 0x04C) */

 auint  sbt;         /* Stack bottom, also high bit(s) for bp and sp */
 auint  stp;         /* Stack top */

 auint  ada;         /* Temporary values for the Addressing unit */
 auint  add;
 auint  ads;
 auint  adm;

 rrpge_m_addr_write_t* awf;
                     /* Addressing mode specific write. Must be used with a
                     ** corresponding addressing mode read to complete an
                     ** R-M-W cycle. The read function sets this function
                     ** pointer to the appropriate write function so it can be
                     ** called. Increments rrpge_m_info.ocy if necessary. */

}rrpge_m_cpu_t;


#endif
