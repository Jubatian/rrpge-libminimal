/**
**  \file
**  \brief     Mouse interfacing
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
**
**
**  Provides the logic for interfacing a single mouse. It can service the
**  mouse device type of RRPGE. Touch sensitive areas act for left button
**  press.
*/


#ifndef IMOUSE_H
#define IMOUSE_H


#include "../host/types.h"
#include "../librrpge/rrpge.h"
#include "inputcom.h"



/*
** Service routine for inputcom_newsrc. Can only return a device source for a
** single mouse device.
*/
auint imouse_newsrc(auint typ);

/*
** Service routine for inputcom_isexist. Always returns one (exists) for it's
** mouse device.
*/
auint imouse_isexist(auint id);

/*
** Service routine for inputcom_freesrc.
*/
void  imouse_freesrc(auint id);

/*
** Service routine for inputcom_di. Returns mouse buttons and touch sensitive
** area hits.
*/
auint imouse_di(auint id, auint ing);

/*
** Service routine for inputcom_ai. Returns mouse cursor coordinates.
*/
auint imouse_ai(auint di, auint inp);


/*
** Refreshes mouse coordinates. The coordinates must range from 0-639 (X), and
** 0-399 (Y).
*/
void  imouse_setcoords(auint x, auint y);

/*
** Refreshes mouse button states. The buttons are as follows:
**
** bit 0: Scroll up
** bit 1: Scroll right
** bit 2: Scroll down
** bit 3: Scroll left
** bit 4: Primary (left) button
** bit 5: Secondary (right) button
** bit 6: Middle button
**
** The bset parameter is a set mask ('1' bits are set), and the bclr parameter
** is a clear mask ('1' bits are cleared).
*/
void  imouse_setbuttons(auint bset, auint bclr);


#endif
