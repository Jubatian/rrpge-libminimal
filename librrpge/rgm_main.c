/**
**  \file
**  \brief     Collection of implementations for rrpge.h
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.27
*/


#include "rgm_main.h"
#include "rgm_ires.h"
#include "rgm_chk.h"
#include "rgm_cb.h"
#include "rgm_ser.h"
#include "rgm_abuf.h"



/* Initialize emulator - implementation of RRPGE library function */
rrpge_uint32 rrpge_init(rrpge_cbpack_t const* cb,   rrpge_uint16 const* apph,
                        rrpge_uint16   const* crom, rrpge_uint32 crn,
                        rrpge_udata_t  const* udat, rrpge_object_t* hnd)
{
 auint f;
 auint i;

 /* First check the application header, and return if it is problematic. */
 f = rrpge_checkapphead(apph);
 if (f != RRPGE_ERR_OK) return f;
 if ( ((((apph[0xBC2U] >> 8) - 1U) & 0xFU) + 1U) != crn ) return RRPGE_ERR_PG;

 /* OK, if got here, the application header is fine. The emulation instance
 ** can be constructed. This only involves loading the application and calling
 ** reset. */

 rrpge_m_cb_process(hnd, cb);

 for (i = 0; i < 0xC00U; i++){
  hnd->stat.ropd[i] = apph[i];
 }
 hnd->stat.ropd[1] = (((auint)('S')) << 8) + (auint)('\n'); /* "RPA\n" -> "RPS\n" */

 for (i = 0; i < (crn << 12); i++){
  hnd->crom[i] = crom[i];
 }
 for (i = (crn << 12); i < (16U << 12); i++){
  hnd->crom[i] = 0;
 }

 rrpge_udata2ropd(udat, &(hnd->stat.ropd[0]));

 /* Now do a reset to finish the initialization so emulation may start. */

 rrpge_reset(hnd);

 /* Also clear all breakpoints */

 for (i = 0; i < 2048U; i++){
  hnd->brkp[i] = 0;
 }

 /* Done, emulation can start */

 return RRPGE_ERR_OK;
}



/* Reset emulator instance - implementation of RRPGE library function */
void rrpge_reset(rrpge_object_t* hnd)
{
 rrpge_m_ires_init(hnd);
}



/* Loads serialized state - implementation of RRPGE library function */
rrpge_uint32 rrpge_importstate(rrpge_object_t* hnd, rrpge_uint8 const* st)
{
 uint16 tropd[4096U];
 auint  f;
 auint  i;
 rrpge_header_t apph;
 rrpge_header_t stah;

 rrpge_convpg_b2w(&(st[0U << 13]), &(tropd[0]));
 f = rrpge_checkropd(&(tropd[0]));
 if (f != RRPGE_ERR_OK) return f;

 f = rrpge_checkcommhead(&(tropd[0]), &stah);
 if (f != RRPGE_ERR_OK) return RRPGE_ERR_UNK; /* Should not happen */

 f = rrpge_checkcommhead(&(hnd->stat.ropd[0]), &apph);
 if (f != RRPGE_ERR_OK) return RRPGE_ERR_UNK; /* Should not happen */

 if (!rrpge_isstatecomp(&stah, &apph)) return RRPGE_ERR_VER;

 /* OK, state is fine, and is compatible, so can be loaded */

 for (i = 0; i < 4096U; i++){
  hnd->stat.ropd[i] = tropd[i];
 }

 for (i = 0; i < 8U; i++){
  rrpge_convpg_b2w(&(st[(i + 1U) << 13]), &(hnd->stat.sram[i << 12]));
 }

 for (i = 0; i < 224U; i++){
  rrpge_convpg_b2w(&(st[(i + 9U) << 13]), &(hnd->stat.dram[i << 12]));
 }

 for (i = 0; i < (128U * 2048U); i++){
  hnd->stat.vram[i] = ((uint32)(st[(233U << 13) + (i << 2) + 0U]) << 24) +
                      ((uint32)(st[(233U << 13) + (i << 2) + 1U]) << 16) +
                      ((uint32)(st[(233U << 13) + (i << 2) + 2U]) <<  8) +
                      ((uint32)(st[(233U << 13) + (i << 2) + 3U])      );
 }

 /* Also reset library internal components */

 rrpge_m_ires_initl(hnd);

 /* Done */

 return RRPGE_ERR_OK;
}



/* Return state - implementation of RRPGE library function */
rrpge_state_t const* rrpge_exportstate(rrpge_object_t* hnd)
{
 return &(hnd->stat);
}



/* Submit task result - implementation of RRPGE library function */
void rrpge_taskend(rrpge_object_t* hnd, rrpge_uint32 tsh, rrpge_uint32 res)
{
 res |= 0x8000U;
 tsh &= 0xFU;

 switch (hnd->stat.ropd[0xD80U + (tsh << 4)]){

  case 0x0100U: /* Start loading bin. page */
   res = 0x8000U;
   break;

  case 0x0110U: /* Start loading nv. save */
   res &= 0xFFFFU;
   break;

  case 0x0111U: /* Start saving nv. save */
   res &= 0x8001U;
   break;

  case 0x0112U: /* List available nv. saves */
   if ((res & 0x7FFFU) > 1024U) res = 0x8000U + 1024U;
   break;

  case 0x0601U: /* Read user UTF-8 */
   res = 0x8000U;
   break;

  case 0x0700U: /* Send data to user */
   res = 0x8000U;
   break;

  case 0x0710U: /* List accessible users */
   if ((res & 0x7FFFU) > 256U) res = 0x8000U + 256U;
   break;

  default:      /* Should not happen */
   res = 0x8000U;
   break;
 }

 hnd->stat.ropd[0xD8F + (tsh << 4)] = (uint16)(res);

 /* (Note: no need to clear the task started flags, it will be done when a
 ** new task is started) */
}



/* Get network availability - implementation of RRPGE library function */
rrpge_uint32 rrpge_getnetavail(rrpge_object_t* hnd)
{
 return (hnd->stat.ropd[0xD3F] & 1U);
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



/* Get audio events and streams - implementation of RRPGE library function */
rrpge_uint32 rrpge_getaudio(rrpge_object_t* hnd, rrpge_uint8* lbuf, rrpge_uint8* rbuf)
{
 auint r;
 auint i;
 auint pl;
 auint pr;
 auint l;

 if ((hnd->aco) == 0) return 0; /* No audio event present */

 r = hnd->aco;
 hnd->aco =  0;
 hnd->hlt &= ~(auint)(RRPGE_HLT_AUDIO);

 /* Fill in the buffers with the half currently marked for outputting */

 rrpge_m_abuf_getptr(hnd->stat.ropd[0xBC2U],
                     hnd->stat.ropd[0xD56U],
                     &pl, &pr);
 pl >>= 1;                      /* Make word offsets */
 pr >>= 1;

 if ((hnd->stat.ropd[0xBC2U] & 0x2000U) != 0U){ l = 512U; }
 else                                         { l = 256U; }

 for (i = 0; i < l; i++){
  lbuf[(i << 1)     ] = (uint8)(hnd->stat.dram[pl + i] >> 8);
  lbuf[(i << 1) + 1U] = (uint8)(hnd->stat.dram[pl + i]     );
  rbuf[(i << 1)     ] = (uint8)(hnd->stat.dram[pr + i] >> 8);
  rbuf[(i << 1) + 1U] = (uint8)(hnd->stat.dram[pr + i]     );
 }

 /* OK, all done */

 return r;
}



/* Toggle graphics rendering - implementation of RRPGE library function */
void rrpge_enarender(rrpge_object_t* hnd, rrpge_uint32 tg)
{
 if (tg != 0) tg = 1U;
 hnd->rena = ((hnd->rena) & (~1U)) | tg;
}
