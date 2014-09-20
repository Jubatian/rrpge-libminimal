/**
**  \file
**  \brief     LibRRPGE standard header package - types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.20
*/


#ifndef RRPGE_TP_H
#define RRPGE_TP_H


#include <stdint.h>


/**
**  \anchor    base_types
**  \name      Base types
**
**  These are fixed size unsigned integer types. In C89 such types are not
**  defined by the standard, when compiling by C99 or above, these may be
**  pointed to the appropriate types of stdint.h.
**
**  \{ */
typedef uint32_t rrpge_uint32; /**< 32 bit unsigned integer */
typedef uint16_t rrpge_uint16; /**< 16 bit unsigned integer */
typedef uint8_t  rrpge_uint8;  /**< 8 bit unsigned integer */
/** \} */



/**
**  \brief     Emulator instance object
**
**  Use for pointers (handles) to emulator instance objects. The RRPGE library
**  implementation fills it up with content the way it needs.
*/
typedef struct rrpge_object_s rrpge_object_t;



/**
**  \brief     Emulator state
**
**  The state of the emulator for exporting and importing.
*/
typedef struct{
 rrpge_uint32 pram[1048576U];  /**< Peripheral RAM (1M * 32 bits) */
 rrpge_uint16 dram[  65536U];  /**< CPU data memory accessible to user */
 rrpge_uint16 sram[  32768U];  /**< CPU stack memory accessible to user */
 rrpge_uint16 stat[   1024U];  /**< Application state (and header) */
}rrpge_state_t;



/**
**  \brief     Initialization data
**
**  Data extracted from an RRPGE Application file by which the RRPGE
**  Application may be initialized for emulation using the rrpge_init()
**  function. The data member must be filled with the appropriate contents of
**  the application binary beginning with address 0.
*/
typedef struct{
 rrpge_uint16 head[64U];       /**< Application header (first 64 words) */
 rrpge_uint16 desc[10U];       /**< Application descriptor's first 10 words */
 rrpge_uint16 crom[65536U];    /**< Application code */
 rrpge_uint16 data[65536U];    /**< Initial data */
 rrpge_uint32 ccnt;            /**< Count of valid code words */
 rrpge_uint32 dcnt;            /**< Count of valid data words */
}rrpge_appini_t;



/**
**  \anchor    state_regions
**  \name      Application State regions
**
**  These are the region start offsets within the Application State
**  (rrpge_state_t.stat).
**
**  \{ */
/** Application header */
#define RRPGE_STA_HEAD     0x000U
/** State variables */
#define RRPGE_STA_VARS     0x040U
/** Mixer */
#define RRPGE_STA_MIXER    0x090U
/** Accelerator */
#define RRPGE_STA_ACC      0x0A0U
/** User Peripheral Area */
#define RRPGE_STA_UPA      0x0C0U
/** Palette */
#define RRPGE_STA_PAL      0x100U
/** Kernel task data */
#define RRPGE_STA_KTASK    0x200U
/** Accelerator reindex table */
#define RRPGE_STA_REIND    0x300U
/** \} */



/**
**  \anchor    error_codes
**  \name      Error codes
**
**  These error codes are returned by the library methods where such type of
**  return is specified. Note that they may contain extra data as specified
**  below with the individual codes.
**
**  \{ */
/** Succesful return */
#define RRPGE_ERR_OK   0U
/** Unknown / unspecified error. Normally this should not be returned, but
**  libraries which may fail for implementation related reasons may use this
**  to indicate such a failure. */
#define RRPGE_ERR_UNK  0x0001U
/** Initialization data error. Normally for Application Header problems, the
**  RRPGE_ERR_INV constant should be returned, this code is reserved for
**  problems specific to the initialization data which hinder loading it. */
#define RRPGE_ERR_INI  0x0002U
/** Version mismatch. This can result when loading an application, indicating
**  that it can not be ran since it was written according to a newer RRPGE
**  specification. It also happens if a state is attempted to be loaded whose
**  data does not match the application's. */
#define RRPGE_ERR_VER  0x0003U
/** Application State contains invalid value. In the low 10 bits the location
**  containing the erratic value is returned. */
#define RRPGE_ERR_INV  0x1000U
/** Application State contains unsupported value. In the low 10 bits the
**  location containing the erratic value is returned. */
#define RRPGE_ERR_UNS  0x2000U
/** \} */



/**
**  \anchor    halt_causes
**  \name      Halt causes
**
**  Definitions of emulation halt causes. Multiple causes may happen
**  simultaneously. Halt causes where "emulation can not continue" is noted
**  may only be cleared up with a reset (or state reload).
**
**  \{ */
/** No halt cause. */
#define RRPGE_HLT_NONE        0U
/** Audio halt. Emulation is blocked until rrpge_getaudio() is called to
**  consume the audio event. */
#define RRPGE_HLT_AUDIO       0x0001U
/** Exit. The mainline of the application returned. The emulation can not
**  continue. */
#define RRPGE_HLT_EXIT        0x0002U
/** Display frame end. A complete display is rendered. This halt cause happens
**  even if the rendering is turned off. */
#define RRPGE_HLT_FRAME       0x0004U
/** Callback performed excluding the line rendering callback. This can be used
**  to postpone processing from the callback to the main line where then the
**  bulk of the work may be performed and the emulator state may be updated
**  according to the result of the operation before continuing. */
#define RRPGE_HLT_CALLBACK    0x0008U
/** Reached breakpoint. Calling rrpge_run() again will ignore the breakpoint,
**  so continues emulation. */
#define RRPGE_HLT_BREAK       0x0010U
/** Application tried to execute invalid instruction (a kernel mode
**  instruction). The emulation should not be continued. If so, the PC will
**  point after the offending (always one word) instruction. */
#define RRPGE_HLT_INVOP       0x0020U
/** Invalid kernel call: the application tried to ask something from the
**  kernel it can not do. The emulation should not be continued. If so, the PC
**  will point after the offending kernel call (after the last parameter).
**  This behavior might be relied upon if the host wants to emulate extended
**  functionality (using rrpge_getkcallpar()). */
#define RRPGE_HLT_INVKCALL    0x0040U
/** Stack fault: addressing outside of allowed stack space. The emulation
**  should not be continued. If so, the PC will point after the offending
**  instruction. */
#define RRPGE_HLT_STACK       0x0080U
/** Emulator state is detached. To continue emulation, rrpge_attachstate()
**  needs to be called first. */
#define RRPGE_HLT_DETACHED    0x0100U
/** Application fault (other). The emulation can not continue. This normally
**  should not happen, emulators may use this to signal internal errors where
**  it is detected they can not continue running the emulation. */
#define RRPGE_HLT_FAULT       0x0200U
/** \} */



/**
**  \anchor    rrpge_run_modes
**  \name      Running modes for rrpge_run()
**
**  These are modes in which the emulator is capable to be running with
**  respect to debugging possibilities.
**
**  \{ */
/** Normal free running mode, no debugging */
#define RRPGE_RUN_FREE        0U
/** Halt before breakpoints are enabled */
#define RRPGE_RUN_BREAK       1U
/** Single stepping mode (processes at most one operation per rrpge_run()
**  call) */
#define RRPGE_RUN_SINGLE      2U
/** \} */



/**
**  \anchor    rrpge_dev_types
**  \name      Input device types
**
**  These are the input device types, as defined by the RRPGE specification.
**
**  \{ */
#define RRPGE_INPT_MOUSE      0U
#define RRPGE_INPT_TOUCH      1U
#define RRPGE_INPT_GAMEPAD    2U
#define RRPGE_INPT_JOYSTICK   3U
#define RRPGE_INPT_TEXT       4U
#define RRPGE_INPT_KEYBOARD   5U
/** \} */



#endif
