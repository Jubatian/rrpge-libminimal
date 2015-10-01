/**
**  \file
**  \brief     LibRRPGE standard header package - callbacks and related
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.04
*/


#ifndef RRPGE_CB_H
#define RRPGE_CB_H


#include "rrpge_tp.h"



/**
**  \brief     Graphic line output.
**
**  This is called when a graphic line is completed and rendered. It is not
**  called in frames where rendering is turned off. The timing of the calls
**  may not match that of what might be deducted from the cycle counts or the
**  exported state, however the order of lines and their occurence related to
**  frame boundaries (RRPGE_HLT_FRAME halt causes) are strictly kept. Note
**  that this callback does not produce halt cause on execution.
**
**  \param[in]   hnd   Emulation instance the callback is called for.
**  \param[in]   ln    The number of the line rendered (0 - 399).
**  \param[in]   buf   The contents of the line (640 elements).
*/
typedef void rrpge_cb_line_t (rrpge_object_t* hnd, rrpge_iuint ln, rrpge_uint8 const* buf);



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
typedef void rrpge_cb_kcalltsk_t (rrpge_object_t* hnd, rrpge_iuint tsh, const void* par);



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
typedef rrpge_iuint rrpge_cb_kcallfun_t (rrpge_object_t* hnd, const void* par);



/**
**  \brief     Kernel task callback descriptor.
**
**  Binds a kernel task callback to callback type.
*/
typedef struct{
 rrpge_iuint              id;          /**< Callback ID (type) */
 rrpge_cb_kcalltsk_t*     cb;          /**< Callback function */
}rrpge_cbd_tsk_t;



/**
**  \brief     Kernel subroutine callback descriptor.
**
**  Binds a kernel subroutine callback to callback type.
*/
typedef struct{
 rrpge_iuint              id;          /**< Callback ID (type) */
 rrpge_cb_kcallsub_t*     cb;          /**< Callback function */
}rrpge_cbd_sub_t;



/**
**  \brief     Kernel function callback descriptor.
**
**  Binds a kernel function callback to callback type.
*/
typedef struct{
 rrpge_iuint              id;          /**< Callback ID (type) */
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
 rrpge_iuint              tsk_n;       /**< Number of task callbacks passed */
 rrpge_cbd_tsk_t const*   tsk_d;       /**< Task callback descriptors */
 rrpge_iuint              sub_n;       /**< Number of sub. callbacks passed */
 rrpge_cbd_sub_t const*   sub_d;       /**< Subroutine callback descriptors */
 rrpge_iuint              fun_n;       /**< Number of fun. callbacks passed */
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
#define RRPGE_CB_IDRANGE       24U



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
 rrpge_iuint         sow; /**< The source (word) offset to load from. */
 rrpge_iuint         scw; /**< Number of words to load */
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
 rrpge_iuint         sob; /**< The source (byte) offset in the file to load from. */
 rrpge_iuint         scb; /**< The number of bytes to load from the file (0 - 16383). */
 rrpge_uint16*       buf; /**< The memory area to fill in, using Big Endian byte order. */
 rrpge_uint16 const* nam; /**< The file name (Big Endian byte order). */
 rrpge_iuint         ncw; /**< Size of file name in words. */
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
 rrpge_iuint         tob; /**< The target (byte) offset in the file to write from. */
 rrpge_iuint         tcb; /**< The number of bytes to write into the file (0 - 16383). */
 rrpge_uint16 const* buf; /**< The memory area to load data from, using Big Endian byte order. */
 rrpge_uint16 const* nam; /**< The file name (Big Endian byte order). */
 rrpge_iuint         ncw; /**< Size of file name in words. */
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
 rrpge_iuint         ncw; /**< Size of file name in words. */
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
 rrpge_iuint         scw; /**< Size of source name in words. */
 rrpge_iuint         tcw; /**< Size of target name in words. */
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
 rrpge_iuint         id;  /**< Color index (0 - 255). */
 rrpge_iuint         col; /**< Color in 4-4-4 RGB (Red high) encoding. */
}rrpge_cbp_setpal_t;



/**
**  \brief     Subroutine: Set stereoscopic 3D.
**
**  Sets stereoscopic 3D output properties. The host, if supports some kind of
**  3D device, may use this information to post-process the graphics output of
**  the application for it. Bit 0 of the parameter if set, indicates that the
**  application is outputting stereoscopic 3D. Bits 1-2 indicate the vertical
**  used pixels: 0: 400 pixels, 1: 320 pixels, 2: 240 pixels, 3: 200 pixels.
**  The stereoscopic content is output in cross-eyed format (image for right
**  eye on the left half; image for left eye on the right half), vertically
**  centered. On init, state load, reset, application state update or any
**  other (such as context switching) action the host has to query this status
**  on it's own using rrpge_getst3dprops().
*/
#define RRPGE_CB_SETST3D       7U
/**
**  \brief     Extra parameters for Set stereoscopic 3D.
*/
typedef struct{
 rrpge_iuint         mod; /**< Stereoscopic 3D output parameters. */
}rrpge_cbp_setst3d_t;



/**
**  \brief     Subroutine: Get local users.
**
**  Requests local users (user using the device), up to four. A 32 word target
**  area is provided to fill in with these User IDs. The area is zeroed out
**  before calling the callback.
*/
#define RRPGE_CB_GETLOCAL      17U
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
#define RRPGE_CB_GETUTF        18U
/**
**  \brief     Extra parameters for Read UTF-8 representation of user.
*/
typedef struct{
 rrpge_uint16 const*  id; /**< User ID (8 words). */
 rrpge_uint16*       nam; /**< Area to fill with the main part of user name. */
 rrpge_iuint         ncw; /**< Size of user name main part area in words. */
 rrpge_uint16*       ext; /**< Area to fill with the extended part of user name. */
 rrpge_iuint         ecw; /**< Size of user name extended part area in words. */
}rrpge_cbp_getutf_t;



/**
**  \brief     Function: Get user preferred language.
**
**  Asks for the preferred languages of an user. In the return up to 4
**  characters may occur, high first, containing a language code. Zero
**  indicates no language.
*/
#define RRPGE_CB_GETLANG       19U
/**
**  \brief     Extra parameters for Get user preferred language.
*/
typedef struct{
 rrpge_iuint         lno; /**< Language number to return (0: most preferred) */
}rrpge_cbp_getlang_t;



/**
**  \brief     Function: Get user preferred colors.
**
**  Ask for user's preferred colors. High word of the return is the foreground
**  in 4-4-4 RGB (high 4 bits zero), low word is the background color. Set the
**  two the same (return zero) to indicate no preferred colors are set by the
**  user.
*/
#define RRPGE_CB_GETCOLORS     20U



/**
**  \brief     Function: Get user stereoscopic 3D preference.
**
**  Returns whether the user may accept stereoscopic 3D content or not. The
**  return is either 0 or 1, the latter indicating that the application may
**  output stereoscopic 3D content.
*/
#define RRPGE_CB_GETST3D       21U



/**
**  \brief     Kernel task: Send out network packet.
**
**  Sends a network packet to the given user. The host has to determine the
**  routing. If a local user name and an user name from the network collides,
**  it must prefer sending to the network user (so it may be possible to
**  connect two RRPGE systems with no user name set, and play).
*/
#define RRPGE_CB_SEND          22U
/**
**  \brief     Extra parameters for Send out network packet.
*/
typedef struct{
 rrpge_uint16 const*  id; /**< User ID to target. */
 rrpge_iuint         bcw; /**< Number of words to send. */
 rrpge_uint16 const* buf; /**< The source data to send. */
}rrpge_cbp_send_t;



/**
**  \brief     Kernel task: List accessible network users.
**
**  The host should gather network users in incremential order by ID starting
**  with the passed ID. It must only return users who are running the same
**  app. and report they are available for connection.
*/
#define RRPGE_CB_LISTUSERS     23U
/**
**  \brief     Extra parameters for List accessible network users.
*/
typedef struct{
 rrpge_uint16 const*  id; /**< User ID to start the list from. */
 rrpge_iuint         bcu; /**< Maximal number of user ID's to receive (8 words each). */
 rrpge_uint16*       buf; /**< The memory to fill in. */
}rrpge_cbp_listusers_t;


#endif
