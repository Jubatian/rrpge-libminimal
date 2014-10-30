/**
**  \file
**  \brief     FIFO (Accelerator, Mixer) related operations.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.29
**
**
**  Contains the Graphics & Mixer FIFO's logic. The FIFOs operate detached
**  from the graphics & audio emulation as such precision is not required,
**  however they necessarily contain the logic to interact with the
**  Accelerator and Mixer, and each other regarding stalls.
*/


#include "rgm_fifo.h"
#include "rgm_grop.h"
#include "rgm_mix.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* Internal function to update an Accelerator register by address, also
** triggering the operation if necessary. */
RRPGE_M_FASTCALL static void rrpge_m_fifoacc(auint adr, auint val)
{
 if ((adr & 0x100U) == 0U){
  rrpge_m_edat->st.stat[RRPGE_STA_ACC   + (adr & 0x1FU)] = (uint16)(val);
  if ((adr & 0x1FU) == 0x0FU){ /* Trigger */
   rrpge_m_info.cyf[1] += rrpge_m_grop_accel();
   rrpge_m_edat->st.stat[RRPGE_STA_UPA_GF + 1U] |= 1U; /* FIFO / Peripheral working */
  }
 }else{
  rrpge_m_edat->st.stat[RRPGE_STA_REIND + (adr & 0xFFU)] = (uint16)(val);
 }
}



/* Internal function to update a Mixer register by address, also triggering
** the operation if necessary. */
RRPGE_M_FASTCALL static void rrpge_m_fifomix(auint adr, auint val)
{
 rrpge_m_edat->st.stat[RRPGE_STA_MIXER + (adr & 0xFU)] = (uint16)(val);
 if ((adr & 0xFU) == 0xFU){ /* Trigger */
  rrpge_m_info.cyf[0] += rrpge_m_mixerop();
  rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + 1U] |= 1U; /* FIFO / Peripheral working */
 }
}



/* Internal function to generate address from a given FIFO pointer and
** position & size register value */
RRPGE_M_FASTCALL static auint rrpge_m_fifoadr(auint ptr, auint ps)
{
 auint ms = 0xFFFFFFFFU << ((ps >> 12) + 8U);
 return ((ptr & (~ms)) | ((ps << 8) & ms)) & (PRAMS - 1U);
}



/* Emulates Graphics and Mixer FIFO for the given amount of cycles. Uses the
** cys (stall), cya (accelerator) and cym (mixer) members of the rrpge_m_info
** structure, updating them as appropriate. */
void  rrpge_m_fifoproc(auint cy)
{
 auint v, t, i;

 /* Check stall cycles */

 if (cy <= rrpge_m_info.cys){
  rrpge_m_info.cys -= cy;
  return;                     /* Can not emulate anything (stalled) */
 }
 cy -= rrpge_m_info.cys;
 rrpge_m_info.cys = 0U;       /* Stall cycles consumed, rest can be used */

 /* Run emulation (truly this is the asynchronous Peripheral bus' emulation
 ** task, excluding the Graphics Display Generator) */

 for (i = 0U; i < 2U; i++){

  while ( (cy != 0U) &&           /* There are cycles to work from */
          (rrpge_m_info.cyf[i] != 0U) ){ /* There are FIFO cycles are to be consumed */

   if (rrpge_m_info.cyf[i] > cy){ /* Can't consume all */

    rrpge_m_info.cyf[i] -= cy;
    cy = 0U;

   }else{                         /* Consumes all: attempt to step emulation */

    cy -= rrpge_m_info.cyf[i];
    rrpge_m_info.cyf[i] = 0U;

    if ( rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x20U + (i << 3)] ==
         rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x21U + (i << 3)] ){ /* FIFO drained */

     rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + 1U + (i << 2)] &= ~1U; /* Empty, peripheral idle */

    }else{                       /* There is data in the FIFO */

     if ((rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + 1U + (i << 2)] & 2U) == 0U){ /* Not suspended */

      t = rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x21U + (i << 3)];
      v = rrpge_m_edat->st.pram[rrpge_m_fifoadr(t, rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + 0x0U + (i << 2)])];
      t ++;
      rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x21U + (i << 3)] = t;
      rrpge_m_info.cyf[i] = 2U;   /* Cycles consumed by FIFO access */
      if (i == 0U){ rrpge_m_fifomix(v >> 16, v); }
      else        { rrpge_m_fifoacc(v >> 16, v); }

     }

    }

   }

  }

 }

}



/* Operates the memory mapped interface of the FIFOs. Only the low 3 bits of
** the address are used (low 4 addresses selecting the Mixer FIFO, high 4
** addresses the Graphics FIFO). Only low 16 bits of value are used. */
void  rrpge_m_fifowrite(auint adr, auint val)
{
 auint t, u, p;

 adr = adr & 7U;

 switch (adr & 0x3U){

  case 0U:                    /* FIFO location & size */

   rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + adr] = (uint16)(val);
   break;

  case 1U:                    /* FIFO status flags */

   break;

  case 2U:                    /* FIFO address latch */

   t = RRPGE_STA_VARS + 0x24U + ((adr & 4U) << 1);
   if ((val & 0x8000U) == 0U){
    rrpge_m_edat->st.stat[t] ++;
   }else{
    rrpge_m_edat->st.stat[t] = (uint16)(val);
   }
   rrpge_m_edat->st.stat[t] &= 0x7FFFU;
   break;

  default:                    /* Store trigger */

   t = RRPGE_STA_VARS + 0x24U + ((adr & 4U) << 1); /* Address latch's address in state */

   if ( (rrpge_m_info.cyf[(adr >> 2)] |
         rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + adr - 2U]) == 0U){ /* Bypass */

    u = rrpge_m_edat->st.stat[t];                  /* Read the address latch */
    if ((adr & 4U) == 0U){ rrpge_m_fifomix(u, val); }
    else                 { rrpge_m_fifoacc(u, val); }

   }else{                     /* No bypass */

    u = rrpge_m_edat->st.stat[t - 4U];             /* Write pointer value */
    p = rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + adr - 3U]; /* FIFO position & size */
    rrpge_m_edat->st.pram[rrpge_m_fifoadr(u, p)] =
     ((auint)(rrpge_m_edat->st.stat[t]) << 16) +
     (val & 0xFFFFU);
    u ++;
    rrpge_m_edat->st.stat[t - 4U] = u;             /* Write ptr. increment */
    rrpge_m_info.cys += 2U;                        /* 2 stall cycles on the Peripheral bus */

   }

   rrpge_m_edat->st.stat[t] = (rrpge_m_edat->st.stat[t] + 1U) & 0x7FFFU;
   break;

 }
}
