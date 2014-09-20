/**
**  \file
**  \brief     LibRRPGE standard header package - main header
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.09.20
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
**  \brief     Initializes emulator over a given application.
**
**  Attempts to initialize an instance of an RRPGE system loading the given
**  application. Checks all it's input for validity, and will only succeed if
**  all it's inputs are valid and consistent. Returns with a failure code if
**  this is not successful. The emulation state is stored into a passed
**  appropriately sized buffer (request the necessary size using
**  rrpge_getdescription()) which buffer will be used as object handle
**  further. It may be discarded any time later to terminate emulation: the
**  library does not allocate any extra resource relating to instances.
**
**  Note that the application binary should be kept open to support the Start
**  loading binary data kernel call.
**
**  \param[in]   cb    Callback set filled with the callbacks.
**  \param[in]   inid  Application initialization data.
**  \param[out]  hnd   Emulation instance to initialize.
**  \return            0 on success, failure code otherwise.
*/
rrpge_uint32 rrpge_init(rrpge_cbpack_t const* cb, rrpge_appini_t const* inid,
                        rrpge_object_t* hnd);



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
**  \brief     Requests emulator state for reading.
**
**  Returns a pointer to the current emulator state for reading, such as for
**  exporting it, or observing it for the purpose of debugging. The state is
**  part of the emulation instance.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            State information.
*/
rrpge_state_t const* rrpge_peekstate(rrpge_object_t* hnd);



/**
**  \brief     Detaches emulator state for modifying.
**
**  Detaches the emulator state for writes, so it may be modified. The state
**  is still a part of the emulation instance, however the emulator is not
**  capable to operate on it until reattached. This may be used for debugging
**  or importing state.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            State information.
*/
rrpge_state_t* rrpge_detachstate(rrpge_object_t* hnd);



/**
**  \brief     Reattaches emulator state.
**
**  Reattaches a previously detached state to it's emulation instance, so the
**  emulation may continue. The reattach may fail if the state contains
**  invalid information. Note that the state reattached is a part of the
**  emulation instance. Clears all halt causes if succesful.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            0 on success, failure code otherwise.
*/
rrpge_uint32 rrpge_attachstate(rrpge_object_t* hnd);



/**
**  \brief     Reattaches emulator state with compatibility check.
**
**  Reattaches a previously detached state to it's emulation instance, so the
**  emulation may continue. The reattach may fail if the state contains
**  invalid information. Note that the state reattached is a part of the
**  emulation instance. Clears all halt causes if succesful. This variant also
**  checks state compatibility with the application as it was before the last
**  rrpge_detachstate() call.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            0 on success, failure code otherwise.
*/
rrpge_uint32 rrpge_attachstatecomp(rrpge_object_t* hnd);



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
**  resetting it, or reattaching state. Note that not all causes allow
**  continuing emulation without specific handling.
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \return            Halt causes (see defines at \ref halt_causes).
*/
rrpge_uint32 rrpge_gethaltcause(rrpge_object_t* hnd);



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
**  systems. Disabling has no effect on the emulation state as seen through
**  rrpge_peekstate().
**
**  \param[in]   hnd   Emulation instance populated by rrpge_init().
**  \param[in]   tg    0: Rendering OFF, nonzero: Rendering ON.
*/
void rrpge_enarender(rrpge_object_t* hnd, rrpge_uint32 tg);



#endif
