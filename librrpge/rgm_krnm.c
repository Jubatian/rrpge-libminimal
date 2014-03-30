/**
**  \file
**  \brief     Kernel functionality
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.27
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
 for (i=0xD80U; i<0xE80U; i+=0x10U){
  if (rrpge_m_edat->stat.ropd[i + 0xFU] == 0){
   for (j=0; j<n; j++){
    rrpge_m_edat->stat.ropd[i + j] = par[j];
   }
   rrpge_m_edat->stat.ropd[i + 0xFU] = 0x0001U;
   j = ((i - 0xD80U) >> 4);
   rrpge_m_edat->tsfl &= ~(1U << j); /* Task not started yet! */
   rrpge_m_info.xr[0] = j;
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
 rrpge_cbp_setcolor_t cbp_setcolor;
 rrpge_cbp_inputcom_t cbp_inputcom;


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

   if (n!=4){     /* Needs 1+3 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n)!=0) goto fault_inv;
   r = 800;
   break;


  case 0x0110U:   /* Kernel task: Start loading nonvolatile save */

   if (n!=6){     /* Needs 1+5 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n)!=0) goto fault_inv;
   r = 800;
   break;


  case 0x0111U:   /* Kernel task: Start saving nonvolatile save */

   if (n!=6){     /* Needs 1+5 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n)!=0) goto fault_inv;
   r = 800;
   break;


  case 0x0112U:   /* Kernel task: List available NV saves */

   if (n!=3){     /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n)!=0) goto fault_inv;
   r = 800;
   break;


  case 0x0120U:   /* Load arbitrary file */

   if (n!=3){     /* Needs 1+2 parameters */
    goto fault_inv;
   }

   /* !!! Not implemented yet. Needs the reset state of the Video peripheral !!! */
   goto fault_inv;
   r = 0;         /* No defined cycle count */
   break;


  case 0x0121U:   /* Save arbitrary file */

   if (n!=4){     /* Needs 1+3 parameters */
    goto fault_inv;
   }

   /* !!! Not implemented yet. Needs the reset state of the Video peripheral !!! */
   goto fault_inv;
   r = 0;         /* No defined cycle count */
   break;


  case 0x0210U:   /* Set audio interrupt */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   rrpge_m_edat->stat.ropd[0xD31U] = par[1];
   r = 150;
   break;


  case 0x0300U:   /* Set palette entry */

   if (n!=3){     /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if ((par[1] & 0xFFU) <= rrpge_m_info.vbm){
    cbp_setcolor.id  = par[1] & 0xFFU;
    cbp_setcolor.col = par[2] & 0xFFFU;
    rrpge_m_edat->cb_sub[RRPGE_CB_SETCOLOR](rrpge_m_edat, &cbp_setcolor);
   }

   rrpge_m_edat->stat.ropd[0xC00U + (par[1] & 0xFFU)] = par[2] & 0xFFFU;

   r = rrpge_m_kvstall(100);
   break;


  case 0x0310U:   /* Set raster interrupt */

   if (n!=3){     /* Needs 1+2 parameters */
    goto fault_inv;
   }

   rrpge_m_edat->stat.ropd[0xD30U] = par[1];
   rrpge_m_edat->stat.ropd[0xD20U] = par[2];

   r = rrpge_m_kvstall(150);
   break;


  case 0x0320U:   /* Query current display line */

   if (n!=1){     /* Needs 1+0 parameters */
    goto fault_inv;
   }

   if (rrpge_m_info.vln < 400U){ /* Normal display lines */
    rrpge_m_info.xr[0] = rrpge_m_info.vln; /* A: Current video line */
   }else{                        /* VBlank lines */
    rrpge_m_info.xr[0] = (0x10000U - RRPGE_M_VLN) + rrpge_m_info.vln;
   }

   r = rrpge_m_kvstall(100);
   break;


  case 0x0400U:   /* Digital joy: Read device availability */

   if (n!=1){     /* Needs 1+0 parameters */
    goto fault_inv;
   }

   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_IDIG_AVA](rrpge_m_edat, RRPGE_M_NULL);

   r = 800;
   break;


  case 0x0401U:   /* Digital joy: Read controls */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_inputcom.sel = par[1];
   r = rrpge_m_edat->cb_fun[RRPGE_CB_IDIG_CTR](rrpge_m_edat, &cbp_inputcom);
   rrpge_m_info.xr[0] = r;       /* Controls in A */

   r = 800;
   break;


  case 0x0410U:   /* Analog joy: Read device availability */

   if (n!=1){     /* Needs 1+0 parameters */
    goto fault_inv;
   }

   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_IANA_AVA](rrpge_m_edat, RRPGE_M_NULL);

   r = 800;
   break;


  case 0x0411U:   /* Analog joy: Read controls */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_inputcom.sel = par[1];
   r = rrpge_m_edat->cb_fun[RRPGE_CB_IANA_CTR](rrpge_m_edat, &cbp_inputcom);
   rrpge_m_info.xr[0] = r;       /* Controls in A */

   r = 800;
   break;


  case 0x0412U:   /* Analog joy: Read analog directions */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_inputcom.sel = par[1];
   r = rrpge_m_edat->cb_fun[RRPGE_CB_IANA_POS](rrpge_m_edat, &cbp_inputcom);
   rrpge_m_info.xr[0] = r;       /* X direction in A */
   rrpge_m_info.xr[2] = r >> 16; /* Y direction in C */

   r = 800;
   break;


  case 0x0420U:   /* Mice: Read device availability */

   if (n!=1){     /* Needs 1+0 parameters */
    goto fault_inv;
   }

   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_IMOU_AVA](rrpge_m_edat, RRPGE_M_NULL);

   r = 800;
   break;


  case 0x0421U:   /* Mice: Read controls */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_inputcom.sel = par[1];
   r = rrpge_m_edat->cb_fun[RRPGE_CB_IMOU_CTR](rrpge_m_edat, &cbp_inputcom);
   rrpge_m_info.xr[0] = r;       /* Controls in A */

   r = 800;
   break;


  case 0x0422U:   /* Mice: Read position */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_inputcom.sel = par[1];
   r = rrpge_m_edat->cb_fun[RRPGE_CB_IMOU_POS](rrpge_m_edat, &cbp_inputcom);
   rrpge_m_info.xr[0] = r;       /* X position in A */
   rrpge_m_info.xr[2] = r >> 16; /* Y position in C */

   r = 800;
   break;


  case 0x0423U:   /* Mice: Read cursor requirement */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_inputcom.sel = par[1];
   r = rrpge_m_edat->cb_fun[RRPGE_CB_IMOU_CUR](rrpge_m_edat, &cbp_inputcom);
   rrpge_m_info.xr[0] = r;       /* Cursor req. in A */

   r = 800;
   break;


  case 0x0430U:   /* Text: Read device availability */

   if (n!=1){     /* Needs 1+0 parameters */
    goto fault_inv;
   }

   rrpge_m_info.xr[0] = rrpge_m_edat->cb_fun[RRPGE_CB_ITXT_AVA](rrpge_m_edat, RRPGE_M_NULL);

   r = 800;
   break;


  case 0x0434U:   /* Text: Get UTF32 character from input fifo */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   cbp_inputcom.sel = par[1];
   r = rrpge_m_edat->cb_fun[RRPGE_CB_ITXT_RFI](rrpge_m_edat, &cbp_inputcom);
   rrpge_m_info.xr[0] = r;       /* Low word in A */
   rrpge_m_info.xr[2] = r >> 16; /* High word in C */

   r = 800;
   break;


  case 0x0500U:   /* Delay some cycles */

   if (n!=2){     /* Needs 1+1 parameters */
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


  case 0x0600U:   /* Read local users */

   if (n!=3){     /* Needs 1+2 parameters */
    goto fault_inv;
   }

   if ( (par[1] < 0x4000U) || (par[1] >= 0x40E0U) ){ /* Not an RW page */
    goto fault_inv;
   }

   o = (((auint)(par[1] - 0x4000U)) << 12) + (auint)(par[2] & 0x0FE0U);
   for (i=0; i<32; i++){ /* Copy user ID data into target */
    rrpge_m_edat->stat.dram[o + i] = rrpge_m_edat->stat.ropd[0xE80U + i];
   }

   r = 2400;
   break;


  case 0x0601U:   /* Kernel task: Read UTF-8 representation of user */

   if (n!=11){    /* Needs 1+10 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n)!=0) goto fault_inv;
   r = 1200;
   break;


  case 0x0610U:   /* Read user preferred language */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] > 4){
    rrpge_m_info.xr[0] = 0;
    rrpge_m_info.xr[2] = 0;
   }else{
    rrpge_m_info.xr[0] = rrpge_m_edat->stat.ropd[0xEA1U + (par[1] << 1)];
    rrpge_m_info.xr[2] = rrpge_m_edat->stat.ropd[0xEA0U + (par[1] << 1)];
   }

   r = 2400;
   break;


  case 0x0611U:   /* Read user preferred colors */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   rrpge_m_info.xr[0] = rrpge_m_edat->stat.ropd[0xEA9U];
   rrpge_m_info.xr[2] = rrpge_m_edat->stat.ropd[0xEA8U];

   r = 2400;
   break;


  case 0x0700U:   /* Kernel task: Send data to user */

   if (n!=11){    /* Needs 1+10 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n)!=0) goto fault_inv;
   r = 2400;
   break;


  case 0x0701U:   /* Poll for packets */

   if (n!=4){     /* Needs 1+3 parameters */
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

   if (n!=11){    /* Needs 1+10 parameters */
    goto fault_inv;
   }

   if (rrpge_m_ktsalloc(par, n)!=0) goto fault_inv;
   r = 2400;
   break;


  case 0x0720U:   /* Set network availability */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] != 0){ rrpge_m_edat->stat.ropd[0xD3FU] = 1U; }
   else            { rrpge_m_edat->stat.ropd[0xD3FU] = 0U; }

   r = 400;
   break;


  case 0x0800U:   /* Query task */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] >= 0x10U){ rrpge_m_info.xr[0] = 0xFFFFU; }
   else                { rrpge_m_info.xr[0] = rrpge_m_edat->stat.ropd[0xD8FU + (par[1] << 4)]; }

   r = 400;
   break;


  case 0x0801U:   /* Discard task */

   if (n!=2){     /* Needs 1+1 parameters */
    goto fault_inv;
   }

   if (par[1] < 0x10U){
    o = 0xD8FU + (par[1] << 4);
    /* Only discards completed tasks */
    if ((rrpge_m_edat->stat.ropd[o] & 0x8000U) != 0){
     rrpge_m_edat->stat.ropd[o] = 0;
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
