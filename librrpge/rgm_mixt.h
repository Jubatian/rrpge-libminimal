/**
**  \file
**  \brief     Mixer emulation related types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.16
*/


#ifndef RRPGE_M_MIXT_H
#define RRPGE_M_MIXT_H


#include "rgm_type.h"


/* Accelerator emulation structure. Components defined here are private to the
** accelerator emulation, only used by the rgm_acc*.c sources. */
typedef struct{

 auint dbk;               /* Destination bank select (16 bits; 0x8) */
 auint dof;               /* Destination start pointer (16 bits; 0x9) */
 auint dct;               /* Destination cell count and overwrite (16 bits; 0xA) */
 auint scf;               /* Source configuration (16 bits; 0xB) */
 auint sfa;               /* Sample pointer fraction add value (16 bits; 0xC) */
 auint ama;               /* Amplitude multiplier add value (16 bits; 0xD) */
 auint ami;               /* Initial amplitude multiplier (16 bits; 0xE) */
 auint sdo;               /* Source descriptor offset (16 bits; 0xF) */

}rrpge_m_mix_t;


#endif
