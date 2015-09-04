/**
**  \file
**  \brief     Keyboard to Text input translation.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.02
**
**
**  Processes Keyboard devices into text input by interpreting the keyboard
**  input, pushing the appropriate text events. It processes modifiers (SHIFT,
**  CTRL, ALT), and also implements key repeating.
**
**  TODO: The SHIFTLOCK key is not functional for now, needs specification...
**  It should work independently of the application, the SHIFTLOCK state
**  probably returning its state (and not the button's state), also needing
**  memory (state variable).
*/


#ifndef RRPGE_M_DEVK_H
#define RRPGE_M_DEVK_H


#include "rgm_info.h"


/* Time until first repeat of a key */
#define RRPGE_M_DEVK_REPS 60U

/* Time between repeats once the first repeat happens */
#define RRPGE_M_DEVK_REPR 6U


/* Processes keyboard devices into text input events. Note that repeating
** operates by counting calls, so normally some 50 - 70Hz rate when using this
** routine appropriately (calling once per frame). */
void  rrpge_m_devk_proc(rrpge_object_t* hnd, auint id);


#endif
