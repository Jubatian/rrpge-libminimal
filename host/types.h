/**
**  \file
**  \brief     Basic type defs & includes
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
*/


#ifndef TYPES_H
#define TYPES_H


#include <stdio.h>                /* printf */
#include <stdlib.h>               /* NULL, malloc, free, etc. */
#include <string.h>               /* Basic block memory routines */
#include <stdint.h>               /* Fixed width integer types */
#include "../librrpge/rrpge_tp.h" /* Fixed width integer types */


typedef   signed  int   asint;  /* Architecture signed integer (At least 2^31) */
typedef unsigned  int   auint;  /* Architecture unsigned integer (At least 2^31) */
typedef        int16_t  sint16;
typedef rrpge_uint16    uint16;
typedef        int32_t  sint32;
typedef rrpge_uint32    uint32;
typedef        int8_t   sint8;
typedef rrpge_uint8     uint8;


#endif
