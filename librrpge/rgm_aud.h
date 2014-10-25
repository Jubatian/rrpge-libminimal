/**
**  \file
**  \brief     Audio output manager, rrpge_getaudio() implementation.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
**
**
**  The rrpge_m_audproc() function is to be called from within the rrpge_run()
**  implementation where the internal structures are prepared. The rrpge
**  library interface function rrpge_getaudio() however, appropriately, does
**  not rely on any of the internal structures. These together realize the
**  audio output DMA of RRPGE.
*/


#ifndef RRPGE_M_AUD_H
#define RRPGE_M_AUD_H


#include "rgm_info.h"
#include "rrpge.h"


/* Based on the cycles needing emulation, processes audio into the internal
** audio buffer, flagging the 512 sample passed event as required. */
void rrpge_m_audproc(auint cy);


#endif
