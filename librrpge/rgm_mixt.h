/**
**  \file
**  \brief     Mixer emulation related types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.15
*/


#ifndef RRPGE_M_MIXT_H
#define RRPGE_M_MIXT_H


#include "rgm_type.h"


/* Accelerator emulation structure. Components defined here are private to the
** accelerator emulation, only used by the rgm_acc*.c sources. */
typedef struct{

 auint dbk;               /* Destination bank select (16 bits; 0x5) */
 auint dof;               /* Destination start pointer (16 bits; 0x6) */
 auint dct;               /* Destination cell count and overwrite (16 bits; 0x7) */
 auint scf;               /* Source configuration (16 bits; 0x8) */
 auint spr;               /* Sample partition select bits (16 bits; 0x9) */
 auint sfa;               /* Sample pointer fraction add value (16 bits; 0xA) */
 auint sfr;               /* Sample pointer fraction (16 bits; 0xB) */
 auint ama;               /* Amplitude multiplier add value (16 bits; 0xC) */
 auint ami;               /* Initial amplitude multiplier (16 bits; 0xD) */
 auint soh;               /* Sample bit pointer, high (16 bits; 0xE) */
 auint sol;               /* Sample bit pointer, low (16 bits; 0xF) */

}rrpge_m_mix_t;


#endif
