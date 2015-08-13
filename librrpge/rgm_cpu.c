/**
**  \file
**  \brief     CPU emulation main
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.13
*/


#include "rgm_cpu.h"
#include "rgm_cpuo.h"
#include "rgm_halt.h"
#include "rgm_stat.h"



/* State: CPU registers (0x040 - 0x04F), reader */
RRPGE_M_FASTCALL static auint rrpge_m_cpu_stat_read(rrpge_object_t* hnd, auint adr, auint rmw)
{
 switch (adr){
  case 0x00U:
  case 0x01U:
  case 0x02U:
  case 0x03U:
  case 0x04U:
  case 0x05U:
  case 0x06U:
  case 0x07U: return (hnd->cpu.xr[adr]) & 0xFFFFU; break;
  case 0x08U: return (hnd->cpu.xmb[0]) & 0xFFFFU;  break;
  case 0x09U: return (hnd->cpu.xmb[1]) & 0xFFFFU;  break;
  case 0x0AU: return (hnd->cpu.pc) & 0xFFFFU;      break;
  case 0x0BU: return (hnd->cpu.sp) & 0xFFFFU;      break;
  case 0x0CU: return (hnd->cpu.bp) & 0xFFFFU;      break;
  default:    return 0U;                           break;
 }
}



/* State: CPU registers (0x040 - 0x04F), writer */
RRPGE_M_FASTCALL static void rrpge_m_cpu_stat_write(rrpge_object_t* hnd, auint adr, auint val)
{
 switch (adr){
  case 0x00U:
  case 0x01U:
  case 0x02U:
  case 0x03U:
  case 0x04U:
  case 0x05U:
  case 0x06U:
  case 0x07U: hnd->cpu.xr[adr] = val & 0xFFFFU; break;
  case 0x08U: hnd->cpu.xmb[0] = val & 0xFFFFU;  break;
  case 0x09U: hnd->cpu.xmb[1] = val & 0xFFFFU;  break;
  case 0x0AU: hnd->cpu.pc = val & 0xFFFFU;      break;
  case 0x0BU: hnd->cpu.sp = val & 0xFFFFU;      break;
  case 0x0CU: hnd->cpu.bp = val & 0xFFFFU;      break;
  default:                                      break;
 }
}



/* Initializes CPU emulation adding the appropriate handlers to the state
** manager. */
void rrpge_m_cpu_init(void)
{
 rrpge_m_stat_add_rw_handler(&rrpge_m_cpu_stat_read, &rrpge_m_cpu_stat_write,
                             RRPGE_STA_VARS + 0x00U, 16U);
}



/* Run CPU emulation for up to a given amount of cycles using the given mode.
** Running may finish prematurely if hitting a halt cause. Returns the number
** of cycles emulated. The "rmod" parameter is the run mode passed to
** rrpge_run(). */
auint rrpge_m_cpu_run(rrpge_object_t* hnd, auint rmod, auint cymax)
{
 auint cy = 0U; /* Count of emulated cycles */
 auint fo = 1U; /* Is this the first operation? (For breakpoints) */

 /* Retrieve stack configuration */

 if (rrpge_m_stat_get(hnd, RRPGE_STA_VARS + 0x1AU) == 0U){ /* Separate stack */
  hnd->cpu.sbt = 0x10000U;
  hnd->cpu.stp = 0x18000U;
 }else{                   /* Data area stack */
  hnd->cpu.sbt = rrpge_m_stat_get(hnd, RRPGE_STA_VARS + 0x1BU);
  hnd->cpu.stp = rrpge_m_stat_get(hnd, RRPGE_STA_VARS + 0x1AU) + (hnd->cpu.sbt);
 }

 /* Run */

 if       (rmod == RRPGE_RUN_SINGLE){ /* Single step: Process only one operation */

  hnd->cpu.opc = hnd->crom[hnd->cpu.pc & 0xFFFFU];
  cy += rrpge_m_optable[hnd->cpu.opc >> 9](hnd);  /* Run opcode */

 }else if (rmod == RRPGE_RUN_BREAK){  /* Breakpoint mode: after 1st op, halt on any breakpoints */

  do{
   if (fo != 0U){                     /* First operation passed: check for breakpoints. */
    if ( (hnd->brkp[hnd->cpu.pc >> 5]) &
         (0x80000000U >> (hnd->cpu.pc & 0x1FU)) ){ /* Breakpoint hit */
     rrpge_m_halt_set(hnd, RRPGE_HLT_BREAK);
     break;
    }
   }
   hnd->cpu.opc = hnd->crom[hnd->cpu.pc & 0xFFFFU];
   cy += rrpge_m_optable[hnd->cpu.opc >> 9](hnd); /* Run opcode */
   fo = 0U;
   if (rrpge_m_halt_isany(hnd)){ break; } /* Some halt event happened */
  }while (cy <= cymax);

 }else{                               /* Normal mode: just run until halt */

  do{
   hnd->cpu.opc = hnd->crom[hnd->cpu.pc & 0xFFFFU];
   cy += rrpge_m_optable[hnd->cpu.opc >> 9](hnd); /* Run opcode */
   if (rrpge_m_halt_isany(hnd)){ break; } /* Some halt event happened */
  }while (cy <= cymax);

 }

 return cy;
}
