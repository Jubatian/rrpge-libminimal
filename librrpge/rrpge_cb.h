/**
**  \file
**  \brief     LibRRPGE standard header package - callbacks and related
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.20
*/


#ifndef RRPGE_CB_H
#define RRPGE_CB_H


#include "rrpge_tp.h"



/**
**  \brief     Graphic line output.
**
**  This is called when a graphic line is completed and rendered. It is not
**  called in frames where rendering is turned off. The pixel order within the
**  lines is Big Endian, that is the leftmost pixel comes first in the highest
**  bits of the first element. The timing of the calls may not match that of
**  what might be deducted from the cycle counts or the exported state,
**  however the order of lines and their occurence related to frame boundaries
**  (RRPGE_HLT_FRAME halt causes) are strictly kept. Note that this callback
**  does not produce halt cause on execution.
**
**  \param[in]   hnd   Emulation instance the callback is called for.
**  \param[in]   ln    The number of the line rendered (0 - 399).
**  \param[in]   buf   The contents of the line (80 elements).
*/
typedef void rrpge_cb_line_t (rrpge_object_t* hnd, rrpge_uint32 ln, rrpge_uint32 const* buf);



/**
**  \brief     Generic kernel task callback.
**
**  The common prototype for callbacks servicing the kernel tasks of the RRPGE
**  system. The library calls them when experiencing such a kernel call (also
**  setting the RRPGE_HLT_CALLBACK halt cause as a consequence).
**
**  The host may carry out these asynchronously. It must report back later
**  using rrpge_taskend(). If it processes the call on instant, it may call
**  rrpge_taskend() from the callback.
**
**  \param[in]   hnd   Emulation instance the callback is called for.
**  \param[in]   tsh   Task handle by which the host may report back (0-15).
**  \param[in]   par   Extra parameters for the particular callback.
*/
typedef void rrpge_cb_kcalltsk_t (rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par);



/**
**  \brief     Generic kernel subroutine callback.
**
**  The common prototype for callbacks servicing the kernel subroutines of the
**  RRPGE system. The library calls them when experiencing such a kernel call
**  (also setting the RRPGE_HLT_CALLBACK halt cause as a consequence).
**
**  \param[in]   hnd   Emulation instance the callback is called for.
**  \param[in]   par   Extra parameters for the particular callback.
*/
typedef void rrpge_cb_kcallsub_t (rrpge_object_t* hnd, const void* par);



/**
**  \brief     Generic kernel function callback.
**
**  The common prototype for callbacks servicing the kernel functions of the
**  RRPGE system. The library calls them when experiencing such a kernel call
**  (also setting the RRPGE_HLT_CALLBACK halt cause as a consequence). A
**  kernel function is one which has return value (either produced in 'X3' or
**  'C:X3'). If the host wants to perform this outside the callback, it has to
**  catch the RRPGE_HLT_CALLBACK halt cause, check for these, service, and
**  then has to write the 'X3' or 'C:X3' registers manually in the Application
**  State before continuing emulation. When servicing the callback proper the
**  high word of the return goes in 'C' (only if the particular kernel call is
**  specified to write it), and the low word in 'X3'.
**
**  \param[in]   hnd   Emulation instance the callback is called for.
**  \param[in]   par   Extra parameters for the particular callback.
**  \return            Result of function to be written into C:X3.
*/
typedef rrpge_uint32 rrpge_cb_kcallfun_t (rrpge_object_t* hnd, const void* par);



/**
**  \brief     Kernel task callback descriptor.
**
**  Binds a kernel task callback to callback type.
*/
typedef struct{
 rrpge_uint32             id;          /**< Callback ID (type) */
 rrpge_cb_kcalltsk_t*     cb;          /**< Callback function */
}rrpge_cbd_tsk_t;



/**
**  \brief     Kernel subroutine callback descriptor.
**
**  Binds a kernel subroutine callback to callback type.
*/
typedef struct{
 rrpge_uint32             id;          /**< Callback ID (type) */
 rrpge_cb_kcallsub_t*     cb;          /**< Callback function */
}rrpge_cbd_sub_t;



/**
**  \brief     Kernel function callback descriptor.
**
**  Binds a kernel function callback to callback type.
*/
typedef struct{
 rrpge_uint32             id;          /**< Callback ID (type) */
 rrpge_cb_kcallfun_t*     cb;          /**< Callback function */
}rrpge_cbd_fun_t;




/**
**  \brief     Callback descriptor package.
**
**  This structure is used to pass the callbacks to the emulator (by
**  rrpge_init()) which are implemented by the host. The emulator contains
**  defaults for all callbacks (producing the default result assuming the
**  feature is not implemented). This layout simplifies host design, and
**  increases portability (A limited host may use a full RRPGE library or a
**  host designed for probable later RRPGE specification may still work fine
**  with a library implementing an earlier one with fewer callbacks).
*/
typedef struct{
 rrpge_cb_line_t*         cb_line;     /**< Line render callback. The library
                                       **   can not produce graphics without
                                       **   this (leaving it may still be
                                       **   useful for playing music only
                                       **   apps). */
 rrpge_uint32             tsk_n;       /**< Number of task callbacks passed */
 rrpge_cbd_tsk_t const*   tsk_d;       /**< Task callback descriptors */
 rrpge_uint32             sub_n;       /**< Number of sub. callbacks passed */
 rrpge_cbd_sub_t const*   sub_d;       /**< Subroutine callback descriptors */
 rrpge_uint32             fun_n;       /**< Number of fun. callbacks passed */
 rrpge_cbd_fun_t const*   fun_d;       /**< Function callback descriptors */
}rrpge_cbpack_t;



/**
**  \anchor    rrpge_fileio_fault
**  \name      File I/O fault codes for file I/O kernel task returns
**
**  These codes should be used to indicate various file I/O related faults.
**
**  \{ */
/** Unsupported feature. The host does not implement the feature. */
#define RRPGE_SFI_UNSUPP  0xC000U
/** Improper or Invalid file name. */
#define RRPGE_SFI_NAME    0xC001U
/** Source file does not exists. */
#define RRPGE_SFI_SNEXIST 0xC002U
/** Target file exists (for file moves). */
#define RRPGE_SFI_TEXIST  0xC003U
/** Target file exists, but it is not writeable. */
#define RRPGE_SFI_TNWRITE 0xC004U
/** Source file exists, but can not be renamed or deleted. */
#define RRPGE_SFI_SNMOVE  0xC005U
/** Can not create the file or directory. */
#define RRPGE_SFI_NCREATE 0xC006U
/** Can not write more data into the file. */
#define RRPGE_SFI_NWRMORE 0xC007U
/** \} */



/** Maximal callback ID plus one (may be used to allocate related storage) */
#define RRPGE_CB_IDRANGE       21U



/**
**  \brief     Kernel task: Load binary data.
**
**  Requests an area from the application binary.
*/
#define RRPGE_CB_LOADBIN       0U
/**
**  \brief     Extra parameters for Load binary data.
*/
typedef struct{
 rrpge_uint32        sow; /**< The source (word) offset to load from. */
 rrpge_uint32        scw; /**< Number of words to load */
 rrpge_uint16*       buf; /**< Target area to fill. */
}rrpge_cbp_loadbin_t;



/**
**  \brief     Kernel task: Load from file.
**
**  Loads an area from a file. Note that the file name may contain arbitrary
**  data, it is the host's responsibility to check it for it's file name
**  constraints. It may not even have a terminator.
*/
#define RRPGE_CB_LOAD          1U
/**
**  \brief     Extra parameters for Load from file.
*/
typedef struct{
 rrpge_uint32        sob; /**< The source (byte) offset in the file to load from. */
 rrpge_uint32        scb; /**< The number of bytes to load from the file (0 - 16383). */
 rrpge_uint16*       buf; /**< The memory area to fill in, using Big Endian byte order. */
 rrpge_uint16 const* nam; /**< The file name (Big Endian byte order). */
 rrpge_uint32        ncw; /**< Size of file name in words. */
}rrpge_cbp_load_t;



/**
**  \brief     Kernel task: Save to file.
**
**  Saves an area into a file. Note that the file name may contain arbitrary
**  data, it is the host's responsibility to check it for it's file name
**  constraints. It may not even have a terminator.
*/
#define RRPGE_CB_SAVE          2U
/**
**  \brief     Extra parameters for Save to file.
*/
typedef struct{
 rrpge_uint32        tob; /**< The target (byte) offset in the file to write from. */
 rrpge_uint32        tcb; /**< The number of bytes to write into the file (0 - 16383). */
 rrpge_uint16 const* buf; /**< The memory area to load data from, using Big Endian byte order. */
 rrpge_uint16 const* nam; /**< The file name (Big Endian byte order). */
 rrpge_uint32        ncw; /**< Size of file name in words. */
}rrpge_cbp_save_t;



/**
**  \brief     Kernel task: Find next file.
**
**  Finds the next file by file name. Note that the file name may contain
**  arbitrary data. It may not even have a terminator.
*/
#define RRPGE_CB_NEXT          3U
/**
**  \brief     Extra parameters for Find next file.
*/
typedef struct{
 rrpge_uint16*       nam; /**< The file name (source and target, Big Endian byte order). */
 rrpge_uint32        ncw; /**< Size of file name in words. */
}rrpge_cbp_next_t;



/**
**  \brief     Kernel task: Move file.
**
**  Moves or deletes a file. Deletes if the target file name is empty (begins
**  with a terminator). Note that the file names may contain arbitrary data.
**  They may not even have a terminator.
*/
#define RRPGE_CB_MOVE          4U
/**
**  \brief     Extra parameters for Move file.
*/
typedef struct{
 rrpge_uint16 const* snm; /**< The source file name (Big Endian byte order). */
 rrpge_uint16 const* tnm; /**< The target file name (Big Endian byte order). */
 rrpge_uint32        scw; /**< Size of source name in words. */
 rrpge_uint32        tcw; /**< Size of target name in words. */
}rrpge_cbp_move_t;



/**
**  \brief     Subroutine: Set palette entry.
**
**  Requests setting a palette entry. If the host displays on a true
**  palettized mode, the palette set should come in effect with a new
**  displayed frame only (it should be ensured that the palette setting does
**  not affect any previous output). Note that this callback only executes
**  from rrpge_run() like the rest, and only when the running application
**  explicitly asks for palette change through a kernel call. On init, state
**  load, reset, application state update or any other (such as context
**  switching) action the host has to reconstruct the palette on it's own
**  using the color table in the application state (RRPGE_STA_PAL, 256 words).
*/
#define RRPGE_CB_SETPAL        5U
/**
**  \brief     Extra parameters for Set palette entry.
*/
typedef struct{
 rrpge_uint32        id;  /**< Color index (0 - 255). */
 rrpge_uint32        col; /**< Color in 4-4-4 RGB (Red high) encoding. */
}rrpge_cbp_setpal_t;



/**
**  \brief     Subroutine: Change video mode.
**
**  Sets the video mode (640x400; 4bit or 320x400; 8bit). Note that the
**  callback only executes from rrpge_run() like the rest, and only when the
**  running application explicitly asks for mode change through a kernel call.
**  On init, state load, reset, application state update or any other (such as
**  context switching) action the host has to set the appropriate mode on it's
**  own using rrpge_getvidmode().
*/
#define RRPGE_CB_MODE          6U
/**
**  \brief     Extra parameters for Change video mode.
*/
typedef struct{
 rrpge_uint32        mod; /**< Requested video mode (0: 640x400; 4bit, 1: 320x400; 8bit). */
}rrpge_cbp_mode_t;



/**
**  \brief     Function: Get device properties.
**
**  Returns the properties of the given input device. The return value has to
**  be formatted according to the specification (returning the device type and
**  optionally the device it maps to if any).
*/
#define RRPGE_CB_GETPROPS      8U
/**
**  \brief     Extra parameters for Get device properties.
*/
typedef struct{
 rrpge_uint32        dev; /**< The device to query. */
}rrpge_cbp_getprops_t;



/**
**  \brief     Subroutine: Drop device.
**
**  Drops a device indicating the application no longer uses it.
*/
#define RRPGE_CB_DROPDEV      9U
/**
**  \brief     Extra parameters for Drop device.
*/
typedef struct{
 rrpge_uint32        dev; /**< The device to drop. */
}rrpge_cbp_dropdev_t;



/**
**  \brief     Function: Get digital input description symbols.
**
**  Returns a descriptive symbol for the given digital input as an UTF32
**  character code or a special code according to the specification. May
**  return 0 indicating the input does not exist.
*/
#define RRPGE_CB_GETDIDESC     10U
/**
**  \brief     Extra parameters for Get digital input description symbols.
*/
typedef struct{
 rrpge_uint32        dev; /**< The device to query. */
 rrpge_uint32        inp; /**< Input number to query (group * 16 + input id). */
}rrpge_cbp_getdidesc_t;



/**
**  \brief     Function: Get digital inputs.
**
**  Returns digital input states for an input group of a given device (16
**  digital inputs per group).
*/
#define RRPGE_CB_GETDI         11U
/**
**  \brief     Extra parameters for Get digital inputs.
*/
typedef struct{
 rrpge_uint32        dev; /**< The device to query. */
 rrpge_uint32        ing; /**< Input group to query. */
}rrpge_cbp_getdi_t;



/**
**  \brief     Function: Get analog inputs.
**
**  Returns analog input state for an analog input of a given device. The
**  return is 16 bits 2's complement.
*/
#define RRPGE_CB_GETAI         12U
/**
**  \brief     Extra parameters for Get analog inputs.
*/
typedef struct{
 rrpge_uint32        dev; /**< The device to query. */
 rrpge_uint32        inp; /**< Input to query. */
}rrpge_cbp_getai_t;



/**
**  \brief     Function: Pop text FIFO.
**
**  Pops the last UTF32 character or control code off the text FIFO of a text
**  input device.
*/
#define RRPGE_CB_POPCHAR       13U
/**
**  \brief     Extra parameters for Pop text FIFO.
*/
typedef struct{
 rrpge_uint32        dev; /**< The device to query. */
}rrpge_cbp_popchar_t;



/**
**  \brief     Function: Return area activity.
**
**  Requests checking whether the given input device is activating the passed
**  area. The return must be formatted according with the 0x0425: Return area
**  activity kernel call's return (lowest bit: set if activated; next bit: set
**  if the device hovers over the area).
**
**  A default implementation for this function is provided in the library
**  which is capable to request this information for mice and touch devices
**  using the appropriate get digital inputs and get analog input calls. It
**  only needs to be implemented if multi-touch support is desired.
*/
#define RRPGE_CB_CHECKAREA     14U
/**
**  \brief     Extra parameters for Define touch sensitive area.
*/
typedef struct{
 rrpge_uint32        dev; /**< The device to query. */
 rrpge_uint32          x; /**< Upper left X (0 - 639). */
 rrpge_uint32          y; /**< Upper left Y (0 - 399). */
 rrpge_uint32          w; /**< Width (0 - 639). */
 rrpge_uint32          h; /**< Height (0 - 399). */
}rrpge_cbp_checkarea_t;



/**
**  \brief     Subroutine: Get local users.
**
**  Requests local users (user using the device), up to four. A 32 word target
**  area is provided to fill in with these User IDs. The area is zeroed out
**  before calling the callback.
*/
#define RRPGE_CB_GETLOCAL      15U
/**
**  \brief     Extra parameters for Get local users.
*/
typedef struct{
 rrpge_uint16*       buf; /**< Target area to load User ID's into (32 words) */
}rrpge_cbp_getlocal_t;



/**
**  \brief     Kernel task: Get UTF-8 representation of User ID.
**
**  Servicing this call may require network queries when the host is connected
**  to request user names.
*/
#define RRPGE_CB_GETUTF        16U
/**
**  \brief     Extra parameters for Read UTF-8 representation of user.
*/
typedef struct{
 rrpge_uint16 const*  id; /**< User ID (8 words). */
 rrpge_uint16*       nam; /**< Area to fill with the main part of user name. */
 rrpge_uint32        ncw; /**< Size of user name main part area in words. */
 rrpge_uint16*       ext; /**< Area to fill with the extended part of user name. */
 rrpge_uint32        ecw; /**< Size of user name extended part area in words. */
}rrpge_cbp_getutf_t;



/**
**  \brief     Function: Get user preferred language.
**
**  Asks for the preferred languages of an user. In the return up to 4
**  characters may occur, high first, containing a language code. Zero
**  indicates no language.
*/
#define RRPGE_CB_GETLANG       17U
/**
**  \brief     Extra parameters for Get user preferred language.
*/
typedef struct{
 rrpge_uint32        lno; /**< Language number to return (0: most preferred) */
}rrpge_cbp_getlang_t;



/**
**  \brief     Function: Get user preferred colors.
**
**  Ask for user's preferred colors. High word of the return is the foreground
**  in 4-4-4 RGB (high 4 bits zero), low word is the background color. Set the
**  two the same (return zero) to indicate no preferred colors are set by the
**  user.
*/
#define RRPGE_CB_GETCOLORS     18U



/**
**  \brief     Kernel task: Send out network packet.
**
**  Sends a network packet to the given user. The host has to determine the
**  routing. If a local user name and an user name from the network collides,
**  it must prefer sending to the network user (so it may be possible to
**  connect two RRPGE systems with no user name set, and play).
*/
#define RRPGE_CB_SEND          19U
/**
**  \brief     Extra parameters for Send out network packet.
*/
typedef struct{
 rrpge_uint16 const*  id; /**< User ID to target. */
 rrpge_uint32        bcw; /**< Number of words to send. */
 rrpge_uint16 const* buf; /**< The source data to send. */
}rrpge_cbp_send_t;



/**
**  \brief     Kernel task: List accessible network users.
**
**  The host should gather network users in incremential order by ID starting
**  with the passed ID. It must only return users who are running the same
**  app. and report they are available for connection.
*/
#define RRPGE_CB_LISTUSERS     20U
/**
**  \brief     Extra parameters for List accessible network users.
*/
typedef struct{
 rrpge_uint16 const*  id; /**< User ID to start the list from. */
 rrpge_uint32        bcu; /**< Maximal number of user ID's to receive (8 words each). */
 rrpge_uint16*       buf; /**< The memory to fill in. */
}rrpge_cbp_listusers_t;



/**
**  \brief     Default implementation for Return area activity.
**
**  The default implementation of Return area activity (RRPGE_CB_CHECKAREA).
**  When implementing a handler for supporting multi-touch, this may be called
**  from that handler to process any other device.
**
**  \param[in]   hnd   Emulation instance the callback is called for.
**  \param[in]   par   Extra parameters (rrpge_cbp_checkarea_t).
**  \return            Result of function to be written into C:X3.
*/
rrpge_uint32 rrpge_cb_checkarea(rrpge_object_t* hnd, const void* par);


#endif
