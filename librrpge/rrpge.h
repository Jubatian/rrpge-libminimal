/**
**  \file
**  \brief     LibRRPGE standard header package - main header
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.03
*/


#ifndef RRPGE_H
#define RRPGE_H


#include "rrpge_tp.h"
#include "rrpge_aq.h"
#include "rrpge_cb.h"
#include "rrpge_ch.h"
#include "rrpge_sr.h"
#include "rrpge_pr.h"
#include "rrpge_db.h"



/**
**  \brief     Initializes the emulator library.
**
**  This must be called before any other RRPGE library function to set up the
**  memory allocator. If omitted, every routine requiring allocation will
**  return failure.
**
**  \param[in]   alc   Allocator function.
**  \param[in]   fre   Free function.
*/
void rrpge_init_lib(rrpge_malloc_t* alc, rrpge_free_t* fre);



/**
**  \brief     Deletes any emulator library allocated object
**
**  This should be used to free any object allocated by the library.
**
**  \param[in]   obj   The object to free.
*/
void rrpge_delete(void* obj);



/**
**  \brief     Initializes emulator over a given callback set.
**
**  Initializes the emulator instance over the passed set of callbacks. It
**  only attempts to allocate memory for the instance, returning NULL if this
**  is not possible. The initialization has to be completed by calling
**  rrpge_init_run() until it finishes proper.
**
**  \param[in]   cb    Callback set filled with callbacks.
**  \return            New emulator object or NULL if allocation failed.
*/
rrpge_object_t* rrpge_new_emu(rrpge_cbpack_t const* cb);



/**
**  \brief     Runs initialization to completion.
**
**  Using the RRPGE_CB_LOADBIN callback attempts to load the RRPGE application
**  initializing for a proper reset state. May return RRPGE_ERR_WAIT which is
**  normal if the host supports asynchronous binary loads. This case the
**  function should be called again later when likely more of the binary
**  becomes available. Other error codes may be returned if the application
**  can not be loaded for some problem with its format or the host doesn't
**  support all features it requests.
**
**  /param[in]   hnd   Emulator instance to work with.
**  /param[in]   tg    Initialization target. See \ref init_levels.
**  /return            0 on success, failure code otherwise.
*/
rrpge_iuint rrpge_init_run(rrpge_object_t* hnd, rrpge_iuint tg);



/**
**  \brief     Resets emulator.
**
**  Similar to calling rrpge_init_run() with RRPGE_INI_RESET, however this
**  method doesn't reload the application header, neither clears breakpoints,
**  only resuming the RRPGE system to the initial state. Only has effect if
**  the emulator was sufficiently initialized (reached RRPGE_INI_RESET).
**
**  /param[in]   hnd   Emulator instance to work with.
*/
void rrpge_reset(rrpge_object_t* hnd);



/**
**  \brief     Exports emulator state.
**
**  Serializes the emulator's state as described by the specification.
**  Allocates memory using the passed allocator, which should be freed after
**  use with rrpge_delete(). The size of the data is 4392960 (0x430800)
**  bytes.
**
**  /param[in]   hnd   Emulator instance to export.
**  /return            Exported data or NULL if allocation failed.
*/
rrpge_uint8 rrpge_export(rrpge_object_t* hnd);



/**
**  \brief     Imports emulator state.
**
**  Deserializes the passed emulator state. It can only succeed if the
**  matching application is loaded and is sufficiently initialized. Apart from
**  the requirement of initialization, the current state is irrelevant and
**  will be overridden: emulation will continue wherever the emulator state
**  was saved.
**
**  /param[in]   hnd   Emulator instance to import into.
**  /param[in]   sta   Emulator state to import (4392960 bytes).
**  /return            0 on success, failure code otherwise.
*/
rrpge_iuint rrpge_import(rrpge_object_t* hnd, rrpge_uint8 const* sta);



/**
**  \brief     Runs the emulator.
**
**  Continues emulation depending on the emulation mode (see \ref
**  rrpge_run_modes). The emulation proceeds until a halt cause is hit (except
**  in single step mode when it will return after processing a single
**  operation). Returns the count of cycles consumed during emulation. Note
**  that if the Program Counter points at a location set as breakpoint when
**  calling this, that breakpoint will be skipped (this allows continuing
**  emulation after hitting a breakpoint).
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   rmod  Running mode. See \ref rrpge_run_modes.
**  \return            Number of cycles emulated.
*/
rrpge_iuint rrpge_run(rrpge_object_t* hnd, rrpge_iuint rmod);



/**
**  \brief     Submits task result to the emulator.
**
**  If the emulator requests a task using a callback, upon completion the host
**  has to report back using this method. It may be called from the callback
**  if the host processes the request immediately not wanting to implement
**  asynchronous handling of tasks. The result code should be formed according
**  to the specification (task return code) of the appropriate kernel task.
**  Bit 15 is set by the library if necessary. Other out of range values are
**  transformed to a valid value by implementation defined means.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   tsh   Task handle acquired from the callback (0-15).
**  \param[in]   res   Result of task (low 15 bits used, bit 15 always set).
*/
void rrpge_taskend(rrpge_object_t* hnd, rrpge_iuint tsh, rrpge_iuint res);



/**
**  \brief     Register new input device.
**
**  Adds a new input device for serving the given device type, and return
**  identifier for it. Subsequently events from the device can be sent using
**  this identifier. Never fails: if too many devices are provided, it simply
**  ignores the excess ones (supports at least 16 devices), never forwarding
**  their events. Device substitution and combining is done by the emulator
**  library in accordance with the specification (so for example a keyboard
**  will provide a digital gamepad and text input device for requesting
**  applications).
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   typ   Types the new device serves (see \ref device_types).
**  \return            Device ID.
*/
rrpge_iuint rrpge_dev_add(rrpge_object_t* hnd, rrpge_iuint typ);



/**
**  \brief     Removes an input device.
**
**  Removes an input device from the pool of available devices. This may be
**  called for implementing hotplugging or dynamic emulator configuration
**  where a devices may be added or removed during runtime.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   dev   Device ID (from rrpge_dev_add()).
*/
void rrpge_dev_rem(rrpge_object_t* hnd, rrpge_iuint dev);



/**
**  \brief     Pushes an input event.
**
**  Submits an input event to the input event queue which might be consumed by
**  the application if it enabled the reception of the event. The event
**  message may have up to 8 members, which should be populated as defined by
**  the specification. For mice, press coordinates may be omitted: this case
**  the last sent coordinates will be used.
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   dev   Device ID (from rrpge_dev_add()).
**  \param[in]   emt   Event message type (as defined in the specification).
**  \param[in]   cnt   Count of data members (up to 8).
**  \param[in]   msg   Event message contents.
*/
void rrpge_dev_push(rrpge_object_t* hnd, rrpge_iuint dev,
                    rrpge_iuint emt, rrpge_iuint cnt, rrpge_uint16 const* msg);



/**
**  \brief     Submit received packet to the emulator.
**
**  If a network packet is received targeting the user running the
**  application, it should be submitted to the emulator using this method.
**  Note that the application may also expect packets when it is not available
**  for network connections. (The availability flag is only used to note that
**  the user is available for new connections).
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   id    User ID of the packet's sender (8 words).
**  \param[in]   buf   Packet contents.
**  \param[in]   len   Size of packet in word units.
*/
void rrpge_pushpacket(rrpge_object_t* hnd, rrpge_uint16 const* id,
                      rrpge_uint16 const* buf, rrpge_iuint len);



/**
**  \brief     Request halt cause.
**
**  Returns the cause of the last termination of rrpge_run(). Multiple causes
**  might be present simultaneously. The causes are cleared by succesfully
**  continuing the emulation (calling rrpge_run() without it returning zero),
**  resetting it, or reattaching state. Note that not all causes allow
**  continuing emulation without specific handling.
**
**  \param[in]   hnd   Emulation instance.
**  \return            Halt causes (see defines at \ref halt_causes).
*/
rrpge_iuint rrpge_gethaltcause(rrpge_object_t* hnd);



/**
**  \brief     Get audio events and streams.
**
**  Queries number of 512 sample audio streaks pending, fills in audio data,
**  and clears the RRPGE_HLT_AUDIO halt cause. Normally this should be called
**  in response to such a halt cause to process audio (and by it to ensure
**  proper timing of the emulation tied to the audio hardware). The return
**  this case is normally 1, but with certain emulation implementations and
**  badly programmed applications (which stall the hardware by some means) it
**  might be more. This indicates skipping.
**
**  The provided left and right buffers are filled with the data fetched in
**  the last audio tick, 512 samples each. The format is 8 bit unsigned.
**
**  When called without experiencing an audio halt cause the return value is
**  zero, and it is implementation defined whether the buffers receive any
**  data.
**
**  \param[in]   hnd   Emulation instance.
**  \param[out]  lbuf  8bit audio buffer to receive left sample data.
**  \param[out]  rbuf  8bit audio buffer to receive right sample data.
**  \return            Number of audio events pending.
*/
rrpge_iuint rrpge_getaudio(rrpge_object_t* hnd, rrpge_uint8* lbuf, rrpge_uint8* rbuf);



/**
**  \brief     Toggles graphics rendering.
**
**  Initially (after rrpge_init()) graphics rendering is ON, and the line
**  callback is called normally at every line. It can be turned OFF to disable
**  this making the library skipping visual frames as needed. The call only
**  becomes effective in the next frame (that is after the next experience of
**  the RRPGE_HLT_FRAME halt cause). This may be used to implement
**  frameskipping to drive the emulation at an accepable performance on slow
**  systems. Disabling has no effect on the emulation state as seen through
**  rrpge_peekstate().
**
**  \param[in]   hnd   Emulation instance.
**  \param[in]   tg    0: Rendering OFF, nonzero: Rendering ON.
*/
void rrpge_enarender(rrpge_object_t* hnd, rrpge_ibool tg);



#endif
