/**
**  \file
**  \brief     SDL audio handling functions
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#ifndef AUDIO_H
#define AUDIO_H


#include "types.h"


/* This user event will be generated when an audio event should happen
** according to the initialization parameters. This can be used to sync. the
** main thread to the audio output. Note that when an event of this type
** happens, multiple servicing of the audio may be necessary (maybe even zero
** if events stall), use audio_needservice() to determine. */
#define AUDIO_EVENT 255U


/* Set up audio. 'f' requests the frequency, bit0 selects it: 0: 24KHz, 1:
** 48KHz. 's' selects the count of samples per buffer fill: 0: 512, 1: 1024.
** The 'b' parameter defines the total buffer size, must be a power of two.
** This buffer size may be set larger to avoid audio skipping. The sample
** format is unsigned 8 bits. Returns 0 on success, 1 on fault. */
auint   audio_set(auint f, auint s, auint b);

/* Cleans up the previously set up audio. */
void    audio_free();

/* Request left & right preparatory buffer pointers. These buffers hold the
** audio data which will be supplied to the audio thread at the next audio
** callback. Each buffer is capable to hold up to 1024 samples of which 512 or
** 1024 will be used depending on the set up mode. The buffers should be
** filled immediately after an AUDIO_EVENT from the main thread to avoid
** skipping (by a subsequent audio event). */
void    audio_getbufptrs(uint8** lpt, uint8** rpt);

/* Requests whether the audio subsystem needs servicing (needing an additional
** call to audio_getbufptrs()). Note that when an audio event is sent, it may
** require multiple servicing. */
auint   audio_needservice(void);

#endif
