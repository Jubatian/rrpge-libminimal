/**
**  \file
**  \brief     Global information structure.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.10
**
**
** The global structure's fields are used within servicing one RRPGE library
** call (and not across calls, so the library is able to handle more emulation
** instances simultaneously as it is supposed to). The reason behind using
** this architecture is performance.
**
** Some of the values are just copies from the read only process descriptor,
** if modified, updated before return. The reason is that the process
** descriptor comes through an indirection by an RRPGE library interface data
** pointer, using that directly multiple times through non native sized (16bit
** instead of 32/64) is slower.
*/


#ifndef RRPGE_M_INFO_H
#define RRPGE_M_INFO_H


#include "rrpge_tp.h"
#include "rrpge_cb.h"


/* Type defs. These are defines since they should be equivalent to the
** appropriate RRPGE types, just shortening them */
#define uint8  rrpge_uint8
#define uint16 rrpge_uint16
#define uint32 rrpge_uint32

/* Architecture optimal unsigned integer, at least 32 bits. */
typedef unsigned int  auint;



/* Will be in rgm_type probably...
** Main frequency. For the minimal specification use 12500000U. You may
** increase it as a simple way to accelerate the emulated machine. Note that
** video timing assumes this frequency to output according to VGA 70Hz, so
** increasing it will also increase the video frequency. Audio frequency is
** maintained for real time synchronization. */
#define RRPGE_M_OSC 12500000U

/* Number of video lines total. At least 449, resulting in 70Hz VGA. */
#define RRPGE_M_VLN 449U

/* Video RAM size. 128 pages, but in 32bit units. */
#define RRPGE_M_VRAMS (2048U * 128U)

/* Null constant */
#define RRPGE_M_NULL (0)

/* Fastcall convention (only used internally within the library) */
#if (defined (__GNUC__) && defined(__i386__))
#define RRPGE_M_FASTCALL __attribute__((fastcall))
#else
#define RRPGE_M_FASTCALL
#endif




/* Emulation state structure. This is the structure of the data passed from
** the host for storing / managing the emulation instance. */
/* (Maybe will be put somewhere else more appropriate) */
struct rrpge_object_s{

 rrpge_state_t stat; /* Complete emulator state as defined in the library interface */

 uint16 crom[16U * 4096U]; /* Code memory */

 uint32 brkp[2048U]; /* Bit map marking code addresses as breakpoints */

 uint16 ropc[704U];  /* Read Only Process Descriptor constant area (0xD40 - 0xFFF) */

 uint16 recb[4096U]; /* Receive data buffer for network packets */
 uint16 reci[512U];  /* Receive source ID buffer (64 sources, 8 words each) */
 auint  recl[64U];   /* Receive packet length buffer */

 auint  frep[4U  * 401U];
                     /* Layer pointers for a forward render buffer.
                     ** Here the data for ROPD: 0xD58 - 0xD5F is calculated;
                     ** they may be loaded from here when saving state. Note
                     ** that line + 1 contains the pointers used to render the
                     ** given line, the first four values are always zero. */

 rrpge_cb_line_t*     cb_lin; /* Line renderer callback */
 rrpge_cb_kcalltsk_t* cb_tsk[RRPGE_CB_IDRANGE]; /* Kernel task callbacks */
 rrpge_cb_kcallsub_t* cb_sub[RRPGE_CB_IDRANGE]; /* Kernel subroutine callbacks */
 rrpge_cb_kcallfun_t* cb_fun[RRPGE_CB_IDRANGE]; /* Kernel function callbacks */

 auint  rebr;        /* Receive data buffer read pointer */
 auint  rebw;        /* Receive data buffer write pointer */
 auint  reir;        /* Receive ID & pk. length buffer read pointer */
 auint  reiw;        /* Receive ID & pk. length buffer write pointer */

 auint  frln;        /* Forward render line pointer */
 auint  frld;        /* Clear if previous line was rendered, used by the
                     ** forward renderer. */

 auint  rena;        /* Render Enable flags.
                     ** bit0: Requested state
                     ** bit1: Current state
                     ** The current state copies the requested state when
                     ** passing frame boundary. It controls whether graphic
                     ** display is computed for the given display frame. */

 auint  tsfl;        /* Task started flags.
                     ** Set if the according kernel task is already started,
                     ** cleared otherwise. The 16 kernel tasks are mapped from
                     ** bit 0 to bit 15. */

 auint  hlt;         /* Finalized halt causes (from the info struct) */

 auint  aco;         /* Count of audio events needing service */

 auint  kfc;         /* Free cycle count remaining between kernel internal
                     ** process takeovers. */

};


/* Global info structure. This is used to accelerate emulation */
typedef struct{

 uint8  grb[512];    /* Recolor bank data extracted from the ROPD */

 auint  rbk[16];     /* Read banks << 12 (ROPD: 0xD00-0xD0F) */
 auint  wbk[16];     /* Write banks << 12 (ROPD: 0xD10-0xD1F) */

 auint  vln;         /* Video line count (ROPD: 0xD50) */
 auint  vlc;         /* Video cycle within line (ROPD: 0xD51) */
 auint  auc;         /* Cycles until next audio event (ROPD: 0xD52-0xD53) */
 auint  vac;         /* Cycles remaining from video acc. op. (ROPD: 0xD54-0xD55) */
 auint  sbt;         /* Current stack bottom (ROPD: 0xD7E) */

 auint  adv;         /* Audio divider, calculated from frequency & half buffer
                     ** sizes in app. header (ROPD: 0xBC2) */

 auint  grr;         /* Recolor bank load necessary flag: set on entry, will
                     ** ask for populating grb[] when it is needed. */
 auint  vbm;         /* Video mode bitmask. 0xF for 4bit mode, 0xFF for 8bit
                     ** mode. (From ROPD: 0xBC0) */

 auint  arq;         /* Request accelerator operation if set */

 auint  vsm;         /* Video Stall Mode. 0,1,2,3 or 4 according to the
                     ** currently effective stall mode (roughly number of
                     ** graphics planes enabled). It is updated in cycle 0 and
                     ** 80 of a graphics line. */

 auint  hlt;         /* Collects halt cause if any. */

 auint  ocy;         /* Opcode address extra cycles. This is used by the
                     ** addressing mode unit to mark if extra cycles were
                     ** consumed. */
 auint  oaw;         /* Opcode address extra word. This is used by the
                     ** addressing mode unit to mark if an extra opcode word
                     ** was consumed. */
 auint  opc;         /* Opcode first word cache / addressing cache. Normally
                     ** the next opcode is loaded in this for faster access, in
                     ** function parameters it is also used for passing the
                     ** first word of the parameter components. */

 auint  xr[8];       /* CPU general registers (A-D, X0-X3) (ROPD: 0xD40-0xD47) */
 auint  xmh[2];      /* CPU pointer mode/high registers (XM, XH) (ROPD: 0xD48-0xD49) */
 auint  pc;          /* CPU program counter (ROPD: 0xD4A) */
 auint  sp;          /* CPU stack pointer (ROPD: 0xD4B) */
 auint  bp;          /* CPU base pointer (ROPD: 0xD4C) */

}rrpge_m_info_t;


extern rrpge_m_info_t  rrpge_m_info;
extern rrpge_object_t* rrpge_m_edat;


#endif
