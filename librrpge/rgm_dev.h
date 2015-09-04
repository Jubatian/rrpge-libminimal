/**
**  \file
**  \brief     Input device manager.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.02
**
**
**  Manages the input devices accepting host events, arranging them as needed,
**  then providing those to the application through the kernel.
**
**  Also serves the following RRPGE library functions (host side interface):
**
**  rrpge_dev_add();
**  rrpge_dev_rem();
**  rrpge_dev_push();
*/


#ifndef RRPGE_M_DEV_H
#define RRPGE_M_DEV_H


#include "rgm_info.h"
#include "rrpge.h"


/* Initializes Input device manager adding the appropriate handlers to the
** state manager. */
void  rrpge_m_dev_init(void);


/* Initializes Input device manager internal resources */
void  rrpge_m_dev_initres(rrpge_object_t* hnd);


/* Processes events into the application side event fifo. This should be
** called once every display frame to conform with the specification, since
** this is where source events are combined. */
void  rrpge_m_dev_proc(rrpge_object_t* hnd);


/* Registers a device from application side. Returns nonzero if this was
** possible (there is a physical device which can serve it */
auint rrpge_m_dev_req(rrpge_object_t* hnd, auint id, auint typ);


/* Drops a device from application side. */
void  rrpge_m_dev_drop(rrpge_object_t* hnd, auint id);


/* Peeks an event from the application side FIFO. This removes it from the
** FIFO, putting it into the state (0x006E - 0x006F; only unless already done,
** that is, the state is empty), so it is retained as described in the
** specification. */
auint rrpge_m_dev_peek(rrpge_object_t* hnd);


/* Pops off an event from the application side FIFO. Either clears the state
** or attempts getting a new event from the FIFO depending on the contents of
** the state. */
auint rrpge_m_dev_pop(rrpge_object_t* hnd);


/* Flushes event FIFO and state. */
void  rrpge_m_dev_flush(rrpge_object_t* hnd);


#endif
