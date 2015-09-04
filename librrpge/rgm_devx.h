/**
**  \file
**  \brief     Device translations.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.02
**
**
**  Translates between device types where appropriate, mostly relating the
**  keyboard. Between keyboard and gamepad it relies on that no keyboard
**  button is normally mapped to the 7 gamepad button ID's, so a device may
**  simply carry both information, and it might be isolated when mapping to an
**  application device only (by taking buttons up to and including button 7
**  for gamepads, and from and including button 8 for keyboards).
**
**  Also calls the text input mapper for the keyboard (in a way the gamepad
**  inputs can propagate into text input as required by the specification).
*/


#ifndef RRPGE_M_DEVX_H
#define RRPGE_M_DEVX_H


#include "rgm_info.h"


/* Processes device translations */
void  rrpge_m_devx_proc(rrpge_object_t* hnd);


#endif
