/**
**  \file
**  \brief     Task scheduling
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.10
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
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U)) return 1; /* Not an RW page */
   if ( (((auint)(p[2]) << 16) + (auint)(p[3])) >=
        ( ((((auint)(d[0xBC2U]) & 0xFFU)) << 16) +
          ((auint)(d[0xBC3U])) )
      )                                       return 1; /* Data page does not exist */
   return 0;

  case 0x0110U: /* Kernel task: Start loading page from file */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U)) return 1; /* Target is not an RW page */
   if ((p[5] < 0x4000U) || (p[5] >= 0x40E0U)) return 1; /* File name is not on an RW page */
   return 0;

  case 0x0111U: /* Kernel task: Start saving page into file */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U)) return 1; /* Target is not an RW page */
   if ((p[5] < 0x4000U) || (p[5] >= 0x40E0U)) return 1; /* File name is not on an RW page */
   return 0;

  case 0x0112U: /* Kernel task: Find next file */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U)) return 1; /* File name is not on an RW page */
   return 0;

  case 0x0113U: /* Kernel task: Move a file */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U)) return 1; /* File name is not on an RW page */
   if ((p[3] < 0x4000U) || (p[3] >= 0x40E0U)) return 1; /* File name is not on an RW page */
   return 0;

  case 0x0601U: /* Kernel task: Read UTF-8 representation of user */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U)) return 1; /* Not an RW page */
   return 0;

  case 0x0700U: /* Kernel task: Send data to user */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U)) return 1; /* Not an RW page */
   return 0;

  case 0x0710U: /* Kernel task: List accessible users */
   if ((p[1] < 0x4000U) || (p[1] >= 0x40E0U)) return 1; /* Not an RW page */
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
 auint t2;
 auint t3;
 rrpge_cbp_loadbin_t   cbp_loadbin;
 rrpge_cbp_load_t      cbp_load;
 rrpge_cbp_save_t      cbp_save;
 rrpge_cbp_next_t      cbp_next;
 rrpge_cbp_move_t      cbp_move;
 rrpge_cbp_getutf_t    cbp_getutf;
 rrpge_cbp_send_t      cbp_send;
 rrpge_cbp_listusers_t cbp_listusers;


 for (i = 0U; i < 16U; i++){
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
    ** the emulated machine's memories, or specify an invalid source page (for
    ** loadbin). It is so safe to use the parameters to form memory pointers
    ** from. */

    switch (rrpge_m_edat->stat.ropd[0xD80U + (i << 4)]){


     case 0x0100U: /* Start loading binary data page */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to load into */
      t0 = (t0 & 0xFFU) << 12;                         /* Make offset */
      t1 = ((auint)(rrpge_m_edat->stat.ropd[0xD82U + (i << 4)]) << 16) +
           ((auint)(rrpge_m_edat->stat.ropd[0xD83U + (i << 4)])); /* Page to load */

      cbp_loadbin.spg = t1;
      cbp_loadbin.buf = &rrpge_m_edat->stat.dram[t0];
      rrpge_m_edat->cb_tsk[RRPGE_CB_LOADBIN](rrpge_m_edat, i, &cbp_loadbin);

      break;


     case 0x0110U: /* Start loaing page from file */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to load into */
      t0 = (t0 & 0xFFU) << 12;                         /* Make offset */
      t1 = ((auint)(rrpge_m_edat->stat.ropd[0xD82U + (i << 4)]) << 16) +
           ((auint)(rrpge_m_edat->stat.ropd[0xD83U + (i << 4)])); /* Page to load */
      t2 = rrpge_m_edat->stat.ropd[0xD84U + (i << 4)]; /* Number of bytes to load */
      if (t2 > 8192U){ t2 = 8192U; }                   /* Saturate byte count if needed */
      t3 = rrpge_m_edat->stat.ropd[0xD85U + (i << 4)]; /* Page of file name */
      t3 = (t3 & 0xFFU) << 12;                         /* Make offset */
      t3 = t3 +                                        /* File name start offset got */
           (((auint)(rrpge_m_edat->stat.ropd[0xD86U + (i << 4)])) & 0x0F80U);

      cbp_load.spg = t1;
      cbp_load.sno = t2;
      cbp_load.buf = &rrpge_m_edat->stat.dram[t0];
      cbp_load.nam = &rrpge_m_edat->stat.dram[t3];
      for (j = 0U; j < 4096U; j++){ cbp_load.buf[j] = 0U; }
      rrpge_m_edat->cb_tsk[RRPGE_CB_LOAD](rrpge_m_edat, i, &cbp_load);

      break;


     case 0x0111U: /* Start saving page into file */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to save from */
      t0 = (t0 & 0xFFU) << 12;                         /* Make offset */
      t1 = ((auint)(rrpge_m_edat->stat.ropd[0xD82U + (i << 4)]) << 16) +
           ((auint)(rrpge_m_edat->stat.ropd[0xD83U + (i << 4)])); /* Page to save into */
      t2 = rrpge_m_edat->stat.ropd[0xD84U + (i << 4)]; /* Number of bytes to save */
      if (t2 > 8192U){ t2 = 8192U; }                   /* Saturate byte count if needed */
      t3 = rrpge_m_edat->stat.ropd[0xD85U + (i << 4)]; /* Page of file name */
      t3 = (t3 & 0xFFU) << 12;                         /* Make offset */
      t3 = t3 +                                        /* File name start offset got */
           (((auint)(rrpge_m_edat->stat.ropd[0xD86U + (i << 4)])) & 0x0F80U);

      cbp_save.tpg = t1;
      cbp_save.tno = t2;
      cbp_save.buf = &rrpge_m_edat->stat.dram[t0];
      cbp_save.nam = &rrpge_m_edat->stat.dram[t3];
      rrpge_m_edat->cb_tsk[RRPGE_CB_SAVE](rrpge_m_edat, i, &cbp_save);

      break;


     case 0x0112U: /* Find next file */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page of file name */
      t0 = (t0 & 0xFFU) << 12;                         /* Make offset */
      t0 = t0 +                                        /* File name start offset got */
           (((auint)(rrpge_m_edat->stat.ropd[0xD82U + (i << 4)])) & 0x0F80U);

      cbp_next.nam = &rrpge_m_edat->stat.dram[t0];
      rrpge_m_edat->cb_tsk[RRPGE_CB_NEXT](rrpge_m_edat, i, &cbp_next);

      break;


     case 0x0113U: /* Move a file */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page of file name */
      t0 = (t0 & 0xFFU) << 12;                         /* Make offset */
      t0 = t0 +                                        /* File name start offset got */
           (((auint)(rrpge_m_edat->stat.ropd[0xD82U + (i << 4)])) & 0x0F80U);
      t1 = rrpge_m_edat->stat.ropd[0xD83U + (i << 4)]; /* Page of file name */
      t1 = (t1 & 0xFFU) << 12;                         /* Make offset */
      t1 = t1 +                                        /* File name start offset got */
           (((auint)(rrpge_m_edat->stat.ropd[0xD84U + (i << 4)])) & 0x0F80U);

      cbp_move.snm = &rrpge_m_edat->stat.dram[t1];
      cbp_move.tnm = &rrpge_m_edat->stat.dram[t0];
      rrpge_m_edat->cb_tsk[RRPGE_CB_MOVE](rrpge_m_edat, i, &cbp_move);

      break;


     case 0x0601U: /* Get UTF-8 representation of User ID */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to load into */
      t0 = (t0 & 0xFFU) << 12;                         /* Make offset */
      t0 = t0 +                                        /* Add start offset within page */
           (((auint)(rrpge_m_edat->stat.ropd[0xD82U + (i << 4)])) & 0x0F00U);

      for (j = 0; j < 8U; j++){
       cbp_getutf.id[j] = rrpge_m_edat->stat.ropd[0xD83U + (i << 4) + j];
      }
      cbp_getutf.buf = &rrpge_m_edat->stat.dram[t0];
      for (j = 0; j < 256U; j++){ cbp_getutf.buf[j] = 0U; }
      rrpge_m_edat->cb_tsk[RRPGE_CB_GETUTF](rrpge_m_edat, i, &cbp_getutf);

      break;


     case 0x0700U: /* Send data to user */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to send from */
      t0 = (t0 & 0xFFU) << 12;                         /* Make offset */
      t1 = rrpge_m_edat->stat.ropd[0xD82U + (i << 4)]; /* Count of words to send */
      t1 = ((t1 - 1U) & 0xFFFU) + 1U;

      for (j = 0; j < 8U; j++){
       cbp_send.id[j] = rrpge_m_edat->stat.ropd[0xD83U + (i << 4) + j];
      }
      cbp_send.no  = t1;
      cbp_send.buf = &rrpge_m_edat->stat.dram[t0];

      rrpge_m_edat->cb_tsk[RRPGE_CB_SEND](rrpge_m_edat, i, &cbp_send);

      break;


     case 0x0710U: /* List accessible users */

      t0 = rrpge_m_edat->stat.ropd[0xD81U + (i << 4)]; /* Page to load into */
      t0 = (t0 & 0xFFU) << 12;                         /* Make offset */

      for (j = 0; j < 8U; j++){
       cbp_listusers.id[j] = rrpge_m_edat->stat.ropd[0xD82U + (i << 4) + j];
      }
      cbp_listusers.buf = &rrpge_m_edat->stat.dram[t0];
      for (j = 0; j < 4096U; j++){ cbp_listusers.buf[j] = 0U; }
      rrpge_m_edat->cb_tsk[RRPGE_CB_LISTUSERS](rrpge_m_edat, i, &cbp_listusers);

      break;


     default:      /* Not a valid kernel function for tasks - invalid! (Should not get here) */

      rrpge_m_info.hlt |= RRPGE_HLT_FAULT;
      break;

    }

   }
  }
 }

}
