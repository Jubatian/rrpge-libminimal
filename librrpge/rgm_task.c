/**
**  \file
**  \brief     Task scheduling
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.27
*/


#include "rgm_task.h"



/* Checks a kernel task for parameter validity. This is used within the
** kernel calls, and also needs to be used outside when validating external
** input (for example starting from a restored state which could be faulty).
** Returns nonzero if the parameters are erratic. It does not write any of
** the data structures. 'n' is the task to check (0-15, unchecked). Returns
** 1 if not valid, 0 otherwise. */
auint rrpge_m_taskcheck(uint16 const* d, auint n)
{
 uint16 const* p = &(d[(n<<4) + 0xD80U]);

 if (p[0xFU] == 0x0000U) return 0; /* Empty task is OK */
 if (p[0xFU] >= 0x8000U) return 0; /* Finished task is OK */

 switch (p[0]){ /* Process by kernel function */

  case 0x0100U: /* Kernel task: Start loading binary data page */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U))     return 1; /* Not an RW page */
   if ( (((auint)(p[2]) << 16) + (auint)(p[3])) >=
        ( ((((auint)(d[0xBC2U]) & 0xFFU)) << 16) +
          ((auint)(d[0xBC3U])) )
      )                                           return 1; /* Data page does not exist */
   return 0;

  case 0x0110U: /* Kernel task: Start loading nonvolatile save */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U))     return 1; /* Not an RW page */
   if (((auint)(p[1]) + (auint)(p[2])) > 0x40E0U) return 1; /* Too many pages */
   return 0;

  case 0x0111U: /* Kernel task: Start saving nonvolatile save */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U))     return 1; /* Not an RW page */
   if (((auint)(p[1]) + (auint)(p[2])) > 0x40E0U) return 1; /* Too many pages */
   return 0;

  case 0x0112U: /* Kernel task: List available NV saves */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U))     return 1; /* Not an RW page */
   return 0;

  case 0x0601U: /* Kernel task: Read UTF-8 representation of user */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U))     return 1; /* Not an RW page */
   return 0;

  case 0x0700U: /* Kernel task: Send data to user */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U))     return 1; /* Not an RW page */
   return 0;

  case 0x0710U: /* Kernel task: List accessible users */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U))     return 1; /* Not an RW page */
   return 0;

  default:      /* Not a valid kernel function for tasks - invalid! */
   return 1;

 }
}




/* Schedule kernel tasks if any is waiting to be started. This includes all
** preparatory actions for the particular task (such as clearing memories),
** calling the appropriate handler (callback), and setting task state
** indicating it is scheduled. Technically this is a part of rrpge_run(),
** excepts rrpge_m_info to be set up, it is just seperated for manageability.
** It generates RRPGE_HLT_FAULT causes if it fails (shouldn't happen). */
void rrpge_m_tasksched(void)
{
 auint i, j;
 auint t0;
 auint t1;
 rrpge_cbp_loadbin_t  cbp_loadbin;
 rrpge_cbp_loadnv_t   cbp_loadnv;
 rrpge_cbp_savenv_t   cbp_savenv;
 rrpge_cbp_listnv_t   cbp_listnv;
 rrpge_cbp_getuutf_t  cbp_getuutf;
 rrpge_cbp_sendto_t   cbp_sendto;
 rrpge_cbp_listuser_t cbp_listuser;


 for (i=0; i<16; i++){
  if ( ((rrpge_m_edat->stat.ropd[0xD8FU + (i << 4)]) == 0x0001U) &&
       ((rrpge_m_edat->tsfl & (1U << i)) == 0) ){

   /* Kernel task is waiting to be dispatched, so do it! It's parameter 0
   ** provides the type to select the appropriate host callback. */

   rrpge_m_edat->tsfl |= (1U << i); /* Task is marked running (rrpge_taskend()
                                    ** will clear this or any reset. */

   if (rrpge_m_taskcheck(&(rrpge_m_edat->stat.ropd[0]), i)){ /* Check if task has proper parameters */
    rrpge_m_info.hlt |= RRPGE_HLT_FAULT;

   }else{

    /* At this point task parameters are valid, so they can not index out of
    ** the emulated machine's memories, or specify too large sizes. It is so
    ** safe to use the parameters to form memory pointers from. */

    switch (rrpge_m_edat->stat.ropd[0xD80U + (i << 4)]){


     case 0x0100U: /* Start loading binary data page */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to load into */
      t0 = t0 & 0xFFU;
      t1 = ((auint)(rrpge_m_edat->stat.ropd[0xD82U + (i << 4)]) << 16) +
           ((auint)(rrpge_m_edat->stat.ropd[0xD83U + (i << 4)])); /* Page to load */

      cbp_loadbin.spg  = t1;
      cbp_loadbin.buf  = &rrpge_m_edat->stat.dram[t0 << 12];
      rrpge_m_edat->cb_tsk[RRPGE_CB_LOADBIN](rrpge_m_edat, i, &cbp_loadbin);

      break;


     case 0x0110U: /* Start loading nonvolatile save */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to load into */
      t0 = t0 & 0xFFU;
      t1 = rrpge_m_edat->stat.ropd[0xD82U + (i << 4)]; /* Pages to use */

      cbp_loadnv.id[0] = rrpge_m_edat->stat.ropd[0xD83U + (i << 4)];
      cbp_loadnv.id[1] = rrpge_m_edat->stat.ropd[0xD84U + (i << 4)];
      cbp_loadnv.id[2] = rrpge_m_edat->stat.ropd[0xD85U + (i << 4)];
      cbp_loadnv.no    = t1;
      cbp_loadnv.buf   = &rrpge_m_edat->stat.dram[t0 << 12];

      for (j = 0; j < (t1 << 12); j++){
       cbp_loadnv.buf[j] = 0;
      }

      rrpge_m_edat->cb_tsk[RRPGE_CB_LOADNV](rrpge_m_edat, i, &cbp_loadnv);

      break;


     case 0x0111U: /* Start saving nonvolatile save */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to save from */
      t0 = t0 & 0xFFU;
      t1 = rrpge_m_edat->stat.ropd[0xD82U + (i << 4)]; /* Pages to use */

      cbp_savenv.id[0] = rrpge_m_edat->stat.ropd[0xD83U + (i << 4)];
      cbp_savenv.id[1] = rrpge_m_edat->stat.ropd[0xD84U + (i << 4)];
      cbp_savenv.id[2] = rrpge_m_edat->stat.ropd[0xD85U + (i << 4)];
      cbp_savenv.no    = t1;
      cbp_savenv.buf   = &rrpge_m_edat->stat.dram[t0 << 12];

      rrpge_m_edat->cb_tsk[RRPGE_CB_SAVENV](rrpge_m_edat, i, &cbp_savenv);

      break;


     case 0x0112U: /* List available nonvolatile saves */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to load main data into */
      t0 = t0 & 0xFFU;

      cbp_listnv.buf   = &rrpge_m_edat->stat.dram[t0 << 12];

      for (j = 0; j < 4096U; j++){
       cbp_listnv.buf[j] = 0;
      }

      rrpge_m_edat->cb_tsk[RRPGE_CB_LISTNV](rrpge_m_edat, i, &cbp_listnv);

      break;


     case 0x0601U: /* Read UTF-8 representation of user */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to load into */
      t0 = t0 & 0xFFU;
      t1 = rrpge_m_edat->stat.ropd[0xD82U + (i << 4)]; /* Start offset within page */
      t1 = t1 & 0x0F00U;

      for (j = 0; j < 8U; j++){
       cbp_getuutf.id[j] = rrpge_m_edat->stat.ropd[0xD83U + (i << 4) + j];
      }
      cbp_getuutf.buf  = &rrpge_m_edat->stat.dram[(t0 << 12) + t1];

      for (j = 0; j < 256U; j++){
       cbp_getuutf.buf[j] = 0;
      }

      rrpge_m_edat->cb_tsk[RRPGE_CB_GETUUTF](rrpge_m_edat, i, &cbp_getuutf);

      break;


     case 0x0700U: /* Send data to user */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to send from */
      t0 = t0 & 0xFFU;
      t1 = rrpge_m_edat->stat.ropd[0xD82U + (i << 4)]; /* Count of words to send */
      t1 = ((t1 - 1U) & 0xFFFU) + 1U;

      for (j = 0; j < 8U; j++){
       cbp_sendto.id[j] = rrpge_m_edat->stat.ropd[0xD83U + (i << 4) + j];
      }
      cbp_sendto.no    = t1;
      cbp_sendto.buf   = &rrpge_m_edat->stat.dram[t0 << 12];

      rrpge_m_edat->cb_tsk[RRPGE_CB_SENDTO](rrpge_m_edat, i, &cbp_sendto);

      break;


     case 0x0710U: /* Kernel task: List accessible users */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to load into */
      t0 = t0 & 0xFFU;

      for (j = 0; j < 8U; j++){
       cbp_listuser.id[j] = rrpge_m_edat->stat.ropd[0xD82U + (i << 4) + j];
      }
      cbp_listuser.buf = &rrpge_m_edat->stat.dram[t0 << 12];

      for (j = 0; j < 4096U; j++){
       cbp_listuser.buf[j] = 0;
      }

      rrpge_m_edat->cb_tsk[RRPGE_CB_LISTUSER](rrpge_m_edat, i, &cbp_listuser);

      break;


     default:      /* Not a valid kernel function for tasks - invalid! (Should not get here) */

      rrpge_m_info.hlt |= RRPGE_HLT_FAULT;
      break;

    }

   }
  }
 }

}
