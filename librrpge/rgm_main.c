/**
**  \file
**  \brief     Collection of implementations for rrpge.h
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#include "rgm_main.h"
#include "rgm_ires.h"
#include "rgm_chk.h"
#include "rgm_cb.h"
#include "rgm_ser.h"



/* Initialize emulator - implementation of RRPGE library function */
rrpge_iuint rrpge_init(rrpge_cbpack_t const* cb, rrpge_object_t* hnd)
{
 /* Add callbacks */

 rrpge_m_cb_process(hnd, cb);

 /* Init halt cause and initialization state machine */

 hnd->insm = 0x1U;
 hnd->hlt  = RRPGE_HLT_WAIT;

 /* Pass over to the run function */

 return rrpge_init_run(hnd);
}



/* Run initialization - implementation of RRPGE library function */
rrpge_iuint rrpge_init_run(rrpge_object_t* hnd)
{
 auint   f;
 auint   i;
 uint16* p;
 rrpge_cbp_loadbin_t cbp_loadbin;

 /* Initialization state machine. Note that hnd->insm might be incremented by
 ** the callbacks if the host does not load the part asynchronously. */


 if (hnd->insm == 0x1U){  /* Initialization start */

  /* Load the application header */
  cbp_loadbin.buf = &(hnd->apph[0]);
  cbp_loadbin.scw = 64U;
  cbp_loadbin.sow = 0U;
  hnd->cb_tsk[RRPGE_CB_LOADBIN](hnd, 0U, &cbp_loadbin);
  hnd->insm ++;

 }


 if (hnd->insm == 0x2U){  /* Wait for loading */
  return RRPGE_ERR_WAIT;
 }


 if (hnd->insm == 0x3U){  /* Start loading application descriptor */

  f = rgm_chk_checkapphead(&(hnd->apph[0]), &i); /* Needed to retrieve descriptor offset */
  if (f != RRPGE_ERR_OK){ return f; }

  /* Load the application descriptor (12 words) */
  cbp_loadbin.buf = &(hnd->appd[0]);
  cbp_loadbin.scw = 12U;
  cbp_loadbin.sow = i;
  hnd->cb_tsk[RRPGE_CB_LOADBIN](hnd, 0U, &cbp_loadbin);
  hnd->insm ++;

 }


 if (hnd->insm == 0x4U){  /* Wait for loading */
  return RRPGE_ERR_WAIT;
 }


 if (hnd->insm == 0x5U){  /* Merge and check application header and descriptor */

  rrpge_m_ires_initstat(hnd);
  f = rrpge_checkappstate(&(hnd->st.stat[0]));
  if (f != RRPGE_ERR_OK){ return f; }

  /* Load code area */
  rrpge_m_ires_initcode(hnd);
  p = &(hnd->appd[0]);
  cbp_loadbin.buf = &(hnd->crom[0]);
  cbp_loadbin.scw = ((p[0x6U] - 1U) & 0xFFFFU) + 1U;
  cbp_loadbin.sow = ((p[0x2U] & 0xFFFFU) << 16) + (p[0x3U] & 0xFFFFU);
  if ( (cbp_loadbin.scw + cbp_loadbin.sow) >
       (((p[0x0U] & 0xFFFFU) << 16) + (p[0x1U] & 0xFFFFU)) ){
   return (RRPGE_ERR_DSC + 0x2U); /* Code is out of app. binary */
  }
  hnd->cb_tsk[RRPGE_CB_LOADBIN](hnd, 0U, &cbp_loadbin);
  hnd->insm ++;

 }


 if (hnd->insm == 0x6U){  /* Wait for loading */
  return RRPGE_ERR_WAIT;
 }


 if (hnd->insm == 0x7U){  /* Check and start loading data */

  p = &(hnd->appd[0]);
  if (p[0x7U] > 0xFFC0U){
   return (RRPGE_ERR_DSC + 0x7U); /* Data wraparound */
  }
  rrpge_m_ires_initdata(hnd);     /* Reset data memory initializer */

  /* Load data area */
  cbp_loadbin.buf = &(hnd->dini[0x40U]);
  cbp_loadbin.scw = ((p[0x7U] & 0xFFFFU));
  cbp_loadbin.sow = ((p[0x4U] & 0xFFFFU) << 16) + (p[0x5U] & 0xFFFFU);
  if ( (cbp_loadbin.scw + cbp_loadbin.sow) >
       (((p[0x0U] & 0xFFFFU) << 16) + (p[0x1U] & 0xFFFFU)) ){
   return (RRPGE_ERR_DSC + 0x4U); /* Data is out of app. binary */
  }
  hnd->cb_tsk[RRPGE_CB_LOADBIN](hnd, 0U, &cbp_loadbin);
  hnd->insm ++;

 }


 if (hnd->insm == 0x8U){  /* Wait for loading */
  return RRPGE_ERR_WAIT;
 }


 if (hnd->insm == 0x9U){  /* Finalize */

  /* Clear all breakpoints */
  for (i = 0U; i < 2048U; i++){ hnd->brkp[i] = 0U; }

  /* Do a reset to finish the initialization so emulation may start. */
  rrpge_reset(hnd);

  /* State machine ends. Halt causes are clear due to rrpge_reset() at this
  ** point. */
  hnd->insm = 0U;

  /* Done, emulation can start */
  return RRPGE_ERR_OK;

 }


 /* No hit in state machine, so not a valid initialization. */

 return RRPGE_ERR_INI;
}



/* Reset emulator instance - implementation of RRPGE library function */
void rrpge_reset(rrpge_object_t* hnd)
{
 rrpge_m_ires_init(hnd);
}



/* Request emu. state for read - implementation of RRPGE library function */
rrpge_state_t const* rrpge_peekstate(rrpge_object_t* hnd)
{
 return &(hnd->st);
}



/* Request emu. state for modify - implementation of RRPGE library function */
rrpge_state_t* rrpge_detachstate(rrpge_object_t* hnd)
{
 return &(hnd->st);
}



/* Reattaches emulator state - implementation of RRPGE library function */
rrpge_iuint rrpge_attachstate(rrpge_object_t* hnd)
{
 auint f;

 /* Check application state */

 f = rrpge_checkappstate(&(hnd->st.stat[0]));
 if (f != RRPGE_ERR_OK){ return f; }

 /* Done */

 return RRPGE_ERR_OK;
}



/* Reattaches emulator state with comp. check - implementation of RRPGE library function */
rrpge_iuint rrpge_attachstatecomp(rrpge_object_t* hnd)
{
 return rrpge_attachstate(hnd); /* Needs additional though... */
}



/* Submit task result - implementation of RRPGE library function */
void rrpge_taskend(rrpge_object_t* hnd, rrpge_iuint tsh, rrpge_iuint res)
{
 if ((hnd->insm) != 0U){ /* Initializing - assume ending an app binary load */
  hnd->insm ++;          /* Advance initialization state (this notifies that the part is loaded) */
  return;
 }

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

 hnd->st.stat[RRPGE_STA_KTASK + 0xF + (tsh << 4)] = res & 0xFFFFU;

 /* (Note: no need to clear the task started flags, it will be done when a
 ** new task is started) */
}



/* Submit received packet - implementation of RRPGE library function */
void rrpge_pushpacket(rrpge_object_t* hnd, rrpge_uint16 const* id,
                      rrpge_uint16 const* buf, rrpge_iuint len)
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
rrpge_iuint rrpge_gethaltcause(rrpge_object_t* hnd)
{
 return hnd->hlt;
}



/* Toggle graphics rendering - implementation of RRPGE library function */
void rrpge_enarender(rrpge_object_t* hnd, rrpge_ibool tg)
{
 if (tg){ hnd->rena = (hnd->rena) |   1U;  }
 else   { hnd->rena = (hnd->rena) & (~1U); }
}
