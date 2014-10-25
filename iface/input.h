/**
**  \file
**  \brief     Input main module
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
**
**
**  The main module for input handling. It mostly performs the init of the
**  input system.
*/


#ifndef INPUT_H
#define INPUT_H


#include "../host/types.h"
#include "../librrpge/rrpge.h"
#include "inputcom.h"
#include "imouse.h"



/*
** Initialize input subsystem. This ties together the components. Note that
** reset should be called afterwards when loading an application.
*/
void input_init(void);

/*
** Resets input system state according to the given application. If the
** application excepts inputs (restored from a state), the appropriate input
** sources are also opened.
*/
void input_reset(rrpge_object_t* hnd);


#endif
