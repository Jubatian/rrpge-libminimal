/**
**  \file
**  \brief     Audio output manager, rrpge_getaudio() implementation.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.15
**
**
**  The rrpge_m_aud_proc() function is to be called from within the
**  rrpge_run() implementation. The rrpge library interface function
**  rrpge_getaudio() and rrpge_set_clock() are also realized here. These
**  together realize the audio output DMA and real time sync of RRPGE.
*/


#ifndef RRPGE_M_AUD_H
#define RRPGE_M_AUD_H


#include "rgm_info.h"
#include "rrpge.h"


/* Initializes audio emulation adding the appropriate handlers to the state
** manager. */
void rrpge_m_aud_init(void);


/* Initializes audio emulation internal resources. */
void rrpge_m_aud_initres(rrpge_object_t* hnd);


/* Based on the cycles needing emulation, processes audio into the internal
** audio buffer, flagging the 512 sample passed event as required. */
void rrpge_m_aud_proc(rrpge_object_t* hnd, auint cy);


#endif
