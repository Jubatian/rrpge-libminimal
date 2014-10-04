/**
**  \file
**  \brief     Input main module
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.10.04
*/


#include "input.h"
#include "inputcom.h"



/*
** Initialize input subsystem. This ties together the components. Note that
** reset should be called afterwards when loading an application.
*/
void input_init(void)
{
 inputcom_cb_t cb;

 /* Register the mouse device */

 cb.newsrc   = &imouse_newsrc;
 cb.isexist  = &imouse_isexist;
 cb.freesrc  = &imouse_freesrc;
 cb.di       = &imouse_di;
 cb.ai       = &imouse_ai;
 inputcom_register(&cb);

}



/*
** Resets input system state according to the given application. If the
** application excepts inputs (restored from a state), the appropriate input
** sources are also opened.
*/
void input_reset(rrpge_object_t* hnd)
{
 inputcom_reset(hnd);
}
