/**
**  \file
**  \brief     Emulation execution manager (rrpge_run())
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#include "rgm_run.h"
#include "rgm_aud.h"
#include "rgm_vid.h"
#include "rgm_cpua.h"
#include "rgm_cpuo.h"
#include "rgm_prng.h"
#include "rgm_task.h"
#include "rgm_fifo.h"



/* Runs the emulator - Implementation of RRPGE library function. */
rrpge_iuint rrpge_run(rrpge_object_t* hnd, rrpge_iuint rmod)
{
 auint i;
 auint cy;                     /* Cycle counting work variable */
 auint r  = 0U;                /* Return number of cycles */
 auint fo = 1U;                /* Is this the first operation? (For breakpoints) */
 uint16* stat;

 rrpge_m_edat = hnd;
 stat = &(rrpge_m_edat->st.stat[0]);

 /* Check halt causes, break emulation if necessary. */

 if ( ( (rrpge_m_edat->hlt) &
        (RRPGE_HLT_WAIT |      /* Need to finish initialization first */
         RRPGE_HLT_AUDIO |     /* Need servicing audio event first */
         RRPGE_HLT_EXIT |      /* Can not continue */
         RRPGE_HLT_FAULT) )    /* Can not continue */
      != 0U){
  return 0U;
 }
 rrpge_m_info.hlt = 0U;        /* All other halt causes simply clear */

 /* Export Application state data into info structure */

 rrpge_m_info.vln = stat[RRPGE_STA_VARS + 0x10U] & 0xFFFFU;
 rrpge_m_info.vlc = stat[RRPGE_STA_VARS + 0x11U] & 0xFFFFU;
 rrpge_m_info.atc = stat[RRPGE_STA_VARS + 0x13U] & 0xFFFFU;
 rrpge_m_info.cyf[0] = ((stat[RRPGE_STA_VARS + 0x22U] & 0xFFFFU) << 16) +
                       ((stat[RRPGE_STA_VARS + 0x23U] & 0xFFFFU));
 rrpge_m_info.cyf[1] = ((stat[RRPGE_STA_VARS + 0x2AU] & 0xFFFFU) << 16) +
                       ((stat[RRPGE_STA_VARS + 0x2BU] & 0xFFFFU));
 rrpge_m_info.cys = 0U;   /* Stall cycles are always consumed right away (no carry-over between runs) */
 rrpge_m_info.grr = 1U;   /* Reload recolor banks */
 for (i = 0U; i < 8U; i++){
  rrpge_m_info.xr[i] = stat[RRPGE_STA_VARS + 0x00U + i] & 0xFFFFU;
 }
 rrpge_m_info.xmh[0] = stat[RRPGE_STA_VARS + 0x08U] & 0xFFFFU;
 rrpge_m_info.xmh[1] = stat[RRPGE_STA_VARS + 0x09U] & 0xFFFFU;
 rrpge_m_info.pc = stat[RRPGE_STA_VARS + 0x0AU] & 0xFFFFU;
 rrpge_m_info.sp = stat[RRPGE_STA_VARS + 0x0BU] & 0xFFFFU;
 rrpge_m_info.bp = stat[RRPGE_STA_VARS + 0x0CU] & 0xFFFFU;
 if ((stat[RRPGE_STA_VARS + 0x1AU] & 0xFFFFU) == 0U){ /* Separate stack */
  rrpge_m_info.sbt = 0x10000U;
  rrpge_m_info.stp = 0x18000U;
 }else{                   /* Data area stack */
  rrpge_m_info.sbt = (stat[RRPGE_STA_VARS + 0x1BU] & 0xFFFFU);
  rrpge_m_info.stp = (stat[RRPGE_STA_VARS + 0x1AU] & 0xFFFFU) + rrpge_m_info.sbt;
 }


 /* Enter main loop */

 do{

  /* Emulate at most as many cycles as fits in what remains from the current
  ** video line. Random kernel stall is only applied after this: it is
  ** irrelevant if it extends into more video lines. */

  cy = 0U;                     /* Count of emulated cycles */

  if       (rmod == RRPGE_RUN_SINGLE){ /* Single step: Process only one operation */

   rrpge_m_info.opc = rrpge_m_edat->crom[rrpge_m_info.pc & 0xFFFFU];
   cy += rrpge_m_optable[rrpge_m_info.opc >> 9]();  /* Run opcode */

  }else if (rmod == RRPGE_RUN_BREAK){  /* Breakpoint mode: after 1st op, halt on any breakpoints */

   do{
    if (fo != 0U){             /* First operation passed: check for breakpoints. */
     if ( (rrpge_m_edat->brkp[rrpge_m_info.pc >> 5]) &
          (0x80000000U >> (rrpge_m_info.pc & 0x1FU)) ){ /* Breakpoint hit */
      rrpge_m_info.hlt |= RRPGE_HLT_BREAK;
      break;
     }
    }
    rrpge_m_info.opc = rrpge_m_edat->crom[rrpge_m_info.pc & 0xFFFFU];
    cy += rrpge_m_optable[rrpge_m_info.opc >> 9](); /* Run opcode */
    fo = 0U;
    if (rrpge_m_info.hlt != 0U){ break; }           /* Some halt event happened */
   }while (cy <= rrpge_m_info.vlc);

  }else{                               /* Normal mode: just run until halt */

   do{
    rrpge_m_info.opc = rrpge_m_edat->crom[rrpge_m_info.pc & 0xFFFFU];
    cy += rrpge_m_optable[rrpge_m_info.opc >> 9](); /* Run opcode */
    if (rrpge_m_info.hlt != 0U){ break; }           /* Some halt event happened */
   }while (cy <= rrpge_m_info.vlc);

  }

  /* Roll and add kernel internal task cycles if necessary. This is done here
  ** since it needs to produce CPU cycles. */

  if ((rrpge_m_edat->kfc & 0x80000000U) != 0U){ /* Free cycles exhausted */
   i   = (rrpge_m_prng() & 0x7FU) * 100U;
   i  += 400U * 32U;           /* 32 - 64 lines of free time */
   rrpge_m_edat->kfc += i + (i >> 2);
   i >>= 2;
   cy += i;                    /*  8 - 16 lines of kernel time */
                               /* (Above i >> 2 is also added to the free time
                               ** since it will be subtracted later below) */
  }

  /* CPU oriented emulation streak complete, cycles emulated are in cy. Now
  ** asynchronous tasks may start and catch up with the CPU. */

  r += cy;                     /* Sum emulated cycles for return value */
  rrpge_m_edat->kfc -= cy;     /* Free cycles remaining until next kernel takeover */


  /* Process asynchronous tasks which should perform during the consumed
  ** cycles. */

  /* Peripheral processing (FIFO: Mixer and Accelerator) */

  rrpge_m_fifoproc(cy);

  /* Audio processing. This may also raise an audio halt cause indicating an
  ** 512 sample streak. */

  rrpge_m_audproc(cy);

  /* Video processing. Generates display output (including calling the line
  ** callback), and processes Display List clear. */

  rrpge_m_vidproc(cy);


  /* If halt causes were set, break out of the main loop before scheduling
  ** kernel tasks. Note that single stepping mode does not break here since
  ** if it did so, kernel tasks would never be processed. */

  if (rrpge_m_info.hlt != 0){ break; }


  /* Schedule kernel tasks if necessary. */

  rrpge_m_tasksched();


  /* When in single stepping mode, an operation is carried out to this point,
  ** so break out from the loop */

  if (rmod == RRPGE_RUN_SINGLE){ break; }

 }while (1);


 /* Write-back Application state data from info structure */

 stat[RRPGE_STA_VARS + 0x10U] = (rrpge_m_info.vln) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x11U] = (rrpge_m_info.vlc) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x13U] = (rrpge_m_info.atc) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x22U] = (rrpge_m_info.cyf[0] >> 16) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x23U] = (rrpge_m_info.cyf[0]      ) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x2AU] = (rrpge_m_info.cyf[1] >> 16) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x2BU] = (rrpge_m_info.cyf[1]      ) & 0xFFFFU;
 for (i = 0U; i < 8U; i++){
  stat[RRPGE_STA_VARS + 0x00U + i] = (rrpge_m_info.xr[i]) & 0xFFFFU;
 }
 stat[RRPGE_STA_VARS + 0x08U] = (rrpge_m_info.xmh[0]) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x09U] = (rrpge_m_info.xmh[1]) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x0AU] = (rrpge_m_info.pc) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x0BU] = (rrpge_m_info.sp) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x0CU] = (rrpge_m_info.bp) & 0xFFFFU;

 rrpge_m_edat->hlt = rrpge_m_info.hlt; /* Update halt cause */

 /* OK, all done, return consumed cycles */

 return r;

}
