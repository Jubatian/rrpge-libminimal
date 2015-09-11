/**
**  \file
**  \brief     Global information structure.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.11
**
**
** The global structure's fields are used within servicing one RRPGE library
** call (and not across calls, so the library is able to handle more emulation
** instances simultaneously as it is supposed to). The reason behind using
** this architecture is performance.
**
** Some of the values are just copies from the application state, if modified,
** updated before return. The reason is that the app. state comes through an
** indirection by an RRPGE library interface data pointer, using that directly
** multiple times through non native sized (16bit instead of 32/64) is slower.
*/


#ifndef RRPGE_M_INFO_H
#define RRPGE_M_INFO_H


#include "rgm_type.h"
#include "rgm_cput.h"
#include "rgm_prmt.h"
#include "rgm_vidt.h"
#include "rgm_acct.h"
#include "rgm_devt.h"



/* Emulation state structure. This is the structure of the data passed from
** the host for storing / managing the emulation instance. */
/* (Maybe will be put somewhere else more appropriate) */
struct rrpge_object_s{

 rrpge_state_t st;   /* Complete emulator state as defined in the library interface */

 uint16 crom[65536U];   /* Code memory */
 uint16 dini[65536U];   /* Initial data memory (for resets) */
 uint16 apph[64U];      /* Application header */
 uint16 appd[64U];      /* Application descriptor */

 uint32 brkp[2048U]; /* Bit map marking code addresses as breakpoints */

 uint16 recb[4096U]; /* Receive data buffer for network packets */
 uint16 reci[512U];  /* Receive source ID buffer (64 sources, 8 words each) */
 auint  recl[64U];   /* Receive packet length buffer */

 uint8  audl[1024U]; /* Audio left double buffer (2 x 512 samples) */
 uint8  audr[1024U]; /* Audio right double buffer (2 x 512 samples) */

 rrpge_cb_line_t*     cb_lin; /* Line renderer callback */
 rrpge_cb_kcalltsk_t* cb_tsk[RRPGE_CB_IDRANGE]; /* Kernel task callbacks */
 rrpge_cb_kcallsub_t* cb_sub[RRPGE_CB_IDRANGE]; /* Kernel subroutine callbacks */
 rrpge_cb_kcallfun_t* cb_fun[RRPGE_CB_IDRANGE]; /* Kernel function callbacks */

 rrpge_m_cpu_t cpu;  /* CPU emulation structure */
 rrpge_m_prm_t prm;  /* PRAM emulation structure */
 rrpge_m_vid_t vid;  /* Video (GDG) emulation structure */
 rrpge_m_acc_t acc;  /* Graphic Accelerator emulation structure */
 rrpge_m_dev_t dev;  /* Input device emulation structure */

 auint  hlt;         /* Halt causes (accessed using rgm_halt) */

 auint  clk;         /* Main clock frequency. Typically 12500000U for 12.5MHz */
 auint  clkf;        /* Clock fraction, for accurately reproducing the main clock from the audio sync */

 auint  rebr;        /* Receive data buffer read pointer */
 auint  rebw;        /* Receive data buffer write pointer */
 auint  reir;        /* Receive ID & pk. length buffer read pointer */
 auint  reiw;        /* Receive ID & pk. length buffer write pointer */

 auint  tsfl;        /* Task started flags.
                     ** Set if the according kernel task is already started,
                     ** cleared otherwise. The 16 kernel tasks are mapped from
                     ** bit 0 to bit 15. */

 auint  audp;        /* Audio double buffer next fill pointer */
 auint  aco;         /* Count of audio events needing service */

 auint  kfc;         /* Free cycle count remaining between kernel internal
                     ** process takeovers. */

 auint  insm;        /* Initialization state machine */
 auint  inss;        /* Current reached initialization state (rrpge_init defines) */

};



/* Global info structure. This is used to accelerate emulation. Note that
** preferably all globals are placed here to increase locality, and to have
** everything preventing threaded use in one place. TODO: This will be removed
** as part of the refactoring process once everything adapts the new
** structure. */
typedef struct{

 auint  atc;         /* Cycles until next audio tick (State: 0x053) */
 auint  cyf[2];      /* FIFO cycles:
                     ** 0: Cycles remaining from mixer op. (State: 0x062-0x063)
                     ** 1: Cycles remaining from video acc. op. (State: 0x06A-0x06B) */

}rrpge_m_info_t;


extern rrpge_m_info_t  rrpge_m_info;
extern rrpge_object_t* rrpge_m_edat;

/* Allocator and deallocator */

extern rrpge_malloc_t* rrpge_m_malloc;
extern rrpge_free_t*   rrpge_m_free;


#endif
