/**
**  \file
**  \brief     Common types.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
*/


#ifndef RRPGE_M_TYPE_H
#define RRPGE_M_TYPE_H


#include "rrpge_tp.h"
#include "rrpge_cb.h"


/* Type defs. These are defines since they should be equivalent to the
** appropriate RRPGE types, just shortening them */
#define uint8  rrpge_uint8
#define uint16 rrpge_uint16
#define uint32 rrpge_uint32

/* Architecture optimal unsigned integer, at least 32 bits. This definition
** should be right for it, the program is not even guaranteed to work if the
** size of "int" is less than 32 bits due to integer promotion quirks. This
** may also cause auint to be 32 bits on a 64 bit machine if the compiler is
** designed with a 32 bit integer size, however this case using the compiler's
** native integer size is usually the best. */
typedef unsigned int auint;


/* Main frequency. For the minimal specification use 12500000U. You may
** increase it as a simple way to accelerate the emulated machine. Note that
** video timing assumes this frequency to output according to VGA 70Hz, so
** increasing it will also increase the video frequency. Audio frequency is
** maintained for real time synchronization. */
#define RRPGE_M_OSC 12500000U

/* Number of video lines total. At least 449, resulting in 70Hz VGA. */
#define RRPGE_M_VLN 449U

/* Peripheral RAM size. */
#define RRPGE_M_PRAMS (sizeof(((rrpge_state_t*)(0))->pram) / sizeof(((rrpge_state_t*)(0))->pram[0]))

/* Null constant */
#define RRPGE_M_NULL (0)

/* Fastcall convention (only used internally within the library) */
#if (defined (__GNUC__) && defined(__i386__))
#define RRPGE_M_FASTCALL __attribute__((fastcall))
#else
#define RRPGE_M_FASTCALL
#endif


#endif
