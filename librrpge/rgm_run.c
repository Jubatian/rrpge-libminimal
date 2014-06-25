/**
**  \file
**  \brief     Emulation execution manager (rrpge_run())
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.06.25
*/


#include "rgm_run.h"
#include "rgm_aud.h"
#include "rgm_cpua.h"
#include "rgm_cpuo.h"
#include "rgm_grln.h"
#include "rgm_grcy.h"
#include "rgm_grop.h"
#include "rgm_prng.h"
#include "rgm_task.h"



/* Runs the emulator - Implementation of RRPGE library function. */
rrpge_uint32 rrpge_run(rrpge_object_t* hnd, rrpge_uint32 rmod)
{
 auint i;
 auint cy;                     /* Cycle counting work variable */
 auint cm;                     /* Cycle limit for inner loop */
 auint r  = 0U;                /* Return number of cycles */
 auint fo = 1U;                /* Is this the first operation? (For breakpoints) */

 rrpge_m_edat = hnd;

 /* Check halt causes, break emulation if necessary. */

 if ( (rrpge_m_edat->hlt) & (
      RRPGE_HLT_AUDIO |        /* Need servicing audio event first */
      RRPGE_HLT_EXIT |         /* Can not continue */
      RRPGE_HLT_FAULT) ){      /* Can not continue */
  return 0;
 }
 rrpge_m_info.hlt = 0;         /* All other halt causes simply clear */

 /* Export ROPD data into info structure */

 for (i=0; i<16; i++){
  rrpge_m_info.rbk[i] = (auint)(rrpge_m_edat->stat.ropd[0xD00U + i]) << 12;
  rrpge_m_info.wbk[i] = (auint)(rrpge_m_edat->stat.ropd[0xD10U + i]) << 12;
 }
 rrpge_m_info.vln = rrpge_m_edat->stat.ropd[0xD50U];
 rrpge_m_info.vlc = rrpge_m_edat->stat.ropd[0xD51U];
 rrpge_m_info.auc = rrpge_m_edat->stat.ropd[0xD53U];
 rrpge_m_info.vac = ((auint)(rrpge_m_edat->stat.ropd[0xD54U]) << 16) +
                    ((auint)(rrpge_m_edat->stat.ropd[0xD55U]));
 rrpge_m_info.grr = 1U;   /* Reload recolor banks */
 if ((rrpge_m_edat->stat.ropd[0xD57U] & 0xFFFFU) != 1U){ /* 4bit mode */
  rrpge_m_info.vbm = 0x0FU;
 }else{                   /* 8bit mode */
  rrpge_m_info.vbm = 0xFFU;
 }
 for (i=0; i<8; i++){
  rrpge_m_info.xr[i] = rrpge_m_edat->stat.ropd[0xD40U + i];
 }
 rrpge_m_info.xmh[0] = rrpge_m_edat->stat.ropd[0xD48U];
 rrpge_m_info.xmh[1] = rrpge_m_edat->stat.ropd[0xD49U];
 rrpge_m_info.pc = rrpge_m_edat->stat.ropd[0xD4AU];
 rrpge_m_info.sp = rrpge_m_edat->stat.ropd[0xD4BU];
 rrpge_m_info.bp = rrpge_m_edat->stat.ropd[0xD4CU];

 rrpge_m_info.arq = 0; /* No accelerator function is waiting */


 /* Enter main loop */

 do{

  /* Emulate at most as many cycles as fits in what remains from the current
  ** video line. Random kernel stall is only applied after this: it is
  ** irrelevant if it extends into more video lines (just as DMA could span
  ** multiple lines this way) */

  cy = 0U;                     /* Count of emulated cycles */

  if       (rmod == RRPGE_RUN_SINGLE){ /* Single step: Process only one operation */

   rrpge_m_info.opc = rrpge_m_edat->crom[rrpge_m_info.pc];
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
    rrpge_m_info.opc = rrpge_m_edat->crom[rrpge_m_info.pc];
    cy += rrpge_m_optable[rrpge_m_info.opc >> 9](); /* Run opcode */
    fo = 0U;
    if ( ( (rrpge_m_info.hlt) |                     /* Some halt event happened */
           (rrpge_m_info.arq) ) != 0U){ break; }    /* Graphics accelerator request */
   }while (cy <= rrpge_m_info.vlc);

  }else{                               /* Normal mode: just run until halt */

   do{
    rrpge_m_info.opc = rrpge_m_edat->crom[rrpge_m_info.pc];
    cy += rrpge_m_optable[rrpge_m_info.opc >> 9](); /* Run opcode */
    if ( ( (rrpge_m_info.hlt) |                     /* Some halt event happened */
           (rrpge_m_info.arq) ) != 0) break;        /* Graphics accelerator request */
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

  /* Audio processing. This may also raise an audio halt cause indicating an
  ** 512 sample streak. */

  rrpge_m_audproc(cy);





  /* Apply consumed cycles. During this audio and video interrupts might be
  ** flagged and video lines may be rendered if necessary. */


  /* Render video lines as needed */

  if ((rrpge_m_info.vlc + cy) >= 400U){ /* At least one video line passed */
   i = cy - (400U - rrpge_m_info.vlc);

   while (1){

    rrpge_m_info.vln ++;
    if (rrpge_m_info.vln >= RRPGE_M_VLN){ rrpge_m_info.vln = 0; }

    /* Part of the randomizing hack. The line may render at either the
    ** beginning or the end of the line's cycles, this case the end. */
    if ((rrpge_m_edat->rena) & 0x2U){   /* Rendering is enabled */
     if ((rrpge_m_edat->frld) != 0U){   /* Previous line was not rendered */
      rrpge_m_grln();
     }
    }

    /* If no forward rendering was performed earlier, render the line */
    if (rrpge_m_info.vln != (rrpge_m_edat->frln)){
     rrpge_m_grpr();                    /* Initialize for rendering */
    }

    /* Render current line. A randomizing hack is placed here which either
    ** renders the line at the beginning or the end of the line's emulated
    ** cycles likely causing noticable flicker in applications attempting to
    ** rely on this unspecified behaviour (that is writing in the fetched
    ** line). If the random passes, this is the render at begin case. */
    if ((rrpge_m_edat->rena) & 0x2U){   /* Rendering is enabled */
     if ((rrpge_m_edat->frld) != 0U){   /* There is a line to render? */
      if (rrpge_m_prng() & 1U){         /* If the dice says so, render! */
       rrpge_m_grln();
      }
     }
    }

    /* Process synchronous (to emulation) graphics logic */
    if (rrpge_m_info.vln >= 400U){      /* In VBlank */
     if (rrpge_m_info.vln == 400U){     /* Just entered */
      if (rrpge_m_edat->stat.ropd[0xD20U] >= 400U){ /* VBlank interrupt */
       rrpge_m_edat->stat.ropd[0xD6EU] |= 0x1U;
      }
     }
    }else{                     /* In display area */
     if (rrpge_m_edat->stat.ropd[0xD20U] == rrpge_m_info.vln){ /* Display line interrupt */
      rrpge_m_edat->stat.ropd[0xD6EU] |= 0x1U;
     }
     /* Update the ROPD display list pointers */
     rrpge_m_edat->stat.ropd[0xD58U] = (uint16)(rrpge_m_edat->frep[(rrpge_m_info.vln << 2) + 4U] >> 16);
     rrpge_m_edat->stat.ropd[0xD59U] = (uint16)(rrpge_m_edat->frep[(rrpge_m_info.vln << 2) + 4U]);
     rrpge_m_edat->stat.ropd[0xD5AU] = (uint16)(rrpge_m_edat->frep[(rrpge_m_info.vln << 2) + 5U] >> 16);
     rrpge_m_edat->stat.ropd[0xD5BU] = (uint16)(rrpge_m_edat->frep[(rrpge_m_info.vln << 2) + 5U]);
     rrpge_m_edat->stat.ropd[0xD5CU] = (uint16)(rrpge_m_edat->frep[(rrpge_m_info.vln << 2) + 6U] >> 16);
     rrpge_m_edat->stat.ropd[0xD5DU] = (uint16)(rrpge_m_edat->frep[(rrpge_m_info.vln << 2) + 6U]);
     rrpge_m_edat->stat.ropd[0xD5EU] = (uint16)(rrpge_m_edat->frep[(rrpge_m_info.vln << 2) + 7U] >> 16);
     rrpge_m_edat->stat.ropd[0xD5FU] = (uint16)(rrpge_m_edat->frep[(rrpge_m_info.vln << 2) + 7U]);
    }

    if (i < 400U) break;       /* Consume lines & loop */
    i -= 400U;
   }

   rrpge_m_info.vlc = i;
  }else{
   rrpge_m_info.vlc += cy;
  }


  /* If a graphic accelerator function was triggered, process it with the
  ** random forward rendering beforehands (the exact completion of the
  ** accelerator relative to the display output is unspecified, when designing
  ** race with the beam style code it may be common to accidentally hit this
  ** behaviour. This will uncover such glitches for the developer). */

  if (rrpge_m_info.arq != 0U){

   rrpge_m_graccy();           /* Calculates cycle requirements in rrpge_m_info.vac */

   if (rrpge_m_prng() & 1U){   /* Randomly do a render forward */
    /* Render "leftover" line if any */
    if ((rrpge_m_edat->rena) & 0x2U){   /* Rendering is enabled */
     if ((rrpge_m_edat->frld) != 0U){   /* Previous line was not rendered */
      rrpge_m_grln();
     }
    }
    /* Now pass over all lines the acc. operation spans and forward render
    ** them. */
    i = rrpge_m_info.vac;
    if (i >= 400U - rrpge_m_info.vlc){  /* At least one line */
     i -= 400U - rrpge_m_info.vlc;
    }
    while (i >= 400U){
     rrpge_m_grpr();
     if ((rrpge_m_edat->rena) & 0x2U){  /* rrpge_m_grpr() might turn it on / off */
      rrpge_m_grln();
     }
     i -= 400U;
    }
   }

   rrpge_m_grop_accel();       /* Run accelerator function */
   rrpge_m_info.arq = 0U;
  }


  /* If halt causes were set, break out of the main loop before scheduling
  ** kernel tasks. Note that single stepping mode does not break here since
  ** if it did so, kernel tasks would never be processed. */

  if (rrpge_m_info.hlt != 0) break;


  /* Schedule kernel tasks if necessary. */

  rrpge_m_tasksched();


  /* When in single stepping mode, an operation is carried out to this point,
  ** so break out from the loop */

  if (rmod == RRPGE_RUN_SINGLE) break;

 }while (1);


 /* Write-back ROPD data from info structure */

 for (i=0; i<16; i++){
  rrpge_m_edat->stat.ropd[0xD00U + i] = (uint16)(rrpge_m_info.rbk[i] >> 12);
  rrpge_m_edat->stat.ropd[0xD10U + i] = (uint16)(rrpge_m_info.wbk[i] >> 12);
 }
 rrpge_m_edat->stat.ropd[0xD50U] = rrpge_m_info.vln;
 rrpge_m_edat->stat.ropd[0xD51U] = rrpge_m_info.vlc;
 rrpge_m_edat->stat.ropd[0xD53U] = rrpge_m_info.auc;
 rrpge_m_edat->stat.ropd[0xD54U] = (uint16)(rrpge_m_info.vac >> 16);
 rrpge_m_edat->stat.ropd[0xD55U] = (uint16)(rrpge_m_info.vac);
 for (i=0; i<8; i++){
  rrpge_m_edat->stat.ropd[0xD40U + i] = rrpge_m_info.xr[i];
 }
 rrpge_m_edat->stat.ropd[0xD48U] = rrpge_m_info.xmh[0];
 rrpge_m_edat->stat.ropd[0xD49U] = rrpge_m_info.xmh[1];
 rrpge_m_edat->stat.ropd[0xD4AU] = rrpge_m_info.pc;
 rrpge_m_edat->stat.ropd[0xD4BU] = rrpge_m_info.sp;
 rrpge_m_edat->stat.ropd[0xD4CU] = rrpge_m_info.bp;

 rrpge_m_edat->hlt = rrpge_m_info.hlt; /* Update halt cause */

 /* OK, all done, return consumed cycles */

 return r;

}

