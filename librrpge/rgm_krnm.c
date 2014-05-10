/**
**  \file
**  \brief     Kernel functionality
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.10
*/


#include "rgm_krnm.h"
#include "rgm_mix.h"
#include "rgm_prng.h"
#include "rgm_task.h"



/* Finds empty kernel task slot and populates & allocates it. Fills in the CPU
** A register (in info) according to the result. Returns nonzero if the
** parameters don't make a valid task. (does not check parameter count!) */
static auint rrpge_m_ktsalloc(uint16 const* par, auint n)
{
 auint i;
 auint j;
 for (i = 0xD80U; i < 0xE80U; i += 0x10U){
  if (rrpge_m_edat->stat.ropd[i + 0xFU] == 0U){
   for (j = 0U; j < n; j++){
    rrpge_m_edat->stat.ropd[i + j] = par[j];
   }
   rrpge_m_edat->stat.ropd[i + 0xFU] = 0x0001U;
   j = ((i - 0xD80U) >> 4);
   rrpge_m_edat->tsfl &= ~((auint)(1U) << j); /* Task not started yet! */
   rrpge_m_info.xr[0] = j;                    /* Fill in register 'a' */
   return rrpge_m_taskcheck(&(rrpge_m_edat->stat.ropd[0]), j);
  }
 }
 rrpge_m_info.xr[0] = 0x8000U;
 return 0;
}



/* Calculates video stall for video related kernel functions. Gets the base
** cycle requirement for the kernel call, returns the cycle requirements
** including the stall. */
static auint rrpge_m_kvstall(auint r)
{
 if (rrpge_m_info.vac != 0){
  r += rrpge_m_info.vac;        /* Accelerator function running */
 }else if (rrpge_m_info.vsm == 4){
  r += 400U - rrpge_m_info.vlc; /* Remaining cycles from a blocking line */
 }
 return r;
}



/* Checks and saves a memory page for Read. Returns nonzero if the page is
** unsuitable for reading. */
static auint rrpge_m_bankread(auint pg, auint tg)
{
 if ( ((pg < 0x4000U) || (pg >= 0x40E0U)) && /* Not an RW page */
      ((pg < 0x8000U) || (pg >= 0x8080U)) && /* Not a VRAM page */
      (pg != 0x7FFFU) && /* Not the audio peripheral page */
      (pg != 0xBFFFU) && /* Not the video peripheral page */
      (pg != 0x40E0U) ){ /* Not the Read Only Process Descriptor */
  return 1U;
 }
 rrpge_m_info.rbk[tg & 0xFU] = pg << 12; /* All OK, switch pages */
 rrpge_m_edat->stat.ropd[0xD00U + (tg & 0xFU)] = pg;
 return 0U;
}



/* Checks and saves a memory page for Write. Returns nonzero if the page is
** unsuitable for writing. */
static auint rrpge_m_bankwrite(auint pg, auint tg)
{
 if ( ((pg < 0x4000U) || (pg >= 0x40E0U)) && /* Not an RW page */
      ((pg < 0x8000U) || (pg >= 0x8080U)) && /* Not a VRAM page */
      (pg != 0x7FFFU) && /* Not the audio peripheral page */
      (pg != 0xBFFFU) ){ /* Not the video peripheral page */
  return 1U;
 }
 rrpge_m_info.wbk[tg & 0xFU] = pg << 12; /* All OK, switch pages */
 rrpge_m_edat->stat.ropd[0xD10U + (tg & 0xFU)] = pg;
 return 0U;
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
 rrpge_cbp_getdidesc_t cbp_getdidesc;
 rrpge_cbp_getdi_t     cbp_getdi;
 rrpge_cbp_getai_t     cbp_getai;
 rrpge_cbp_popchar_t   cbp_popchar;
 rrpge_cbp_settouch_t  cbp_settouch;
 rrpge_cbp_getlocal_t  cbp_getlocal;
 rrpge_cbp_getlang_t   cbp_getlang;


 if (n==0){       /* No parameters for the kernel call */
  goto fault_inv;
 }

 /* Notes for memory clearing actions:
 ** In the case of kernel tasks clearing the target area (if necessary) is
 ** done at the scheduling of the kernel task (it belongs there). So for
 ** kernel tasks there is no target initialization action here. */

 switch (par[0]){ /* At least one parameter is supplied, so can go */


  case 0x0000U:   /* Bank in Read memory page */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if ( (rrpge_m_info.wbk[par[1] & 0xFU]) >= 0x8000000U ){
    goto fault_inv;   /* Corresponding write page is VRAM */
   }
   if (rrpge_m_bankread (par[2], par[1]) != 0U){ goto fault_inv; } /* Can not read this */
   r = 150;

   break;


  case 0x0001U:   /* Bank in Write memory page */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if (par[2] >= 0x8000U){ /* Trying to bank in a VRAM write page or peripheral */
    if ( ((rrpge_m_info.rbk[par[1] & 0xFU]) >> 12) != (auint)(par[2]) ){
     goto fault_inv;  /* Corresponding read page does not match */
    }
   }
   if (rrpge_m_bankwrite(par[2], par[1]) != 0U){ goto fault_inv; } /* Can not write this */
   r = 150;

   break;


  case 0x0002U:   /* Bank in Read & Write memory pages */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if (par[3] >= 0x8000U){ /* Video RAM or peripheral requested for writing */
    if (par[3] != par[2]){ /* Does not match the Read page */
     goto fault_inv;
    }
   }
   if (rrpge_m_bankread (par[2], par[1]) != 0U){ goto fault_inv; } /* Can not read this */
   if (rrpge_m_bankwrite(par[3], par[1]) != 0U){ goto fault_inv; } /* Can not write this */
   r = 150;

   break;


  case 0x0003U:   /* Bank in same page for Read & Write */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if (rrpge_m_bankread (par[2], par[1]) != 0U){ goto fault_inv; } /* Can not read this */
   if (rrpge_m_bankwrite(par[2], par[1]) != 0U){ goto fault_inv; } /* Can not write this */
   r = 150;

   break;


  case 0x0100U:   /* Kernel task: Start loading binary data page */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800;
   break;


  case 0x0110U:   /* Kernel task: Start loading page from file */

   if (n != 7U){  /* Needs 1+6 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800;
   break;


  case 0x0111U:   /* Kernel task: Start saving page into file */

   if (n != 7U){  /* Needs 1+6 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800;
   break;


  case 0x0112U:   /* Kernel task: Find next file */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800;
   break;


  case 0x0113U:   /* Kernel task: Move a file */

   if (n != 5U){  /* Needs 1+4 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 800;
   break;


  case 0x0210U:   /* Set audio event handler */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   rrpge_m_edat->stat.ropd[0xD31U] = par[1];
   r = 150;
   break;


  case 0x0300U:   /* Set palette entry */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   cbp_setpal.id  = par[1] & 0xFFU;
   cbp_setpal.col = par[2] & 0xFFFU;
   rrpge_m_edat->stat.ropd[0xC00U + (par[1] & 0xFFU)] = cbp_setpal.col;
   rrpge_m_edat->cb_sub[RRPGE_CB_SETPAL](rrpge_m_edat, &cbp_setpal);

   r = rrpge_m_kvstall(100);
   break;


  case 0x0310U:   /* Set video event handler */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   rrpge_m_edat->stat.ropd[0xD30U] = par[1];
   rrpge_m_edat->stat.ropd[0xD20U] = par[2];

   r = rrpge_m_kvstall(150);
   break;


  case 0x0320U:   /* Query current display line */

   if (n != 1U){  /* Needs 1+0 parameters */
    goto fault_inv;
   }

   if (rrpge_m_info.vln < 400U){ /* Normal display lines */
    rrpge_m_info.xr[0] = rrpge_m_info.vln; /* A: Current video line */
   }else{                        /* VBlank lines */
    rrpge_m_info.xr[0] = (0x10000U - RRPGE_M_VLN) + rrpge_m_info.vln;
   }

   r = rrpge_m_kvstall(100);
   break;


  case 0x0330U:   /* Change video mode */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] == 1U){ cbp_mode.mod = 1U; }
   else{              cbp_mode.mod = 0U; }
   rrpge_m_edat->stat.ropd[0xD57U] = cbp_mode.mod;
   rrpge_m_edat->cb_sub[RRPGE_CB_MODE](rrpge_m_edat, &cbp_mode);

   r = rrpge_m_kvstall(100000);
   break;


  case 0x0400U:   /* Get input device availability */

   if (n != 1U){  /* Needs 1+0 parameters */
    goto fault_inv;
   }

   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_GETDEV](rrpge_m_edat, RRPGE_M_NULL);

   r = 800;
   break;


  case 0x0410U:   /* Get device properties */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_getprops.dev = par[1] & 0xFU; /* Device to query */
   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_GETPROPS](rrpge_m_edat, &cbp_getprops);

   r = 800;
   break;


  case 0x0411U:   /* Get digital input description symbols */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   cbp_getdidesc.dev = par[1] & 0xFU; /* Device to query */
   cbp_getdidesc.inp = (par[2] << 4) + (par[3] & 0xFU); /* Input to query */
   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_GETDIDESC](rrpge_m_edat, &cbp_getdidesc);
   rrpge_m_info.xr[2] = rrpge_m_info.xr[0] >> 16;

   r = 800;
   break;


  case 0x0420U:   /* Get digital inputs */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   cbp_getdi.dev = par[1] & 0xFU; /* Device to query */
   cbp_getdi.ing = par[2];        /* Input group to query */
   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_GETDI](rrpge_m_edat, &cbp_getdi);

   r = 800;
   break;


  case 0x0421U:   /* Get analog inputs */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   cbp_getai.dev = par[1] & 0xFU; /* Device to query */
   cbp_getai.inp = par[2];        /* Input to query */
   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_GETAI](rrpge_m_edat, &cbp_getai);

   r = 800;
   break;


  case 0x0423U:   /* Pop text FIFO */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_popchar.dev = par[1] & 0xFU; /* Device to query */
   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_POPCHAR](rrpge_m_edat, &cbp_popchar);
   rrpge_m_info.xr[2] = rrpge_m_info.xr[0] >> 16;

   r = 800;
   break;


  case 0x0430:    /* Define touch sensitive area */

   if (n != 6U){  /* Needs 1+5 parameters */
    goto fault_inv;
   }

   cbp_settouch.aid = par[1] & 0xFU; /* Device to query */
   cbp_settouch.x   = par[2];
   cbp_settouch.y   = par[3];
   cbp_settouch.w   = par[4];
   cbp_settouch.h   = par[5];

   if ((cbp_settouch.x & 0x8000U) != 0U){ /* Negative X */
    cbp_settouch.w = (cbp_settouch.w + cbp_settouch.x) & 0xFFFFU;
    cbp_settouch.x = 0U;
   }
   if ((cbp_settouch.y & 0x8000U) != 0U){ /* Negative Y */
    cbp_settouch.h = (cbp_settouch.h + cbp_settouch.y) & 0xFFFFU;
    cbp_settouch.y = 0U;
   }
   if ((cbp_settouch.x >= 640U) || (cbp_settouch.x >= 400U)){
    cbp_settouch.x = 0U;                  /* Off - screen */
    cbp_settouch.y = 0U;
    cbp_settouch.w = 0U;
    cbp_settouch.h = 0U;
   }
   if ((cbp_settouch.x + cbp_settouch.w) >= 640U){
    cbp_settouch.w = 640U - cbp_settouch.x;
   }
   if ((cbp_settouch.y + cbp_settouch.h) >= 400U){
    cbp_settouch.h = 400U - cbp_settouch.y;
   }
   rrpge_m_edat->stat.ropd[0xE80U + cbp_settouch.aid] = cbp_settouch.x;
   rrpge_m_edat->stat.ropd[0xE90U + cbp_settouch.aid] = cbp_settouch.y;
   rrpge_m_edat->stat.ropd[0xEA0U + cbp_settouch.aid] = cbp_settouch.w;
   rrpge_m_edat->stat.ropd[0xEB0U + cbp_settouch.aid] = cbp_settouch.h;

   rrpge_m_edat->cb_sub[RRPGE_CB_SETTOUCH](rrpge_m_edat, &cbp_settouch);

   r = 800;
   break;


  case 0x0500U:   /* Delay some cycles */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   i = rrpge_m_edat->stat.ropd[0xD20U];             /* Raster line to fire IT at */
   if (i > 400U) i = 400U;
   if (rrpge_m_info.vln <= i) i -= rrpge_m_info.vln;
   else                       i = i + RRPGE_M_VLN - rrpge_m_info.vln;
   i = i * 400U;                                    /* Cycles (roughly) until next IT */

   r = (auint)(par[1]) - (rrpge_m_prng() & 0x3FFU); /* 1024 cycle jitter */
   if (r > (auint)(par[1]))  r = 0;                 /* Underflow */
   if (r > rrpge_m_info.auc) r = rrpge_m_info.auc;  /* Constrain to audio */
   if (r > i)                r = i;                 /* Constrain to video */
   if (r < 200U)             r = 200;               /* Minimal consumed cycles */
   break;


  case 0x0600U:   /* Get local users */

   if (n != 3U){  /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if ( (par[1] < 0x4000U) || (par[1] >= 0x40E0U) ){ /* Not an RW page */
    goto fault_inv;
   }

   o = (((auint)(par[1] & 0xFFU)) << 12) + (auint)(par[2] & 0x0FE0U);
   cbp_getlocal.buf = &rrpge_m_edat->stat.dram[o];
   rrpge_m_edat->cb_sub[RRPGE_CB_GETLOCAL](rrpge_m_edat, &cbp_getlocal);

   r = 2400;
   break;


  case 0x0601U:   /* Kernel task: Get UTF-8 representation of User ID */

   if (n != 11U){ /* Needs 1+10 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 1200;
   break;


  case 0x0610U:   /* Get user preferred language */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_getlang.lno = par[1];
   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_GETLANG](rrpge_m_edat, &cbp_getlang);
   rrpge_m_info.xr[2] = rrpge_m_info.xr[0] >> 16;

   r = 2400;
   break;


  case 0x0611U:   /* Get user preferred colors */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_GETCOLORS](rrpge_m_edat, RRPGE_M_NULL);
   rrpge_m_info.xr[2] = rrpge_m_info.xr[0] >> 16;

   r = 2400;
   break;


  case 0x0700U:   /* Kernel task: Send data to user */

   if (n != 11U){ /* Needs 1+10 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 2400;
   break;


  case 0x0701U:   /* Poll for packets */

   if (n != 4U){  /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if ( (par[1] < 0x4000U) || (par[1] >= 0x40E0U) ){ /* Not an RW page */
    goto fault_inv;
   }

   if ( (par[2] < 0x4000U) || (par[2] >= 0x40E0U) ){ /* Not an RW page */
    goto fault_inv;
   }

   /* Note: No validity checking in rrpge_m_edat. These elements are
   ** maintained by the library, they shouldn't be wrong. */

   rrpge_m_edat->reir &= 0x3FU; /* Some sanity masks to always prevent */
   rrpge_m_edat->reiw &= 0x3FU; /* addressing out of array */
   rrpge_m_edat->rebr &= 0xFFFU;
   rrpge_m_edat->rebw &= 0xFFFU;

   if ((rrpge_m_edat->reir) == (rrpge_m_edat->reiw)){ /* No packet */
    rrpge_m_info.xr[0] = 0;
   }else{                                             /* There are packets */

    rrpge_m_edat->recl[rrpge_m_edat->reir] &= 0xFFFU; /* Sanity mask */

    r = rrpge_m_edat->reir;
    o = (((auint)(par[2] - 0x4000U)) << 12) + (auint)(par[3] & 0x0FF8U);
    for (i=0; i<8; i++){        /* Copy user ID data into target */
     rrpge_m_edat->stat.dram[o + i] = rrpge_m_edat->reci[(r << 3) + i];
    }

    r = rrpge_m_edat->recl[r];
    o = ((auint)(par[1] - 0x4000U)) << 12;
    for (i=0; i<r; i++){        /* Copy packet data */
     rrpge_m_edat->stat.dram[o + i] = rrpge_m_edat->recb[rrpge_m_edat->rebr];
     rrpge_m_edat->rebr = (rrpge_m_edat->rebr + 1U) & 0xFFFU;
    }

    rrpge_m_info.xr[0] = r;     /* A: the length of the packet */

    rrpge_m_edat->reir = (rrpge_m_edat->reir + 1U) & 0x3FU;

   }

   r = 2400;
   break;


  case 0x0710U:   /* Kernel task: List accessible users */

   if (n != 11U){ /* Needs 1+10 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n) != 0U){ goto fault_inv; }
   r = 2400;
   break;


  case 0x0720U:   /* Set network availability */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] != 0U){ rrpge_m_edat->stat.ropd[0xD3FU] = 1U; }
   else             { rrpge_m_edat->stat.ropd[0xD3FU] = 0U; }

   r = 400;
   break;


  case 0x0800U:   /* Query task */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] >= 0x10U){ rrpge_m_info.xr[0] = 0xFFFFU; }
   else                { rrpge_m_info.xr[0] = rrpge_m_edat->stat.ropd[0xD8FU + (par[1] << 4)]; }

   r = 400;
   break;


  case 0x0801U:   /* Discard task */

   if (n != 2U){  /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] < 0x10U){
    o = 0xD8FU + (par[1] << 4);
    /* Only discards completed tasks */
    if ((rrpge_m_edat->stat.ropd[o] & 0x8000U) != 0U){
     rrpge_m_edat->stat.ropd[o] = 0U;
    }
   }

   r = 100;
   break;


  default:        /* Unimplemented */

   goto fault_inv;
   break;

 }

 return r;

fault_inv:

 rrpge_m_info.hlt |= RRPGE_HLT_INVKCALL;
 return 0;

}
