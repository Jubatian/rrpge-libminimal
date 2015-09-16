/**
**  \file
**  \brief     Audio emulation related types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.16
*/


#ifndef RRPGE_M_AUDT_H
#define RRPGE_M_AUDT_H


#include "rgm_type.h"


/* Audio emulation structure. Components defined here are private to the audio
** emulation, only used by the rgm_aud*.c sources. */
typedef struct{

 uint16 bufl[1024U];      /* Audio left double buffer (2 x 512 samples) */
 uint16 bufr[1024U];      /* Audio right double buffer (2 x 512 samples) */
 auint  bufp;             /* Audio double buffer next fill pointer */
 auint  evct;             /* Count of audio events needing service */

 auint  mclk;             /* Main clock frequency in Hz (Normally 12500000) */
 auint  mclf;             /* Clock fraction for converting from audio to main clock */

 auint  rclk;             /* Cycles until next 48KHz audio base clock tick (State: 0x053) */
 auint  c48k;             /* 48KHz audio base clock (State: 0x054) */

 auint  c187;             /* 187.5Hz clock (0x0001) */
 auint  dsct;             /* Audio DMA sample counter (0x0002) */
 auint  dbcl;             /* Audio DMA base clock (0x0003) */
 auint  dlof;             /* Audio DMA left channel start offset bits (0x0004) */
 auint  drof;             /* Audio DMA right channel start offset bits (0x0005) */
 auint  dsiz;             /* Audio DMA buffer size mask bits (0x0006) */
 auint  ddiv;             /* Audio clock divider (0x0007) */

}rrpge_m_aud_t;


#endif
