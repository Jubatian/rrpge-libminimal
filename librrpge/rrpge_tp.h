/**
**  \file
**  \brief     LibRRPGE standard header package - types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.26
*/


#ifndef RRPGE_TP_H
#define RRPGE_TP_H


#include <stdint.h>


/**
**  \anchor    fixed_types
**  \name      Fixed size types
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
**  \anchor    if_types
**  \name      Interface types
**
**  Types used for passing and returning values for functions. These should be
**  defined so they are optimal for the target architecture, and match it's
**  software infrastructure so the library can fit in.
**
**  \{ */
typedef unsigned int rrpge_iuint; /**< Interface integer, at least 32 bits */
typedef unsigned int rrpge_ibool; /**< Interface boolean, 0 for FALSE */
/** \} */



/**
**  \brief     Memory allocator
**
**  Memory allocator function interface. Usually if available malloc and free
**  may be supplied. If not, depending on library use, simpler or more complex
**  solutions may be provided. Allocation attempts are noted at each method
**  which could use such: in general a stack allocator may be sufficient.
**
**  \param[in]   siz   Size of memory to allocate in bytes (uint8).
**  \return            Pointer to memory allocated or NULL if not possible.
*/
typedef void* rrpge_malloc_t (rrpge_iuint siz);



/**
**  \brief     Memory deallocator
**
**  Complements the allocator, freeing previously allocated memory.
**
**  \param[in]   ptr   Memory to deallocate.
*/
typedef void  rrpge_free_t (void* ptr);



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
 rrpge_uint16 dram[  98304U];  /**< CPU data & stack memory accessible to user (96K * 16 bits) */
 rrpge_uint16 stat[   1024U];  /**< Application state (and header) */
}rrpge_state_t;



/**
**  \anchor    init_levels
**  \name      Emulator initialization levels
**
**  These are the initialization levels which can be passed to
**  rrpge_init_run() to control its behavior.
**
**  \{ */
/** Blank state, no initialization */
#define RRPGE_INI_BLANK    0x00U
/** Application info state, not runnable, for info. query */
#define RRPGE_INI_INFO     0x01U
/** Reset state, runnable */
#define RRPGE_INI_RESET    0x02U
/** \} */



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
/** User Peripheral Area, Audio */
#define RRPGE_STA_UPA_A    0x0C0U
/** User Peripheral Area, Mixer FIFO */
#define RRPGE_STA_UPA_MF   0x0C8U
/** User Peripheral Area, Graphics FIFO */
#define RRPGE_STA_UPA_GF   0x0CCU
/** User Peripheral Area, Graphics Display Generator */
#define RRPGE_STA_UPA_G    0x0D0U
/** User Peripheral Area, Pointers (PRAM interface) */
#define RRPGE_STA_UPA_P    0x0E0U
/** Palette */
#define RRPGE_STA_PAL      0x100U
/** Kernel task data */
#define RRPGE_STA_KTASK    0x200U
/** Accelerator reindex table */
#define RRPGE_STA_REIND    0x300U
/** \} */



/**
**  \anchor    device_types
**  \name      Input device types
**
**  These are the input devices supported by RRPGE.
**
**  \{ */
/** Pointing device */
#define RRPGE_DEV_POINT    0x0U
/** Touch device */
#define RRPGE_DEV_TOUCH    0x1U
/** Digital gamepad */
#define RRPGE_DEV_PAD      0x2U
/** Analog joystick */
#define RRPGE_DEV_STICK    0x3U
/** Text input */
#define RRPGE_DEV_TEXT     0x4U
/** Keyboard */
#define RRPGE_DEV_KEYB     0x5U
/** \} */



/**
**  \andor     device_types_kbflag
**  \name      Keyboard text input separation flag
**
**  This flag is provided for rrpge_dev_add(), to be OR-ed with a keyboard
**  device type. It indicates that the library shouldn't attempt to use the
**  keyboard events to provide a text input device by the keyboard, rather
**  use text input events (message types 0x8 and 0x9) from the same device for
**  this purpose. This is useful for advanced text input (international or
**  independent of the key mapping) for hosts supporting such.
*/
#define RRPGE_DEV_KBTXT    0x10U



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
/** Initialization is waiting to be completed. This is returned by the
**  initialization functions when they have to wait for the host to provide
**  parts of the application binary. */
#define RRPGE_ERR_WAIT 1U
/** Unknown / unspecified error. Normally this should not be returned, but
**  libraries which may fail for implementation related reasons may use this
**  to indicate such a failure. */
#define RRPGE_ERR_UNK  0x0002U
/** Initialization error. Returned if the emulator instance is not
**  sufficiently initialized to perform a given task. In rrpge_init_run() it
**  is returned if it is not possible to complete initialization for any
**  problem other than reportable by other error codes. */
#define RRPGE_ERR_INI  0x0003U
/** Version mismatch. This can result when loading an application, indicating
**  that it can not be ran since it was written according to a newer RRPGE
**  specification. It also happens if a state is attempted to be loaded whose
**  data does not match the application's. */
#define RRPGE_ERR_VER  0x0004U
/** Application State contains invalid value. In the low 10 bits the word
**  location containing the erratic value is returned. */
#define RRPGE_ERR_STA  0x1000U
/** Application State contains unsupported value. In the low 10 bits the word
**  location containing the unsupported value is returned. */
#define RRPGE_ERR_UNS  0x2000U
/** Application descriptor contains invalid value. Returned only by
**  rrpge_init_run(). In the low 4 bits the word location containing the
**  erratic value is returned. */
#define RRPGE_ERR_DSC  0x3000U
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
/** Initialization is not completed yet (still returns RRPGE_ERR_WAIT). */
#define RRPGE_HLT_WAIT        0x0200U
/** Application fault (other). The emulation can not continue. This normally
**  should not happen, emulators may use this to signal internal errors where
**  it is detected they can not continue running the emulation. */
#define RRPGE_HLT_FAULT       0x0400U
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
