/**
**  \file
**  \brief     LibRRPGE standard header package - types
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
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
typedef uint32_t rrpge_uint32;  /**< 32 bit unsigned integer */
typedef uint16_t rrpge_uint16;  /**< 16 bit unsigned integer */
typedef uint8_t  rrpge_uint8;   /**< 8 bit unsigned integer */
/** \} */



/**
**  \brief     Emulator instance object
**
**  Use for pointers (handles) to emulator instance objects. The RRPGE library
**  implementation fills it up with content the way it needs.
*/
typedef struct rrpge_object_s rrpge_object_t;



/**
**  \brief     User specific data
**
**  User specific data to be transferred in/out the Read Only Process
**  Descriptor.
*/
typedef struct{
 rrpge_uint16 name[4][8];  /**< Names (user IDs) for the 4 users */
 rrpge_uint32 lnex[4];     /**< User's preferred languages in order. The high
                                byte contains the first character in
                                uppercase. Padded with zeroes. */
 rrpge_uint16 clfg;        /**< Preferred foreground color */
 rrpge_uint16 clbg;        /**< Preferred background color. If no colors are
                                set, the two are equal. */
}rrpge_udata_t;



/**
**  \brief     Binary header extract
**
**  A more managable extract for the first 96 bytes of RRPGE related binary
**  headers (application, state saves, nvram saves).
*/
typedef struct{
 rrpge_uint32 vmaj;        /**< Major version number */
 rrpge_uint32 vmin;        /**< Minor version number */
 rrpge_uint32 vpat;        /**< Patch version number */
 rrpge_uint32 tp;          /**< Binary type (ASCII 'A', 'N' or 'S') */
 rrpge_uint8  auth[16];    /**< Author name bytes */
 rrpge_uint8  name[36];    /**< Application name bytes */
}rrpge_header_t;



/**
**  \brief     Emulator state
**
**  The state of the emulator for exporting or importing. Note that the Read
**  Only Process Descriptor is presented populated according to the state
**  export's requirements (packed with the video hardware peripheral data and
**  internal state). Code ROM is not included as it comes from the application
**  binary.
*/
typedef struct{
 rrpge_uint32 vram[128U * 2048U]; /**< Video memory (128 pages) */
 rrpge_uint16 dram[224U * 4096U]; /**< Data memory (224 pages) */
 rrpge_uint16 sram[8U   * 4096U]; /**< Stack memory (8 pages) */
 rrpge_uint16 ropd[4096U];        /**< Read only process descriptor */
}rrpge_state_t;



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
/** License of the application is not supported by the library. */
#define RRPGE_ERR_LIC  0x0002U
/** Nonexistent page when uploading / downloading state, or too few code pages
**  when initializing. */
#define RRPGE_ERR_PG   0x0003U
/** Version mismatch. This can result when loading an application, indicating
**  that it can not be ran since it was written according to a newer RRPGE
**  specification. It also happens if a state is attempted to be loaded whose
**  data does not match the application's. */
#define RRPGE_ERR_VER  0x0004U
/** Page contains invalid value (applies to elements of the Read Only Process
**  Descriptor including application header). In the low 12 bits the location
**  containing the erratic value is returned. */
#define RRPGE_ERR_INV  0x1000U
/** Page contains unsupported value (applies to elements of the Read Only
**  Process Descriptor including application header). In the low 12 bits the
**  location containing the erratic value is returned. This can be returned
**  for example if the host wants to change the RRPGE variant during emulation
**  by uploading a new application binary header, and the library can not
**  support this. */
#define RRPGE_ERR_UNS  0x2000U
/** \} */



/**
**  \anchor    page_ids
**  \name      Page indices
**
**  Page indices for uploading / downloading specific pages of the state or
**  code. Mostly these may be used for debugging applications (or cheating).
**
**  \{ */
/** Video RAM. Low 7 bits (0-127) select the page to use. */
#define RRPGE_PG_VRAM  0x0000U
/** Data RAM. Low 8 bits (0-223) select the page to use. */
#define RRPGE_PG_DRAM  0x1000U
/** Code ROM. Low 4 bits (0-15) select the page to use. Note that it is
**  possible to populate any of the 16 code pages even if the application
**  contained less. This is not preserved after the destruction of the
**  emulator instance (it is not contained in the exported state). */
#define RRPGE_PG_CROM  0x2000U
/** Stack RAM. Low 3 bits (0-7) select the page to use. */
#define RRPGE_PG_SRAM  0x3000U
/** Read Only Process Descriptor. Note that it may be rejected when uploading
**  if it contains invalid values. Low bits must be 0. */
#define RRPGE_PG_ROPD  0x4000U
/** Application binary header.  Note that it may be rejected when uploading
**  if it contains invalid values. Low bits must be 0. */
#define RRPGE_PG_APPH  0x5000U
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
/** Application fault (other). The emulation can not continue. This normally
**  should not happen, emulators may use this to signal internal errors where
**  it is detected they can not continue running the emulation. */
#define RRPGE_HLT_FAULT       0x0100U
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
#define RRPGE_RUN_FREE        0
/** Halt before breakpoints are enabled */
#define RRPGE_RUN_BREAK       1
/** Single stepping mode (processes at most one operation per rrpge_run()
**  call) */
#define RRPGE_RUN_SINGLE      2
/** \} */



#endif
