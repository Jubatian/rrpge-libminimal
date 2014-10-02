/**
**  \file
**  \brief     Collection of implementations for rrpge.h
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.10.02
*/


#include "rgm_main.h"
#include "rgm_ires.h"
#include "rgm_chk.h"
#include "rgm_cb.h"
#include "rgm_ser.h"



/* Initialize emulator - implementation of RRPGE library function */
rrpge_uint32 rrpge_init(rrpge_cbpack_t const* cb, rrpge_appini_t const* inid,
                        rrpge_object_t* hnd)
{
 auint f;
 auint i;

 /* First check the application header, and return if it is problematic. */

 f = rrpge_checkapphead(inid->head);
 if (f != RRPGE_ERR_OK){ return f; }

 if (inid->ccnt > 0x10000U){ return RRPGE_ERR_INI; }
 if (inid->dcnt >  0xFFC0U){ return RRPGE_ERR_INI; }

 /* OK, if got here, the application header is fine. The emulation instance
 ** can be constructed. This only involves loading the application and calling
 ** reset. */

 /* Add callbacks */

 rrpge_m_cb_process(hnd, cb);

 /* Do an init to get initial data RAM. This is not nice, and later should be
 ** re-organized, so init won't be necessary to be called twice (secondly in
 ** reset) by properly implementing handling the initial state of data memory
 ** as combined from the app. binary and the initial RRPGE data. */

 rrpge_m_ires_init(hnd);

 /* Code ROM */

 for (i = 0U; i < inid->ccnt; i++){
  hnd->crom[i] = inid->crom[i];
 }
 for (      ; i < 0x10000U;   i++){
  hnd->crom[i] = 0U;
 }

 /* CPU Data memory (initializer) */

 for (i = 0U; i < inid->dcnt; i++){
  hnd->dini[i + 0x40U] = inid->dram[i];
 }

 /* Also clear all breakpoints */

 for (i = 0U; i < 2048U; i++){
  hnd->brkp[i] = 0U;
 }

 /* Now do a reset to finish the initialization so emulation may start. */

 rrpge_reset(hnd);

 /* Done, emulation can start */

 return RRPGE_ERR_OK;
}



/* Reset emulator instance - implementation of RRPGE library function */
void rrpge_reset(rrpge_object_t* hnd)
{
 auint i;

 rrpge_m_ires_init(hnd);

 for (i = 0U; i < 65536U; i++){
  hnd->st.dram[i] = hnd->dini[i];
 }
}



/* Request emu. state for read - implementation of RRPGE library function */
rrpge_state_t const* rrpge_peekstate(rrpge_object_t* hnd)
{
 return hnd->st;
}



/* Request emu. state for modify - implementation of RRPGE library function */
rrpge_state_t* rrpge_peekstate(rrpge_object_t* hnd)
{
 return hnd->st;
}



/* Reattaches emulator state - implementation of RRPGE library function */
rrpge_uint32 rrpge_attachstate(rrpge_object_t* hnd)
{
 auint f;

 /* Check application state */

 f = rrpge_checkappstate(&(hnd->st.stat[0]));
 if (f != RRPGE_ERR_OK){ return f; }

 /* Done */

 return RRPGE_ERR_OK;
}



/* Reattaches emulator state with comp. check - implementation of RRPGE library function */
rrpge_uint32 rrpge_attachstatecomp(rrpge_object_t* hnd)
{
 return rrpge_attachstate(hnd); /* Needs additional though... */
}



/* Submit task result - implementation of RRPGE library function */
void rrpge_taskend(rrpge_object_t* hnd, rrpge_uint32 tsh, rrpge_uint32 res)
{
 res |= 0x8000U;
 tsh &= 0xFU;

 switch (hnd->st.stat[RRPGE_STA_KTASK + (tsh << 4)]){

  case 0x0100U: /* Start loading bin. page */
   res = 0x8000U;
   break;

  case 0x0110U: /* Start loading from file */
   res |= 0x8000U;
   break;

  case 0x0111U: /* Start saving into file */
   res |= 0x8000U;
   break;

  case 0x0112U: /* Find next file */
   res = 0x8000U;
   break;

  case 0x0113U: /* Move a file */
   res |= 0x8000U;
   break;

  case 0x0601U: /* Read user UTF-8 */
   res = 0x8000U;
   break;

  case 0x0700U: /* Send data to user */
   res = 0x8000U;
   break;

  case 0x0710U: /* List accessible users */
   res |= 0x8000U;
   break;

  default:      /* Should not happen */
   res = 0x8000U;
   break;
 }

 hnd->st.stat[RRPGE_STA_KTASK + 0xF + (tsh << 4)] = (uint16)(res);

 /* (Note: no need to clear the task started flags, it will be done when a
 ** new task is started) */
}



/* Submit received packet - implementation of RRPGE library function */
void rrpge_pushpacket(rrpge_object_t* hnd, rrpge_uint16 const* id,
                      rrpge_uint16 const* buf, rrpge_uint32 len)
{
 auint s;
 auint i;

 if (len > 4095U) return; /* This packet won't fit */
 if (len ==   0U) return; /* Zero length is not valid */

 /* Sum the currently waiting packets' lenghts for determining what to discard
 ** to make it fit. */
 s = 0;
 for (i = (hnd->reir); i != (hnd->reiw); i = (i + 1U) & 0x3FU){
  s += hnd->recl[i];
 }

 /* Discard packets until the new one fits. */
 while ((s + len) > 4095U){
  hnd->rebr = ((hnd->rebr) + (hnd->recl[hnd->reir])) & 0xFFFU;
  s        -= hnd->recl[hnd->reir];
  hnd->reir = ((hnd->reir) + 1U) & 0x3FU;
 }

 /* Discard one packet if the ID / length buffer is full (may happen with
 ** small packets) */
 if ( (((hnd->reiw) + 1U) & 0x3FU) == (hnd->reir) ){
  hnd->rebr = ((hnd->rebr) + (hnd->recl[hnd->reir])) & 0xFFFU;
  hnd->reir = ((hnd->reir) + 1U) & 0x3FU;
 }

 /* Now the new packet fits. Copy it in, incrementing the pointers meanwhile,
 ** so it is done. */
 for (i = 0; i < len; i++){
  hnd->recb[hnd->rebw] = buf[i];
  hnd->rebw = ((hnd->rebw) + 1U) & 0xFFFU;
 }
 for (i = 0; i < 8U; i++){
  hnd->reci[((hnd->reiw) << 3) + i] = id[i];
 }
 hnd->recl[hnd->reiw] = len;
 hnd->reiw = ((hnd->reiw) + 1U) & 0x3FU;
}



/* Request halt cause - implementation of RRPGE library function */
rrpge_uint32 rrpge_gethaltcause(rrpge_object_t* hnd)
{
 return hnd->hlt;
}



/* Toggle graphics rendering - implementation of RRPGE library function */
void rrpge_enarender(rrpge_object_t* hnd, rrpge_uint32 tg)
{
 if (tg != 0) tg = 1U;
 hnd->rena = ((hnd->rena) & (~1U)) | tg;
}
