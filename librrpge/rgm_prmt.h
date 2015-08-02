/**
**  \file
**  \brief     PRAM access related types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
*/


#ifndef RRPGE_M_PRMT_H
#define RRPGE_M_PRMT_H


#include "rgm_type.h"


/* PRAM access structure. Components defined here are private to the PRAM
** emulation, only used by the rgm_pram.c source. */
typedef struct{

 auint  sta[32U];    /* PRAM peripheral registers */

 auint  pia;         /* Temporary values for Peripheral RAM interface */
 auint  pid;
 auint  pis;
 auint  pim;

}rrpge_m_prm_t;


#endif
