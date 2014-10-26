/**
**  \file
**  \brief     Kernel functionality
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.26
*/


#include "rgm_krnm.h"
#include "rgm_mix.h"
#include "rgm_prng.h"
#include "rgm_task.h"



/* Finds empty kernel task slot and populates & allocates it. Fills in the CPU
** X3 register (in info) according to the result. Returns nonzero if the
** parameters don't make a valid task. (does not check parameter count!) */
static auint rrpge_m_ktsalloc(uint16 const* par, auint n)
{
 auint i;
 auint j;
 for (i = RRPGE_STA_KTASK; i < (RRPGE_STA_KTASK + 0x100U); i += 0x10U){
  if (rrpge_m_edat->st.stat[i + 0xFU] == 0U){
   for (j = 0U; j < n; j++){
    rrpge_m_edat->st.stat[i + j] = par[j];
   }
   rrpge_m_edat->st.stat[i + 0xFU] = 0x0001U;
   j = ((i - 0xD80U) >> 4);
   rrpge_m_edat->tsfl &= ~((auint)(1U) << j); /* Task not started yet! */
   rrpge_m_info.xr[7] = j;                    /* Fill in register 'x3' */
   return rrpge_m_taskcheck(&(rrpge_m_edat->st.stat[0]), j);
  }
 }
 rrpge_m_info.xr[7] = 0x8000U;
 return 0;
}



/* Processes a supervisor call. The first element of the parameter list
** selects the call. The number of parameters are given in the second
** parameter: the array must be at least this long. The function sets the
** appropriate halt cause if necessary (as defined in the host callbacks of
** the RRPGE library interface) Returns the number of cycles which were
** necessary for performing the call. */
auint rrpge_m_kcall(uint16 const* par, auint n)
{
 auint  i;
 auint  o;
 auint  r;        /* Return: Cycles consumed */
 rrpge_cbp_setpal_t    cbp_setpal;
 rrpge_cbp_mode_t      cbp_mode;
 rrpge_cbp_getprops_t  cbp_getprops;
 rrpge_cbp_dropdev_t   cbp_dropdev;
 rrpge_cbp_getdidesc_t cbp_getdidesc;
 rrpge_cbp_getdi_t     cbp_getdi;
 rrpge_cbp_getai_t     cbp_getai;
 rrpge_cbp_popchar_t   cbp_popchar;
 rrpge_cbp_checkarea_t cbp_checkarea;
 rrpge_cbp_getlocal_t  cbp_getlocal;
 rrpge_cbp_getlang_t   cbp_getlang;


 if (n == 0U){    /* No parameters for the kernel call */
  goto fault_inv;
 }


 switch (par[0]){ /* At least one parameter is supplied, so can go */


  case 0x0100U:   /* Kernel task: Start loading binary data page */

   if (n != 5U){  /* Needs 1+4 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x0110U:   /* Kernel task: Start loading page from file */

   if (n != 7U){  /* Needs 1+6 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x0111U:   /* Kernel task: Start saving page into file */

   if (n != 7U){  /* Needs 1+6 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x0112U:   /* Kernel task: Find next file */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x0113U:   /* Kernel task: Move a file */

   if (n != 5U){  /* Needs 1+4 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800U;
   goto ret_callback;


  case 0x0300U:   /* Set palette entry */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   cbp_setpal.id  = par[1] & 0xFFU;
   cbp_setpal.col = par[2] & 0xFFFU;
   rrpge_m_edat->st.stat[RRPGE_STA_PAL + cbp_setpal.id] = cbp_setpal.col;
   rrpge_m_edat->cb_sub[RRPGE_CB_SETPAL](rrpge_m_edat, &cbp_setpal);

   r = 100U;
   goto ret_callback;


  case 0x0330U:   /* Change video mode */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] > 3U){ cbp_mode.mod = 0U; }
   else{             cbp_mode.mod = par[1]; }
   rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x12U] = cbp_mode.mod;
   rrpge_m_edat->cb_sub[RRPGE_CB_MODE](rrpge_m_edat, &cbp_mode);

   r = 100000U;
   goto ret_callback;


  case 0x0410U:   /* Get device properties */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_getprops.dev = par[1] & 0xFU; /* Device to query */
   rrpge_m_info.xr[7] = rrpge_m_edat->cb_fun[RRPGE_CB_GETPROPS](rrpge_m_edat, &cbp_getprops);
   rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x30U + (par[1] & 0xFU)] = rrpge_m_info.xr[7];

   r = 800U;
   goto ret_callback;


  case 0x0411U:   /* Drop device */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_dropdev.dev = par[1] & 0xFU; /* Device to drop */
   rrpge_m_edat->cb_sub[RRPGE_CB_DROPDEV](rrpge_m_edat, &cbp_dropdev);
   rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x30U + (par[1] & 0xFU)] = 0U;

   r = 800U;
   goto ret_callback;


  case 0x0412U:   /* Get digital input description symbols */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   cbp_getdidesc.dev = par[1] & 0xFU; /* Device to query */
   if (rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x30U + cbp_getdidesc.dev] != 0U){
    cbp_getdidesc.inp = (par[2] << 4) + (par[3] & 0xFU); /* Input to query */
    rrpge_m_info.xr[7] = rrpge_m_edat->cb_fun[RRPGE_CB_GETDIDESC](rrpge_m_edat, &cbp_getdidesc);
   }else{
    rrpge_m_info.xr[7] = 0U;
   }
   rrpge_m_info.xr[2] = rrpge_m_info.xr[7] >> 16;

   r = 800U;
   goto ret_callback;


  case 0x0422U:   /* Get digital inputs */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   cbp_getdi.dev = par[1] & 0xFU; /* Device to query */
   if (rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x30U + cbp_getdi.dev] != 0U){
    cbp_getdi.ing = par[2];       /* Input group to query */
    rrpge_m_info.xr[7] = rrpge_m_edat->cb_fun[RRPGE_CB_GETDI](rrpge_m_edat, &cbp_getdi);
   }else{
    rrpge_m_info.xr[7] = 0U;
   }

   r = 800U;
   goto ret_callback;


  case 0x0423U:   /* Get analog inputs */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   cbp_getai.dev = par[1] & 0xFU; /* Device to query */
   if (rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x30U + cbp_getai.dev] != 0U){
    cbp_getai.inp = par[2];       /* Input to query */
    rrpge_m_info.xr[7] = rrpge_m_edat->cb_fun[RRPGE_CB_GETAI](rrpge_m_edat, &cbp_getai);
   }else{
    rrpge_m_info.xr[7] = 0U;
   }

   r = 800U;
   goto ret_callback;


  case 0x0424U:   /* Pop text FIFO */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_popchar.dev = par[1] & 0xFU; /* Device to query */
   if (rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x30U + cbp_popchar.dev] != 0U){
    rrpge_m_info.xr[7] = rrpge_m_edat->cb_fun[RRPGE_CB_POPCHAR](rrpge_m_edat, &cbp_popchar);
   }else{
    rrpge_m_info.xr[7] = 0U;
   }
   rrpge_m_info.xr[2] = rrpge_m_info.xr[7] >> 16;

   r = 800U;
   goto ret_callback;


  case 0x0425U:   /* Return area activity */

   if (n != 6U){  /* Needs 1+5 parameters */
    goto fault_inv;
   }

   cbp_checkarea.dev = par[1] & 0xFU; /* Device to query */
   cbp_checkarea.x   = par[2];
   cbp_checkarea.y   = par[3];
   cbp_checkarea.w   = par[4];
   cbp_checkarea.h   = par[5];

   if ((cbp_checkarea.x & 0x8000U) != 0U){ /* Negative X */
    cbp_checkarea.w = (cbp_checkarea.w + cbp_checkarea.x) & 0xFFFFU;
    cbp_checkarea.x = 0U;
   }
   if ((cbp_checkarea.y & 0x8000U) != 0U){ /* Negative Y */
    cbp_checkarea.h = (cbp_checkarea.h + cbp_checkarea.y) & 0xFFFFU;
    cbp_checkarea.y = 0U;
   }
   if ((cbp_checkarea.x >= 640U) || (cbp_checkarea.x >= 400U)){
    cbp_checkarea.x = 0U;                  /* Off - screen */
    cbp_checkarea.y = 0U;
    cbp_checkarea.w = 0U;
    cbp_checkarea.h = 0U;
   }
   if ((cbp_checkarea.x + cbp_checkarea.w) >= 640U){
    cbp_checkarea.w = 640U - cbp_checkarea.x;
   }
   if ((cbp_checkarea.y + cbp_checkarea.h) >= 400U){
    cbp_checkarea.h = 400U - cbp_checkarea.y;
   }

   rrpge_m_info.xr[7] = rrpge_m_edat->cb_fun[RRPGE_CB_CHECKAREA](rrpge_m_edat, &cbp_checkarea);
   rrpge_m_info.xr[2] = rrpge_m_info.xr[7] >> 16;

   r = 1200U;
   goto ret_callback;


  case 0x0500U:   /* Delay some cycles */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   r = (auint)(par[1]) - (rrpge_m_prng() & 0x3FFU); /* 1024 cycle jitter */
   if (r > (auint)(par[1])){ r = 0U;   }            /* Underflow */
   if (r < 200U){            r = 200U; }            /* Minimal consumed cycles */
   break;


  case 0x0600U:   /* Get local users */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (rrpge_m_task_chkdata(par[1], 32U)){ /* Not in Data area */
    goto fault_inv;
   }

   cbp_getlocal.buf = &rrpge_m_edat->st.dram[par[1]];
   rrpge_m_edat->cb_sub[RRPGE_CB_GETLOCAL](rrpge_m_edat, &cbp_getlocal);

   r = 2400U;
   goto ret_callback;


  case 0x0601U:   /* Kernel task: Get UTF-8 representation of User ID */

   if (n != 6U){ /* Needs 1+5 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 1200U;
   goto ret_callback;


  case 0x0610U:   /* Get user preferred language */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_getlang.lno = par[1];
   rrpge_m_info.xr[7] = rrpge_m_edat->cb_fun[RRPGE_CB_GETLANG](rrpge_m_edat, &cbp_getlang);
   rrpge_m_info.xr[2] = rrpge_m_info.xr[7] >> 16;

   r = 2400U;
   goto ret_callback;


  case 0x0611U:   /* Get user preferred colors */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   rrpge_m_info.xr[7] = rrpge_m_edat->cb_fun[RRPGE_CB_GETCOLORS](rrpge_m_edat, RRPGE_M_NULL);
   rrpge_m_info.xr[2] = rrpge_m_info.xr[7] >> 16;

   r = 2400U;
   goto ret_callback;


  case 0x0700U:   /* Kernel task: Send data to user */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 2400U;
   goto ret_callback;


  case 0x0701U:   /* Poll for packets */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if (rrpge_m_task_chkdata(par[1], par[2])){ /* Not in Data area */
    goto fault_inv;
   }

   if (rrpge_m_task_chkdata(par[3], 8U)){ /* Not in Data area */
    goto fault_inv;
   }

   /* Note: No validity checking in rrpge_m_edat. These elements are
   ** maintained by the library, they shouldn't be wrong. */

   rrpge_m_edat->reir &= 0x3FU; /* Some sanity masks to always prevent */
   rrpge_m_edat->reiw &= 0x3FU; /* addressing out of array */
   rrpge_m_edat->rebr &= 0xFFFU;
   rrpge_m_edat->rebw &= 0xFFFU;

   if ((rrpge_m_edat->reir) == (rrpge_m_edat->reiw)){ /* No packet */

    rrpge_m_info.xr[7] = 0U;

   }else{                                             /* There are packets */

    rrpge_m_edat->recl[rrpge_m_edat->reir] &= 0xFFFU; /* Sanity mask */

    r = rrpge_m_edat->reir;
    for (i = 0U; i < 8U; i++){  /* Copy user ID data into target */
     rrpge_m_edat->st.dram[par[3] + i] = rrpge_m_edat->reci[(r << 3) + i];
    }

    r = rrpge_m_edat->recl[r];
    if (r > par[2]){ r = par[2]; }
    for (i = 0U; i < r; i++){   /* Copy packet data */
     rrpge_m_edat->st.dram[par[1] + i] = rrpge_m_edat->recb[rrpge_m_edat->rebr];
     rrpge_m_edat->rebr = (rrpge_m_edat->rebr + 1U) & 0xFFFU;
    }

    rrpge_m_info.xr[7] = r;     /* A: the length of the packet */

    rrpge_m_edat->reir = (rrpge_m_edat->reir + 1U) & 0x3FU;

   }

   r = 2400U;
   break;


  case 0x0710U:   /* Kernel task: List accessible users */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 2400U;
   goto ret_callback;


  case 0x0720U:   /* Set network availability */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] != 0U){ rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x1FU] = 1U; }
   else             { rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x1FU] = 0U; }

   r = 400U;
   break;


  case 0x0721U:   /* Query network availability */

   if (n != 1U){  /* Needs 1+0 parameters */
    goto fault_inv;
   }

   rrpge_m_info.xr[7] = rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x1FU] = 1U;

   r = 400U;
   break;


  case 0x0800U:   /* Query task */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] >= 0x10U){ rrpge_m_info.xr[7] = 0xFFFFU; }
   else                { rrpge_m_info.xr[7] = rrpge_m_edat->st.stat[RRPGE_STA_KTASK + 0xFU + (par[1] << 4)]; }

   r = 400U;
   break;


  case 0x0801U:   /* Discard task */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] < 0x10U){
    o = RRPGE_STA_KTASK + (par[1] << 4);
    /* Only discards completed tasks */
    if ((rrpge_m_edat->st.stat[o] & 0x8000U) != 0U){
     rrpge_m_edat->st.stat[o] = 0U;
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

 rrpge_m_info.hlt |= RRPGE_HLT_CALLBACK;
 return r;

fault_inv:        /* Return with invalid kernel call */

 rrpge_m_info.hlt |= RRPGE_HLT_INVKCALL;
 return 0;

}
