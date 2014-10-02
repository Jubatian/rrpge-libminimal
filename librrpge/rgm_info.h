/**
**  \file
**  \brief     Global information structure.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.10.02
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

/* Peripheral RAM size. */
#define RRPGE_M_PRAMS (sizeof(rrpge_state_t.pram) / sizeof(rrpge_state_t.pram[0]))

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

 rrpge_state_t st;   /* Complete emulator state as defined in the library interface */

 uint16 crom[65536U];   /* Code memory */
 uint16 dini[65536U];   /* Initial data memory (for resets) */

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

 auint  rebr;        /* Receive data buffer read pointer */
 auint  rebw;        /* Receive data buffer write pointer */
 auint  reir;        /* Receive ID & pk. length buffer read pointer */
 auint  reiw;        /* Receive ID & pk. length buffer write pointer */

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

 auint  audp;        /* Audio double buffer next fill pointer */
 auint  aco;         /* Count of audio events needing service */

 auint  kfc;         /* Free cycle count remaining between kernel internal
                     ** process takeovers. */

};



/* Addressing unit's function types. */
typedef RRPGE_M_FASTCALL auint (rrpge_m_addr_read_t)(void);
typedef RRPGE_M_FASTCALL void (rrpge_m_addr_write_t)(auint);



/* Global info structure. This is used to accelerate emulation. Note that
** preferably all globals are placed here to increase locality, and to have
** everything preventing threaded use in one place. */
typedef struct{

 uint8  grb[512];    /* Recolor bank data extracted from the state */
 auint  grr;         /* Recolor bank load necessary flag: set on entry, will
                     ** ask for populating grb[] when it is needed. */

 auint  vln;         /* Video line count (State: 0x050) */
 auint  vlc;         /* Video remaining cycles within line (State: 0x051) */
 auint  atc;         /* Cycles until next audio tick (State: 0x053) */
 auint  cyf[2];      /* FIFO cycles:
                     ** 0: Cycles remaining from mixer op. (State: 0x062-0x063)
                     ** 1: Cycles remaining from video acc. op. (State: 0x06A-0x06B) */
 auint  cys;         /* PRAM Stall cycles collected during a run of CPU emulation */

 auint  vbm;         /* Video mode bitmask. 0xF for 4bit mode, 0xFF for 8bit
                     ** mode. (From State: 0x052) */

 auint  hlt;         /* Collects halt cause if any. */

 auint  ocy;         /* Opcode address extra cycles. This is used by the
                     ** addressing mode unit to mark if extra cycles were
                     ** consumed for any reason stalling the CPU. */
 auint  oaw;         /* Opcode address extra word. This is used by the
                     ** addressing mode unit to mark if an extra opcode word
                     ** was consumed. */
 auint  opc;         /* Opcode first word cache / addressing cache. Normally
                     ** the next opcode is loaded in this for faster access, in
                     ** function parameters it is also used for passing the
                     ** first word of the parameter components. */

 auint  xr[8];       /* CPU general registers (A-D, X0-X3) (State: 0x040-0x047) */
 auint  xmh[2];      /* CPU pointer mode/high registers (XM, XH) (State: 0x048-0x049) */
 auint  pc;          /* CPU program counter (State: 0x04A) */
 auint  sp;          /* CPU stack pointer (State: 0x04B) */
 auint  bp;          /* CPU base pointer (State: 0x04C) */

 auint  sbt;         /* Stack bottom, also high bit(s) for bp and sp */
 auint  stp;         /* Stack top */

 auint  pia;         /* Temporary values for Peripheral RAM interface */
 auint  pid;
 auint  pis;
 auint  pim;

 auint  ada;         /* Temporary values for the Addressing unit */
 auint  add;
 auint  ads;
 auint  adm;

 rrpge_m_addr_write_t* awf;
                     /* Addressing mode specific write. Must be used with a
                     ** corresponding addressing mode read to complete an
                     ** R-M-W cycle. The read function sets this function
                     ** pointer to the appropriate write function so it can be
                     ** called. Increments rrpge_m_info.ocy if necessary. */

}rrpge_m_info_t;


extern rrpge_m_info_t  rrpge_m_info;
extern rrpge_object_t* rrpge_m_edat;


#endif
