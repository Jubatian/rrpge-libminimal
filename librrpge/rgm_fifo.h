/**
**  \file
**  \brief     FIFO (Accelerator, Mixer) related operations.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.23
**
**
**  Contains the Graphics & Mixer FIFO's logic. The FIFOs operate detached
**  from the graphics & audio emulation as such precision is not required,
**  however they necessarily contain the logic to interact with the
**  Accelerator and Mixer, and each other regarding stalls.
*/


#ifndef RRPGE_M_FIFO_H
#define RRPGE_M_FIFO_H


#include "rgm_info.h"


/* Emulates Graphics and Mixer FIFO for the given amount of cycles. Uses the
** cys (stall), cya (accelerator) and cym (mixer) members of the rrpge_m_info
** structure, updating them as appropriate. */
void  rrpge_m_fifoproc(auint cy);


/* Operates the memory mapped interface of the FIFOs. Only the low 3 bits of
** the address are used (low 4 addresses selecting the Mixer FIFO, high 4
** addresses the Graphics FIFO). Only low 16 bits of value are used. */
void  rrpge_m_fifowrite(auint adr, auint val);


#endif
