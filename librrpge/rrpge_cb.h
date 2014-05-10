/**
**  \file
**  \brief     LibRRPGE standard header package - callbacks and related
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.10
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
**  kernel function is one which has return value (either produced in 'A' or
**  'C:A'). If the host wants to perform this outside the callback, it has to
**  catch the RRPGE_HLT_CALLBACK halt cause, check for these, service, and
**  then has to write the 'A' or 'C:A' registers manually in the ROPD before
**  continuing emulation. When servicing the callback proper the high word of
**  the return goes in 'C' (only if the particular kernel call is specified to
**  write it), and the low word in 'A'.
**
**  \param[in]   hnd   Emulation instance the callback is called for.
**  \param[in]   par   Extra parameters for the particular callback.
**  \return            Result of function to be written into C:A.
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
**  Requests a page from the application binary.
*/
#define RRPGE_CB_LOADBIN       0U
/**
**  \brief     Extra parameters for Load binary data.
*/
typedef struct{
 rrpge_uint32        spg; /**< The source page to supply. */
 rrpge_uint16*       buf; /**< The memory page to fill in (4096 elements). */
}rrpge_cbp_loadbin_t;



/**
**  \brief     Kernel task: Load page from file.
**
**  Loads a page from a file. The library clears the target page to zero
**  before calling this callback. Note that the file name may contain
**  arbitrary data, it is the host's responsibility to check it for it's file
**  name constraints. It may not even have a terminator.
*/
#define RRPGE_CB_LOAD          1U
/**
**  \brief     Extra parameters for Load page from file.
*/
typedef struct{
 rrpge_uint32        spg; /**< The page in the file to load. */
 rrpge_uint32        sno; /**< The number of bytes to load from the file (0 - 8192). */
 rrpge_uint16*       buf; /**< The memory page to fill in (4096 elements). */
 rrpge_uint16 const* nam; /**< The file name (128 words: 256 bytes, might not terminate). */
}rrpge_cbp_load_t;



/**
**  \brief     Kernel task: Save page to file.
**
**  Saves a page into a file. Note that the file name may contain arbitrary
**  data, it is the host's responsibility to check it for it's file name
**  constraints. It may not even have a terminator.
*/
#define RRPGE_CB_SAVE          2U
/**
**  \brief     Extra parameters for Save page to file.
*/
typedef struct{
 rrpge_uint32        tpg; /**< The page in the file to write. */
 rrpge_uint32        tno; /**< The number of bytes to write into the file (0 - 8192). */
 rrpge_uint16 const* buf; /**< The memory page to load data from (4096 elements). */
 rrpge_uint16 const* nam; /**< The file name (128 words: 256 bytes, might not terminate). */
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
 rrpge_uint16*       nam; /**< The file name (128 words: 256 bytes, might not terminate). */
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
**  \brief     Extra parameters for Find next file.
*/
typedef struct{
 rrpge_uint16 const* snm; /**< The source file name (128 words: 256 bytes, might not terminate). */
 rrpge_uint16 const* tnm; /**< The target file name (128 words: 256 bytes, might not terminate). */
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
**  load, reset, ROPD page update or any other (such as context switching)
**  action the host has to reconstruct the palette on it's own using the color
**  table in the ROPD (0xC00 - 0xCFF).
*/
#define RRPGE_CB_SETPAL        5U
/**
**  \brief     Extra parameters for Set palette entry.
*/
typedef struct{
 rrpge_uint32        id;  /**< Color index (0 - 255). */
 rrpge_uint32        col; /**< Color in 5-6-5 RGB (Red high) encoding. */
}rrpge_cbp_setpal_t;



/**
**  \brief     Subroutine: Change video mode.
**
**  Sets the video mode (640x400; 4bit or 320x400; 8bit). Note that the
**  callback only executes from rrpge_run() like the rest, and only when the
**  running application explicitly asks for mode change through a kernel call.
**  On init, state load, reset, ROPD page update or any other (such as context
**  switching) action the host has to set the appropriate mode on it's own
**  using the mode field in the ROPD (0xD57).
*/
#define RRPGE_CB_MODE          6U
/**
**  \brief     Extra parameters for Change video mode.
*/
typedef struct{
 rrpge_uint32        mod; /**< Requested video mode (0: 640x400; 4bit, 1: 320x400; 8bit). */
}rrpge_cbp_mode_t;



/**
**  \brief     Function: Get input device availability.
**
**  Returns which input devices are available from the possible 16 devices.
**  Bit 0 of the return value corresponds to device 0, set if available.
*/
#define RRPGE_CB_GETDEV        7U



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
**  \brief     Function: Get digital input description symbols.
**
**  Returns a descriptive symbol for the given digital input as an UTF32
**  character code or a special code according to the specification. May
**  return 0 indicating the input does not exist.
*/
#define RRPGE_CB_GETDIDESC     9U
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
**  \brief     Subroutine: Define touch sensitive area.
**
**  Requests defining or redefining a touch sensitive area. The parameters are
**  sanitized by the library (constrained between 0 and 639 for X, 0 and 399
**  for Y). If the width or height is zero, the touch sensitive area has to be
**  disabled. Note that this is only called back by rrpge_run(), when loading
**  a state the touch sensitive areas have to be set up from the ROPD. On init
**  and reset they need to be disabled. This callback needs to be implemented
**  even if the host has no touch device (but has a mouse) to support buttons
**  specified as touch areas.
*/
#define RRPGE_CB_SETTOUCH      14U
/**
**  \brief     Extra parameters for Define touch sensitive area.
*/
typedef struct{
 rrpge_uint32        aid; /**< Area ID to (re)define (0 - 15). */
 rrpge_uint32        x;   /**< Upper left X (0 - 639). */
 rrpge_uint32        y;   /**< Upper left Y (0 - 399). */
 rrpge_uint32        w;   /**< Width (0 - 639). */
 rrpge_uint32        h;   /**< Height (0 - 399). */
}rrpge_cbp_settouch_t;



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
}rrpge_cb_getlocal_t;



/**
**  \brief     Kernel task: Get UTF-8 representation of User ID.
**
**  Servicing this call may require network queries when the host is connected
**  to request user names. Note that the library zeroes out the target area
**  before this callback, so it is not necessary to do so if the name is
**  shorter than requested or it is not possible to retrieve it. The host must
**  take care to leave a terminating zero word intact for each part (at
**  buf+127 and buf+255).
*/
#define RRPGE_CB_GETUTF        16U
/**
**  \brief     Extra parameters for Read UTF-8 representation of user.
*/
typedef struct{
 rrpge_uint16      id[8]; /**< User ID. */
 rrpge_uint16*       buf; /**< The memory to fill in (256 elements). */
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
 rrpge_uint16      id[8]; /**< User ID to target. */
 rrpge_uint32        no;  /**< Number of words to send (1 - 4096). */
 rrpge_uint16 const* buf; /**< The source data to send. */
}rrpge_cbp_send_t;



/**
**  \brief     Kernel task: List accessible network users.
**
**  The host should gather network users in incremential order by ID starting
**  with the passed ID. It must only return users who are running the same
**  app. and report they are available for connection. The library zeroes out
**  the area before calling the callback.
*/
#define RRPGE_CB_LISTUSERS     20U
/**
**  \brief     Extra parameters for List accessible network users.
*/
typedef struct{
 rrpge_uint16      id[8]; /**< User ID to start the list from. */
 rrpge_uint16*       buf; /**< The memory to fill in (4096 words). */
}rrpge_cbp_listusers_t;



#endif
