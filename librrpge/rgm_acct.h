/**
**  \file
**  \brief     Accelerator emulation related types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.15
*/


#ifndef RRPGE_M_ACCT_H
#define RRPGE_M_ACCT_H


#include "rgm_type.h"


/* Accelerator emulation structure. Components defined here are private to the
** accelerator emulation, only used by the rgm_acc*.c sources. */
typedef struct{

 uint8 grb[512];          /* Reindex table */

 auint rwm;               /* PRAM write mask (32 bits; 0x00 - 0x01) */
 auint dbp;               /* Destination bank select & partition size (0x02) */
 auint dps;               /* Destination partition select (0x03) */
 auint dad;               /* Destination post-add (32 bits, 0x04 - 0x05) */
 auint cad;               /* Count post-add (32 bits, 0x06 - 0x07) */
 auint yad;               /* Pointer Y post-add (32 bits, 0x08 - 0x09) */
 auint xad;               /* Pointer X post-add (32 bits, 0x0A - 0x0B) */
 auint yin;               /* Pointer Y increment (32 bits, 0x0C - 0x0D) */
 auint xin;               /* Pointer X increment (32 bits, 0x0E - 0x0F) */
 auint yof;               /* Pointer Y offset (32 bits, 0x10 - 0x11) */
 auint sbn;               /* Source bank select (0x12) */
 auint spt;               /* Source partition select (0x13) */
 auint sps;               /* Source partitioning settings (0x14) */
 auint bfl;               /* Blit control flags & source barrel rot. (0x15) */
 auint sms;               /* Source AND mask & colorkey (0x16) */
 auint rct;               /* Count of rows to blit (0x17) */
 auint cct;               /* Count of cells / pixels to blit (32 bits, 0x18 - 0x19) */
 auint xof;               /* Pointer X offset (32 bits, 0x1A - 0x1B) */
 auint dof;               /* Destination offset (32 bits, 0x1C - 0x1D) */
 auint rpm;               /* Reindexing & pixel OR mask (0x1E) */
 auint pat;               /* Pattern (0x1F) */

}rrpge_m_acc_t;


#endif
