/**
**  \file
**  \brief     CPU opcode decoder & ALU
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#ifndef RRPGE_M_CPUO_H
#define RRPGE_M_CPUO_H


#include "rgm_info.h"


/* Function type definition for opcode services. Returns number of cycles
** consumed, they may alter the state at will. */
typedef RRPGE_M_FASTCALL auint (rrpge_m_opf_t)(void);


/* CPU opcode call table. The rrpge_m_info structure must be set up
** appropriately to call these (note the opcode cache member). As above, the
** effect of the opcode's execution varies depending on what the user program
** attempted to perform. Callbacks may also happen (here through calling the
** supervisor). */
extern rrpge_m_opf_t* const rrpge_m_optable[128];


#endif
