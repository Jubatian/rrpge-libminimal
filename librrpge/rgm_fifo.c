/**
**  \file
**  \brief     FIFO (Accelerator, Mixer) related operations.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.15
**
**
**  Contains the Graphics & Mixer FIFO's logic. The FIFOs operate detached
**  from the graphics & audio emulation as such precision is not required,
**  however they necessarily contain the logic to interact with the
**  Accelerator and Mixer, and each other regarding stalls.
*/


#include "rgm_fifo.h"
#include "rgm_acc.h"
#include "rgm_mix.h"
#include "rgm_pram.h"
#include "rgm_stat.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* Internal function to update an Accelerator register by address, also
** triggering the operation if necessary. */
RRPGE_M_FASTCALL static void rrpge_m_fifoacc(auint adr, auint val)
{
 if ((adr & 0x100U) == 0U){
  rrpge_m_stat_write(rrpge_m_edat, RRPGE_STA_ACC   + (adr & 0x1FU), val);
  if ((adr & 0x1FU) == 0x1FU){ /* Trigger */
   rrpge_m_info.cyf[1] += rrpge_m_acc_op(rrpge_m_edat);
   rrpge_m_edat->st.stat[RRPGE_STA_UPA_GF + 1U] |= 1U; /* FIFO / Peripheral working */
  }
 }else{
  rrpge_m_stat_write(rrpge_m_edat, RRPGE_STA_REIND + (adr & 0xFFU), val);
 }
}



/* Internal function to update a Mixer register by address, also triggering
** the operation if necessary. */
RRPGE_M_FASTCALL static void rrpge_m_fifomix(auint adr, auint val)
{
 rrpge_m_edat->st.stat[RRPGE_STA_MIXER + (adr & 0xFU)] = val & 0xFFFFU;
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

 cy = rrpge_m_pram_cys_cons(rrpge_m_edat, cy);

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

    if ( (rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x20U + (i << 3)] & 0xFFFFU) ==
         (rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x21U + (i << 3)] & 0xFFFFU) ){ /* FIFO drained */

     rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + 1U + (i << 2)] &= ~1U; /* Empty, peripheral idle */

    }else{                       /* There is data in the FIFO */

     if ((rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + 1U + (i << 2)] & 2U) == 0U){  /* Not suspended */

      t = rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x21U + (i << 3)];
      v = rrpge_m_edat->st.pram[rrpge_m_fifoadr(t, rrpge_m_edat->st.stat[RRPGE_STA_UPA_MF + 0x0U + (i << 2)])];
      t ++;
      rrpge_m_edat->st.stat[RRPGE_STA_VARS + 0x21U + (i << 3)] = t & 0xFFFFU;
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
 uint16* stat = &(rrpge_m_edat->st.stat[0]);

 adr = adr & 7U;

 switch (adr & 0x3U){

  case 0U:                    /* FIFO location & size */

   stat[RRPGE_STA_UPA_MF + adr] = val & 0xFFFFU;
   break;

  case 1U:                    /* FIFO status flags */

   break;

  case 2U:                    /* FIFO address latch */

   t = RRPGE_STA_VARS + 0x24U + ((adr & 4U) << 1);
   if ((val & 0x8000U) != 0U){
    stat[t] = (stat[t] + 1U) & 0x7FFFU;
   }else{
    stat[t] = val & 0x7FFFU;
   }
   break;

  default:                    /* Store trigger */

   t = RRPGE_STA_VARS + 0x24U + ((adr & 4U) << 1);  /* Address latch's address in state */

   if ( (rrpge_m_info.cyf[(adr >> 2)] |
         stat[RRPGE_STA_UPA_MF + adr - 2U]) == 0U){ /* Bypass */

    u = stat[t] & 0xFFFFU;    /* Read the address latch */
    if ((adr & 4U) == 0U){ rrpge_m_fifomix(u, val); }
    else                 { rrpge_m_fifoacc(u, val); }

   }else{                     /* No bypass */

    u = stat[t - 4U] & 0xFFFFU;  /* Write pointer value */
    p = stat[RRPGE_STA_UPA_MF + adr - 3U] & 0xFFFFU; /* FIFO position & size */
    rrpge_m_edat->st.pram[rrpge_m_fifoadr(u, p)] =
     ((stat[t] & 0xFFFFU) << 16) + (val & 0xFFFFU);
    u ++;
    stat[t - 4U] = u & 0xFFFFU;  /* Write ptr. increment */
    rrpge_m_pram_cys_add(rrpge_m_edat, 2U); /* 2 stall cycles on the Peripheral bus */

   }

   stat[t] = (stat[t] + 1U) & 0x7FFFU;
   break;

 }
}
