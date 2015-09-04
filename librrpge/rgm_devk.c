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


#include "rgm_devk.h"


void  rrpge_m_devk_proc(rrpge_object_t* hnd, auint id)
{
 /* TODO: guess */
}
