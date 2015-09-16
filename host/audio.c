/**
**  \file
**  \brief     SDL audio handling functions
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.15
*/


#include "audio.h"
#include <SDL/SDL.h>



/* Note: Weak...! Changing this also requires changes in the code */
#define AUDIO_BUF_MAX 8192U

/* Samples per one audio refill. It is fixed at 512 by the RRPGE specification. */
#define AUDIO_RFS     512U


/* Left & Right audio buffers */
static uint16 audio_buf_l[AUDIO_BUF_MAX];
static uint16 audio_buf_r[AUDIO_BUF_MAX];
/* Pointers into the audio buffers */
static volatile auint audio_bpt_r;
static volatile auint audio_bpt_w;
/* Total buffer size mask */
static auint audio_bsm;



/* Audio callback */
static void audio_cb(void* udata, Uint8 *stream, int len)
{
 auint i;
 SDL_Event auev;

 len >>= 2; /* Convert to samples */
 if (len > AUDIO_BUF_MAX) len = AUDIO_BUF_MAX;

 /* Output samples */

 for (i = 0; i < len; i++){
  stream[(i << 2) + 0U] =  audio_buf_l[audio_bpt_r]       & 0xFFU;
  stream[(i << 2) + 1U] = (audio_buf_l[audio_bpt_r] >> 8) & 0xFFU;
  stream[(i << 2) + 2U] =  audio_buf_r[audio_bpt_r]       & 0xFFU;
  stream[(i << 2) + 3U] = (audio_buf_r[audio_bpt_r] >> 8) & 0xFFU;
  audio_bpt_r = (audio_bpt_r + 1U) & audio_bsm;
 }

 /* Push an audio event if there is a buffer worth of space after the write
 ** pointer */

 if ( ((audio_bpt_r - audio_bpt_w) & audio_bsm) > AUDIO_RFS ){
  auev.type      = SDL_USEREVENT;
  auev.user.code = AUDIO_EVENT;
  SDL_PushEvent(&auev); /* Send event to the main thread */
 }

}



/* Set up audio. The 'b' parameter defines the total buffer size, must be a
** power of two. This buffer size may be set larger to avoid audio skipping.
** The sample format is unsigned 16 bits. Returns 0 on success, 1 on fault. */
auint   audio_set(auint b)
{
 SDL_AudioSpec des;

 des.freq     = 48000U;
 des.samples  = 1024U;
 des.format   = AUDIO_U16;
 des.channels = 2U;
 des.callback = &audio_cb;
 des.userdata = NULL;

 /* (Note: keep AUDIO_BUF_MAX and this in sync !!!) */
 if      (b < 1024U) audio_bsm = 1024U - 1U;
 else if (b < 2048U) audio_bsm = 2048U - 1U;
 else if (b < 4096U) audio_bsm = 4096U - 1U;
 else                audio_bsm = 8192U - 1U;

 if (SDL_OpenAudio(&des, NULL) < 0) return 1U;

 /* Fills with 0x8080, good enough for now... */
 memset(&(audio_buf_l[0]), 0x80U, sizeof(audio_buf_l));
 memset(&(audio_buf_r[0]), 0x80U, sizeof(audio_buf_r));
 audio_bpt_r = 0;
 audio_bpt_w = 0;

 SDL_PauseAudio(0);

 return 0;
}



/* Cleans up the previously set up audio. */
void    audio_free()
{
 SDL_CloseAudio();
}



/* Request left & right preparatory buffer pointers. These buffers hold the
** audio data which will be supplied to the audio thread at the next audio
** callback. Each buffer is capable to hold up to 1024 samples of which 512 or
** 1024 will be used depending on the set up mode. The buffers should be
** filled immediately after an AUDIO_EVENT from the main thread to avoid
** skipping (by a subsequent audio event). */
void    audio_getbufptrs(uint16** lpt, uint16** rpt)
{
 *lpt = &(audio_buf_l[audio_bpt_w]);
 *rpt = &(audio_buf_r[audio_bpt_w]);
 audio_bpt_w = (audio_bpt_w + AUDIO_RFS) & audio_bsm;
}



/* Requests whether the audio subsystem needs servicing (needing an additional
** call to audio_getbufptrs()). Note that when an audio event is sent, it may
** require multiple servicing. */
auint   audio_needservice(void)
{
 if ( ((audio_bpt_r - audio_bpt_w) & audio_bsm) > AUDIO_RFS ){ return 1U; }
 return 0U;
}
