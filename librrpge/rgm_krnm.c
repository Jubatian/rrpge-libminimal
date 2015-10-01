/**
**  \file
**  \brief     Kernel functionality
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.04
*/


#include "rgm_krnm.h"
#include "rgm_mix.h"
#include "rgm_prng.h"
#include "rgm_task.h"
#include "rgm_halt.h"
#include "rgm_dev.h"



/* Finds empty kernel task slot and populates & allocates it. Fills in resl
** according to the result. Returns nonzero if the  parameters don't make a
** valid task. (does not check parameter count!) */
static auint rrpge_m_ktsalloc(uint16 const* par, auint n, auint* resl)
{
 auint i;
 auint j;
 for (i = RRPGE_STA_KTASK; i < (RRPGE_STA_KTASK + 0x100U); i += 0x10U){
  if ((rrpge_m_edat->st.stat[i + 0xFU] & 0xFFFFU) == 0U){
   for (j = 0U; j < n; j++){
    rrpge_m_edat->st.stat[i + j] = par[j] & 0xFFFFU;
   }
   rrpge_m_edat->st.stat[i + 0xFU] = 0x0001U;
   j = ((i - 0xD80U) >> 4);
   rrpge_m_edat->tsfl &= ~((auint)(1U) << j); /* Task not started yet! */
   *resl = j;                                 /* Fill in result */
   return rrpge_m_taskcheck(&(rrpge_m_edat->st.stat[0]), j);
  }
 }
 *resl = 0x8000U;
 return 0;
}



/* Processes a supervisor call. The first element of the parameter list
** selects the call. The number of parameters are given in the second
** parameter: the array must be at least this long. The function sets the
** appropriate halt cause if necessary (as defined in the host callbacks of
** the RRPGE library interface). Returns the number of cycles which were
** necessary for performing the call. The return value is produced in
** resh:resl (16 bits each), the original value retained if the specification
** does not specify a return. */
auint rrpge_m_kcall(uint16 const* par, auint n, auint* resh, auint* resl)
{
 auint   i;
 auint   o;
 auint   r;       /* Return: Cycles consumed */
 uint16* stat = &(rrpge_m_edat->st.stat[0]);
 rrpge_cbp_setpal_t    cbp_setpal;
 rrpge_cbp_setst3d_t   cbp_setst3d;
 rrpge_cbp_getlocal_t  cbp_getlocal;
 rrpge_cbp_getlang_t   cbp_getlang;


 if (n == 0U){    /* No parameters for the kernel call */
  goto fault_inv;
 }


 switch (par[0]){ /* At least one parameter is supplied, so can go */


  case 0x00U:     /* Kernel task: Start loading binary data page */

   if (n != 5U){  /* Needs 1+4 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n, resl) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x01U:     /* Kernel task: Start loading page from file */

   if (n != 7U){  /* Needs 1+6 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n, resl) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x02U:     /* Kernel task: Start saving page into file */

   if (n != 7U){  /* Needs 1+6 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n, resl) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x03U:     /* Kernel task: Find next file */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n, resl) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x04U:     /* Kernel task: Move a file */

   if (n != 5U){  /* Needs 1+4 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n, resl) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x08U:     /* Set palette entry */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   cbp_setpal.id  = par[1] & 0xFFU;
   cbp_setpal.col = par[2] & 0xFFFU;
   stat[RRPGE_STA_PAL + cbp_setpal.id] = cbp_setpal.col;
   rrpge_m_edat->cb_sub[RRPGE_CB_SETPAL](rrpge_m_edat, &cbp_setpal);

   r = 100U;
   goto ret_callback;


  case 0x0AU:     /* Set stereoscopic 3D */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_setst3d.mod = par[1] & 0x7U;
   stat[RRPGE_STA_VARS + 0x17U] = cbp_setst3d.mod;
   rrpge_m_edat->cb_sub[RRPGE_CB_SETST3D](rrpge_m_edat, &cbp_setst3d);

   r = 2400U;
   goto ret_callback;


  case 0x10U:     /* Request device */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if (rrpge_m_dev_req(rrpge_m_edat, par[1] & 0xFU, par[2] & 0xFU) == 0U){
    *resl = 0U;
   }else{
    *resl = 1U;
   }

   r = 800U;
   break;


  case 0x11U:     /* Drop device */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   rrpge_m_dev_drop(rrpge_m_edat, par[1] & 0xFU);

   r = 800U;
   break;


  case 0x12U:     /* Pop input event queue */

   if (n != 1U){  /* Needs 1+0 parameters */
    goto fault_inv;
   }

   i = rrpge_m_dev_pop(rrpge_m_edat);
   *resh = (i >> 16) & 0xFFFFU;
   *resl = (i      ) & 0xFFFFU;

   r = 800U;
   break;


  case 0x13U:     /* Peek input event queue */

   if (n != 1U){  /* Needs 1+0 parameters */
    goto fault_inv;
   }

   i = rrpge_m_dev_peek(rrpge_m_edat);
   *resh = (i >> 16) & 0xFFFFU;
   *resl = (i      ) & 0xFFFFU;

   r = 800U;
   break;


  case 0x14U:     /* Flush input event queue */

   if (n != 1U){  /* Needs 1+0 parameters */
    goto fault_inv;
   }

   rrpge_m_dev_flush(rrpge_m_edat);

   r = 800U;
   break;


  case 0x1FU:     /* Delay some cycles */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   r = (par[1] & 0xFFFFU) - (rrpge_m_prng() & 0x3FFU); /* 1024 cycle jitter */
   if (r > (par[1] & 0xFFFFU)){ r = 0U;   }            /* Underflow */
   if (r < 200U){               r = 200U; }            /* Minimal consumed cycles */
   break;


  case 0x20U:     /* Get local users */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (rrpge_m_task_chkdata(par[1] & 0xFFFFU, 32U)){ /* Not in Data area */
    goto fault_inv;
   }

   cbp_getlocal.buf = &rrpge_m_edat->st.dram[par[1] & 0xFFFFU];
   rrpge_m_edat->cb_sub[RRPGE_CB_GETLOCAL](rrpge_m_edat, &cbp_getlocal);

   r = 2400U;
   goto ret_callback;


  case 0x21U:     /* Kernel task: Get UTF-8 representation of User ID */

   if (n != 6U){ /* Needs 1+5 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n, resl) != 0U){ goto fault_inv; }
   r = 1200U;
   goto ret_callback;


  case 0x22U:     /* Get user preferred language */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_getlang.lno = par[1] & 0xFFFFU;
   *resl = rrpge_m_edat->cb_fun[RRPGE_CB_GETLANG](rrpge_m_edat, &cbp_getlang);
   *resh = *resl >> 16;

   r = 2400U;
   goto ret_callback;


  case 0x23U:     /* Get user preferred colors */

   if (n != 1U){  /* Needs 1+0 parameters */
    goto fault_inv;
   }

   *resl = rrpge_m_edat->cb_fun[RRPGE_CB_GETCOLORS](rrpge_m_edat, RRPGE_M_NULL);
   *resh = *resl >> 16;

   r = 2400U;
   goto ret_callback;


  case 0x24U:     /* Get user stereoscopic 3D preference */

   if (n != 1U){  /* Needs 1+0 parameters */
    goto fault_inv;
   }

   *resl = rrpge_m_edat->cb_fun[RRPGE_CB_GETST3D](rrpge_m_edat, RRPGE_M_NULL) & 1U;

   r = 2400U;
   goto ret_callback;


  case 0x28U:     /* Kernel task: Send data to user */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n, resl) != 0U){ goto fault_inv; }
   r = 2400U;
   goto ret_callback;


  case 0x29U:     /* Poll for packets */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if (rrpge_m_task_chkdata(par[1] & 0xFFFFU, par[2] & 0xFFFFU)){ /* Not in Data area */
    goto fault_inv;
   }

   if (rrpge_m_task_chkdata(par[3] & 0xFFFFU, 8U)){ /* Not in Data area */
    goto fault_inv;
   }

   /* Note: No validity checking in rrpge_m_edat. These elements are
   ** maintained by the library, they shouldn't be wrong. */

   rrpge_m_edat->reir &= 0x3FU; /* Some sanity masks to always prevent */
   rrpge_m_edat->reiw &= 0x3FU; /* addressing out of array */
   rrpge_m_edat->rebr &= 0xFFFU;
   rrpge_m_edat->rebw &= 0xFFFU;

   if ((rrpge_m_edat->reir) == (rrpge_m_edat->reiw)){ /* No packet */

    *resl = 0U;

   }else{                                             /* There are packets */

    rrpge_m_edat->recl[rrpge_m_edat->reir] &= 0xFFFU; /* Sanity mask */

    r = rrpge_m_edat->reir;
    for (i = 0U; i < 8U; i++){  /* Copy user ID data into target */
     rrpge_m_edat->st.dram[(par[3] & 0xFFFFU) + i] = rrpge_m_edat->reci[(r << 3) + i];
    }

    r = rrpge_m_edat->recl[r];
    if (r > (par[2] & 0xFFFFU)){ r = par[2] & 0xFFFFU; }
    for (i = 0U; i < r; i++){   /* Copy packet data */
     rrpge_m_edat->st.dram[(par[1] & 0xFFFFU) + i] = rrpge_m_edat->recb[rrpge_m_edat->rebr];
     rrpge_m_edat->rebr = (rrpge_m_edat->rebr + 1U) & 0xFFFU;
    }

    *resl = r;     /* A: the length of the packet */

    rrpge_m_edat->reir = (rrpge_m_edat->reir + 1U) & 0x3FU;

   }

   r = 2400U;
   break;


  case 0x2AU:     /* Kernel task: List accessible users */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n, resl) != 0U){ goto fault_inv; }
   r = 2400U;
   goto ret_callback;


  case 0x2BU:     /* Set network availability */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if ((par[1] & 0xFFFFU) != 0U){
    stat[RRPGE_STA_VARS + 0x1FU] = 1U;
   }else{
    stat[RRPGE_STA_VARS + 0x1FU] = 0U;
   }

   r = 400U;
   break;


  case 0x2CU:     /* Query network availability */

   if (n != 1U){  /* Needs 1+0 parameters */
    goto fault_inv;
   }

   *resl = stat[RRPGE_STA_VARS + 0x1FU] = 1U;

   r = 400U;
   break;


  case 0x2EU:     /* Query task */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if ((par[1] & 0xFFFFU) >= 0x10U){
    *resl = 0xFFFFU;
   }else{
    *resl = stat[RRPGE_STA_KTASK + 0xFU + (par[1] << 4)];
   }

   r = 400U;
   break;


  case 0x2FU:     /* Discard task */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if ((par[1] & 0xFFFFU) < 0x10U){
    o = RRPGE_STA_KTASK + ((par[1] & 0xFFFFU) << 4);
    /* Only discards completed tasks */
    if ((stat[o] & 0x8000U) != 0U){
     stat[o] = 0U;
    }
   }

   r = 100U;
   break;


  default:        /* Unimplemented */

   goto fault_inv;
   break;

 }

 return r;

ret_callback:     /* Return after servicing a callback */

 rrpge_m_halt_set(rrpge_m_edat, RRPGE_HLT_CALLBACK);
 return r;

fault_inv:        /* Return with invalid kernel call */

 rrpge_m_halt_set(rrpge_m_edat, RRPGE_HLT_INVKCALL);
 return 0;

}
