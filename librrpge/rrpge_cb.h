/**
**  \file
**  \brief     LibRRPGE standard header package - callbacks and related
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
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
**  \brief     Kernel task: Load binary data.
**
**  Requests a page from the application binary.
*/
#define RRPGE_CB_LOADBIN       0U
/**
**  \brief     Extra parameters for Load binary data.
*/
typedef struct{
 rrpge_uint32   spg;   /**< The source page to supply. */
 rrpge_uint16*  buf;   /**< The memory page to fill in (4096 elements). */
}rrpge_cbp_loadbin_t;



/**
**  \brief     Kernel task: Load nonvolatile save.
**
**  Loads a nonvolatile save. Note that the library zeroes out the target
**  pages before this callback, so it is not necessary to do so if the save is
**  smaller than requested or it is not possible to load it.
*/
#define RRPGE_CB_LOADNV        1U
/**
**  \brief     Extra parameters for Load nonvolatile save.
*/
typedef struct{
 rrpge_uint16   id[3]; /**< Save ID */
 rrpge_uint16   no;    /**< Maximal number of pages (4096 words) to fill */
 rrpge_uint16*  buf;   /**< The memory to fill in. */
}rrpge_cbp_loadnv_t;



/**
**  \brief     Kernel task: Save nonvolatile save.
**
**  Saves a nonvolatile save. The first 96 bytes of the save must be generated
**  from the application header (replacing only the beginning "RPA" to "RPN").
**  This replace has to be performed by the host!
*/
#define RRPGE_CB_SAVENV        2U
/**
**  \brief     Extra parameters for Save nonvolatile save.
*/
typedef struct{
 rrpge_uint16   id[3]; /**< Save ID */
 rrpge_uint16   no;    /**< Number of pages (4096 words) to save */
 rrpge_uint16
        const*  buf;   /**< The source data. */
}rrpge_cbp_savenv_t;



/**
**  \brief     Kernel task: List nonvolatile saves.
**
**  Lists existing nonvolatile saves. Note that the library zeroes out the
**  target pages before this callback, so it is not necessary to do so if the
**  list is smaller than 1024 elements. Also note that saves can not have a
**  size of zero, if such exists on the host, it should be treated
**  nonexistent.
*/
#define RRPGE_CB_LISTNV        3U
/**
**  \brief     Extra parameters for Load nonvolatile save.
*/
typedef struct{
 rrpge_uint16*  buf;   /**< The memory to fill in (4096 words). */
}rrpge_cbp_listnv_t;



/**
**  \brief     Function: Load arbitrary file.
**
**  This callback needs some kind of file selector to be presented for proper
**  servicing. If it is not economical to do this from the callback, the host
**  may note this and catch the RRPGE_HLT_CALLBACK cause to service it. Note
**  that the library clears the target area to zero before calling this, so it
**  is not necessary to do so if the file is smaller, can not be loaded, or
**  the load was cancelled.
*/
#define RRPGE_CB_LOADFILE      4U
/**
**  \brief     Extra parameters for Load arbitrary file.
*/
typedef struct{
 rrpge_uint32   no;    /**< Maximal number of pages (4096 words) to fill */
 rrpge_uint16*  buf;   /**< The memory to fill in (4096 words). */
}rrpge_cbp_loadfile_t;



/**
**  \brief     Function: Save arbitrary file.
**
**  This callback needs some kind of file selector to be presented for proper
**  servicing. If it is not economical to do this from the callback, the host
**  may note this and catch the RRPGE_HLT_CALLBACK cause to service it. The
**  bytes to save parameter might not be used if the host only supports page
**  size blocks.
*/
#define RRPGE_CB_SAVEFILE      5U
/**
**  \brief     Extra parameters for Load arbitrary file.
*/
typedef struct{
 rrpge_uint16   no;    /**< Number of pages to save (1 - 224). */
 rrpge_uint16   byt;   /**< Bytes to save from last page (1 - 8192). */
 rrpge_uint16
        const*  buf;   /**< The memory to read from. */
}rrpge_cbp_savefile_t;



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
#define RRPGE_CB_SETCOLOR      6U
/**
**  \brief     Extra parameters for Set palette entry.
*/
typedef struct{
 rrpge_uint16   id;    /**< Color index (0 - 255). */
 rrpge_uint16   col;   /**< Color in 5-6-5 RGB (Red high) encoding. */
}rrpge_cbp_setcolor_t;



/**
**  \brief     Inpud device functions: Common parameter structure
**
**  All input device functions share the same parameter layout: they take one
**  parameter selecting the device to query.
*/
typedef struct{
 rrpge_uint32   sel;   /**< Device selector (0 - 15). */
}rrpge_cbp_inputcom_t;



/**
**  \brief     Function: Digital joystick: Read device availability.
**
**  Reads current availability of digital joysticks. Has no parameters.
*/
#define RRPGE_CB_IDIG_AVA      7U



/**
**  \brief     Function: Digital joystick: Read controls.
**
**  Reads active controls of the device. Uses rrpge_cbp_inputcomm_t for
**  parameters.
*/
#define RRPGE_CB_IDIG_CTR      8U



/**
**  \brief     Function: Analog joystick: Read device availability.
**
**  Reads current availability of analog joysticks. Has no parameters.
*/
#define RRPGE_CB_IANA_AVA      9U



/**
**  \brief     Function: Analog joystick: Read controls.
**
**  Reads active controls of the device. Uses rrpge_cbp_inputcomm_t for
**  parameters.
*/
#define RRPGE_CB_IANA_CTR     10U



/**
**  \brief     Function: Analog joystick: Read position.
**
**  Reads position (directions) of the analog joystick. Uses
**  rrpge_cbp_inputcomm_t for parameters.
*/
#define RRPGE_CB_IANA_POS     11U



/**
**  \brief     Function: Mice: Read device availability.
**
**  Reads current availability of mice. Has no parameters.
*/
#define RRPGE_CB_IMOU_AVA     12U



/**
**  \brief     Function: Mice: Read controls.
**
**  Reads active controls of the device. Uses rrpge_cbp_inputcomm_t for
**  parameters.
*/
#define RRPGE_CB_IMOU_CTR     13U



/**
**  \brief     Function: Mice: Read position.
**
**  Reads position of the mouse. Uses rrpge_cbp_inputcomm_t for parameters.
*/
#define RRPGE_CB_IMOU_POS     14U



/**
**  \brief     Function: Mice: Read cursor requirement.
**
**  Reads whether a cursor is necessary to be displayed for the given mouse.
**  Uses rrpge_cbp_inputcomm_t for parameters.
*/
#define RRPGE_CB_IMOU_CUR     15U



/**
**  \brief     Function: Text: Read device availability.
**
**  Reads current availability of text input devices. Has no parameters.
*/
#define RRPGE_CB_ITXT_AVA     16U



/**
**  \brief     Function: Text: Read input fifo.
**
**  Reads next character from text device's fifo. Uses rrpge_cbp_inputcomm_t
**  for parameters.
*/
#define RRPGE_CB_ITXT_RFI     17U



/**
**  \brief     Kernel task: Read UTF-8 representation of user.
**
**  Servicing this call may require network queries when the host is connected
**  to request user names. Note that the library zeroes out the target area
**  before this callback, so it is not necessary to do so if the name is
**  shorter than requested or it is not possible to retrieve it. The host must
**  take care to leave a terminating zero word intact for each part (at
**  buf+127 and buf+255).
*/
#define RRPGE_CB_GETUUTF      18U
/**
**  \brief     Extra parameters for Read UTF-8 representation of user.
*/
typedef struct{
 rrpge_uint16   id[8]; /**< User ID. */
 rrpge_uint16*  buf;   /**< The memory to fill in (256 elements). */
}rrpge_cbp_getuutf_t;



/**
**  \brief     Kernel task: Send out network packet.
**
**  Sends a network packet to the given user. The host has to determine the
**  routing. If a local user name and an user name from the network collides,
**  it must prefer sending to the network user (so it may be possible to
**  connect two RRPGE systems with no user name set, and play).
*/
#define RRPGE_CB_SENDTO       19U
/**
**  \brief     Extra parameters for Send out network packet.
*/
typedef struct{
 rrpge_uint16   id[8]; /**< User ID to target. */
 rrpge_uint32   no;    /**< Number of words to send (1 - 4096). */
 rrpge_uint16
        const*  buf;   /**< The source data to send. */
}rrpge_cbp_sendto_t;



/**
**  \brief     Kernel task: List accessible network users.
**
**  The host should gather network users in incremential order by ID starting
**  with the passed ID. It must only return users who are running the same
**  app. and report they are available for connection.
*/
#define RRPGE_CB_LISTUSER     20U
/**
**  \brief     Extra parameters for List accessible network users.
*/
typedef struct{
 rrpge_uint16   id[8]; /**< User ID to start the list from. */
 rrpge_uint16*  buf;   /**< The memory to fill in (4096 words). */
}rrpge_cbp_listuser_t;



#endif
