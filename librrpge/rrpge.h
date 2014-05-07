/**
**  \file
**  \brief     LibRRPGE standard header package - main header
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#ifndef RRPGE_H
#define RRPGE_H


#include "rrpge_tp.h"
#include "rrpge_cb.h"
#include "rrpge_ch.h"
#include "rrpge_sr.h"
#include "rrpge_pr.h"
#include "rrpge_db.h"



/**
**  \brief     Initializes emulator over a given application.
**
**  Attempts to initialize an instance of an RRPGE system loading the given
**  application. Checks all it's input for validity, and will only succeed if
**  all it's inputs are valid and consistent. License check if appropriate
**  also happens here. Returns with a failure code if this is not successful.
**  The emulation state is stored into a passed appropriately sized buffer
**  (request the necessary size using rrpge_getdescription()) which buffer
**  will be used as object handle further. It may be discarded any time later
**  to terminate emulation: the library does not allocate any extra resource
**  relating to instances.
**
**  \param[in]   cb    Callback set filled with the callbacks.
**  \param[in]   apph  Application binary header as-is (4096 elements).
**  \param[in]   crom  Code pages (up to 16 pages, 4096 elements per page).
**  \param[in]   crn   Number of code pages passed.
**  \param[in]   udat  Initial user specific data for the ROPD.
**  \param[out]  hnd   Emulation instance to initialize.
**  \return            0 on success, failure code otherwise.
*/
rrpge_uint32 rrpge_init(rrpge_cbpack_t const* cb,   rrpge_uint16 const* apph,
                        rrpge_uint16   const* crom, rrpge_uint32 crn,
                        rrpge_udata_t  const* udat, rrpge_object_t* hnd);



/**
**  \brief     Resets emulator instance.
**
**  Returns the given emulator instance to it's initial state, capable to
**  restart the application from the beginning. The result of this is
**  identical to the state obtained after an appropriate call to rrpge_init().
**  The user specific data is left as-is.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
*/
void rrpge_reset(rrpge_object_t* hnd);



/**
**  \brief     Loads serialized state into emulator instance.
**
**  Attempts to load a saved state into the given emulator instance. The saved
**  state has to be compatible with the loaded application for this to be
**  successful. Note that this will always trigger certain operations such as
**  starting kernel tasks if the state contains any running. All halt causes
**  are cleared by this method when it succeeds.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   st    Raw state to load (361 * 8192 bytes).
**  \return            0 on success, failure code otherwise.
*/
rrpge_uint32 rrpge_importstate(rrpge_object_t* hnd, rrpge_uint8 const* st);



/**
**  \brief     Returns (serialized) state from emulator instance.
**
**  Returns or exports if necessary current application state from the
**  emulator instace suitable for serializing and loading later with
**  rrpge_importstate(). The data pointed by the pointer is managed by the
**  emulation library and belongs to the instance (is a part of hnd). It
**  persists until another operation is called on the same instance. This
**  method should be used to watch memories during debugging sessions.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            State information.
*/
rrpge_state_t const* rrpge_exportstate(rrpge_object_t* hnd);



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
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   rmod  Running mode. See \ref rrpge_run_modes.
**  \return            Number of cycles emulated.
*/
rrpge_uint32 rrpge_run(rrpge_object_t* hnd, rrpge_uint32 rmod);



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
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   tsh   Task handle acquired from the callback (0-15).
**  \param[in]   res   Result of task (low 15 bits used, bit 15 always set).
*/
void rrpge_taskend(rrpge_object_t* hnd, rrpge_uint32 tsh, rrpge_uint32 res);



/**
**  \brief     Get network availability status.
**
**  Extracts the network availability bit from the Read Only Process
**  Descriptor. Returns zero if the user should not be available for new
**  connections, nonzero (one) otherwise.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            Nonzero if user is available for new connections.
*/
rrpge_uint32 rrpge_getnetavail(rrpge_object_t* hnd);



/**
**  \brief     Submit received packet to the emulator.
**
**  If a network packet is received targeting the user running the
**  application, it should be submitted to the emulator using this method.
**  Note that the application may also expect packets when it is not available
**  for network connections. (The availability flag is only used to note that
**  the user is available for new connections).
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   id    User ID of the packet's sender (8 words).
**  \param[in]   buf   Packet contents.
**  \param[in]   len   Size of packet in word units.
*/
void rrpge_pushpacket(rrpge_object_t* hnd, rrpge_uint16 const* id,
                      rrpge_uint16 const* buf, rrpge_uint32 len);



/**
**  \brief     Request halt cause.
**
**  Returns the cause of the last termination of rrpge_run(). Multiple causes
**  might be present simultaneously. The causes are cleared by succesfully
**  continuing the emulation (calling rrpge_run() without it returning zero),
**  resetting it, or loading a new state. Note that not all causes allow
**  continuing emulation without specific handling.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            Halt causes (see defines at \ref halt_causes).
*/
rrpge_uint32 rrpge_gethaltcause(rrpge_object_t* hnd);



/**
**  \brief     Get audio events and streams.
**
**  Queries number of audio events pending, fills in audio data, and clears
**  the RRPGE_HLT_AUDIO halt cause. Normally this should be called in response
**  to such a stall to process audio (and by it to ensure proper timing of the
**  emulation tied to the audio hardware). The return this case is normally 1,
**  but with certain emulation implementations and badly programmed
**  applications (which fail to service audio interrupts themselves) it might
**  be more. This indicates skipping.
**
**  The provided left and right buffers are filled with the data fetched in
**  the last audio tick, 512 samples each. The format is 8 bit unsigned.
**
**  When called without experiencing an audio halt cause the return value is
**  zero, and it is implementation defined whether the buffers receive any
**  data.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[out]  lbuf  8bit audio buffer to receive left sample data.
**  \param[out]  rbuf  8bit audio buffer to receive right sample data.
**  \return            Number of audio events pending.
*/
rrpge_uint32 rrpge_getaudio(rrpge_object_t* hnd, rrpge_uint8* lbuf, rrpge_uint8* rbuf);



/**
**  \brief     Toggles graphics rendering.
**
**  Initially (after rrpge_init()) graphics rendering is ON, and the line
**  callback is called normally at every line. It can be turned OFF to disable
**  this making the library skipping visual frames as needed. The call only
**  becomes effective in the next frame (that is after the next experience of
**  the RRPGE_HLT_FRAME halt cause). This may be used to implement
**  frameskipping to drive the emulation at an accepable performance on slow
**  systems. Disabling has no effect on the exportable emulation state (return
**  of rrpge_exportstate()).
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   tg    0: Rendering OFF, nonzero: Rendering ON.
*/
void rrpge_enarender(rrpge_object_t* hnd, rrpge_uint32 tg);



#endif
