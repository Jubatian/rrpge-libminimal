/**
**  \file
**  \brief     Emulation execution manager (rrpge_run())
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.13
*/


#include "rgm_run.h"
#include "rgm_aud.h"
#include "rgm_vid.h"
#include "rgm_prng.h"
#include "rgm_task.h"
#include "rgm_fifo.h"
#include "rgm_halt.h"
#include "rgm_cpu.h"
#include "rgm_pram.h"



/* Runs the emulator - Implementation of RRPGE library function. */
rrpge_iuint rrpge_run(rrpge_object_t* hnd, rrpge_iuint rmod)
{
 auint i;
 auint cy;                     /* Cycle counting work variable */
 auint r  = 0U;                /* Return number of cycles */
 uint16* stat;

 rrpge_m_edat = hnd;
 stat = &(rrpge_m_edat->st.stat[0]);

 /* Check halt causes, break emulation if necessary. */

 if (rrpge_m_halt_isset(hnd,
        RRPGE_HLT_WAIT |       /* Need to finish initialization first */
        RRPGE_HLT_AUDIO |      /* Need servicing audio event first */
        RRPGE_HLT_EXIT |       /* Can not continue */
        RRPGE_HLT_FAULT        /* Can not continue */
    )){
  return 0U;
 }
 rrpge_m_halt_clrall(hnd);     /* All other halt causes simply clear */

 /* Export Application state data into info structure */

 rrpge_m_info.atc = stat[RRPGE_STA_VARS + 0x13U] & 0xFFFFU;
 rrpge_m_info.cyf[0] = ((stat[RRPGE_STA_VARS + 0x22U] & 0xFFFFU) << 16) +
                       ((stat[RRPGE_STA_VARS + 0x23U] & 0xFFFFU));
 rrpge_m_info.cyf[1] = ((stat[RRPGE_STA_VARS + 0x2AU] & 0xFFFFU) << 16) +
                       ((stat[RRPGE_STA_VARS + 0x2BU] & 0xFFFFU));
 rrpge_m_pram_cys_clr(hnd);    /* Stall cycles are always consumed right away (no carry-over between runs) */
 rrpge_m_info.grr = 1U;        /* Reload recolor banks */


 /* Enter main loop */

 do{

  /* Emulate at most as many cycles as fits in what remains from the current
  ** video line. Random kernel stall is only applied after this: it is
  ** irrelevant if it extends into more video lines. */

  cy = rrpge_m_cpu_run(hnd, rmod, rrpge_m_vid_getremcy(hnd));

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

  rrpge_m_vid_proc(hnd, cy);


  /* If halt causes were set, break out of the main loop before scheduling
  ** kernel tasks. Note that single stepping mode does not break here since
  ** if it did so, kernel tasks would never be processed. */

  if (rrpge_m_halt_isany(hnd)){ break; }


  /* Schedule kernel tasks if necessary. */

  rrpge_m_tasksched();


  /* When in single stepping mode, an operation is carried out to this point,
  ** so break out from the loop */

  if (rmod == RRPGE_RUN_SINGLE){ break; }

 }while (1);


 /* Write-back Application state data from info structure */

 stat[RRPGE_STA_VARS + 0x13U] = (rrpge_m_info.atc) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x22U] = (rrpge_m_info.cyf[0] >> 16) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x23U] = (rrpge_m_info.cyf[0]      ) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x2AU] = (rrpge_m_info.cyf[1] >> 16) & 0xFFFFU;
 stat[RRPGE_STA_VARS + 0x2BU] = (rrpge_m_info.cyf[1]      ) & 0xFFFFU;

 /* OK, all done, return consumed cycles */

 return r;

}
