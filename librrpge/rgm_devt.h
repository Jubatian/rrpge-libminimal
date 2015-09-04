/**
**  \file
**  \brief     Input device manager types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.27
*/


#ifndef RRPGE_M_DEVT_H
#define RRPGE_M_DEVT_H


#include "rgm_type.h"


/* Maximal number of buttons on a device. By the specification 0xB0 is
** sufficient even for the keyboard. */
#define RRPGE_M_DEVT_MAX_BTN 0xB0U

/* Maximal number of touches + 1. The limitation is that the coordinates have
** to be stored somewhere indicating where the touch happened. This also
** applies to pointing device buttons, for that, a minimum of four is
** required. */
#define RRPGE_M_DEVT_MAX_TOU 8U

/* Event queue length for unmanaged events (types 0x8 - 0xF). */
#define RRPGE_M_DEVT_MAX_EVT 32U

/* Maximal number of devices */
#define RRPGE_M_DEVT_MAX_DEV 16U

/* Length of input event fifo. Must be a power of 2. */
#define RRPGE_M_DEVT_FIFOLEN 256U



/* Descriptor of a single device */
typedef struct{

 uint8 btnc[RRPGE_M_DEVT_MAX_BTN]; /* Button states (set: pressed; clear: released) */
 uint8 btnp[RRPGE_M_DEVT_MAX_BTN]; /* Previous button states */
 uint8 btnt[RRPGE_M_DEVT_MAX_BTN]; /* Repeat timers for keyboard text input */
 auint evtq[RRPGE_M_DEVT_MAX_EVT]; /* Event queue for unmanaged events */
 auint prcx[RRPGE_M_DEVT_MAX_TOU]; /* Touch / press X coordinates */
 auint prcy[RRPGE_M_DEVT_MAX_TOU]; /* Touch / press Y coordinates */
 auint ainc[6];                    /* Analog style inputs (0x2 - 0x7), current state */
 auint ainp[6];                    /* Analog style inputs (0x2 - 0x7), previous state */
 auint evtn;                       /* Number of events waiting in the event queue */
 auint typ;                        /* Type of the device (with keyboard text flag) */

}rrpge_m_dev_s_t;


/* Input device manager structure. Components defined here are private to the
** input device emulation, only used by the rgm_dev*.c sources. */
typedef struct{

 rrpge_m_dev_s_t ds[RRPGE_M_DEVT_MAX_DEV]; /* Input devices */
 uint8 devo[RRPGE_M_DEVT_MAX_DEV];         /* Input device occupation (set: allocated) */

 auint fifo[RRPGE_M_DEVT_FIFOLEN];         /* Application side event FIFO */

 auint devb[16];         /* Device bindings by the application (State: 0x0070 - 0x007F) */
 auint peek;             /* Next event on the input event queue (State: 0x006E - 0x006F) */

 auint ffrd;             /* Read pointer in the event FIFO */
 auint ffwr;             /* Write pointer in the event FIFO */

}rrpge_m_dev_t;


#endif
