/**
**  \file
**  \brief     Task scheduling
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
*/


#include "rgm_task.h"



/* Checks an offset & size pair if it is within the Data RAM. Returns
** nonzero if it is not within. */
RRPGE_M_FASTCALL auint rrpge_m_task_chkdata(auint off, auint len)
{
 if (len > 0xFFC0U){ return 1; }
 if ( (off < 0x40U) || (off > (0x10000U - len)) ){ return 1; }
 return 0;
}



/* Checks a kernel task for parameter validity. This is used within the
** kernel calls, and also needs to be used outside when validating external
** input (for example starting from a restored state which could be faulty).
** Returns nonzero if the parameters are erratic. It does not write any of
** the data structures. 'n' is the task to check (0-15, unchecked). Returns
** 1 if not valid, 0 otherwise. 'd' is the application state to check. */
auint rrpge_m_taskcheck(uint16 const* d, auint n)
{
 uint16 const* p = &(d[RRPGE_STA_KTASK + (n<<4)]);

 if (p[0xFU] == 0x0000U){ return 0; } /* Empty task is OK */
 if (p[0xFU] >= 0x8000U){ return 0; } /* Finished task is OK */

 switch (p[0]){ /* Process by kernel function */

  case 0x0100U: /* Kernel task: Start loading binary data page */
   if (rrpge_m_task_chkdata(p[1], p[2])){ return 1; } /* Not in Data area */
   if ( (((auint)(p[3]) << 16) + (auint)(p[4]) + (auint)(p[2])) >
        ( ((auint)(d[RRPGE_STA_VARS + 0x18U]) << 16) +
          ((auint)(d[RRPGE_STA_VARS + 0x19U])) ) ){ return 1; } /* Out of data area */
   return 0;

  case 0x0110U: /* Kernel task: Start loading page from file */
   if (rrpge_m_task_chkdata(p[1], (((auint)(p[2]) + 1U) >> 1))){ return 1; } /* Target not in Data area */
   if (rrpge_m_task_chkdata(p[5], p[6])){ return 1; } /* Name not in Data area */
   return 0;

  case 0x0111U: /* Kernel task: Start saving page into file */
   if (rrpge_m_task_chkdata(p[1], (((auint)(p[2]) + 1U) >> 1))){ return 1; } /* Source not in Data area */
   if (rrpge_m_task_chkdata(p[5], p[6])){ return 1; } /* Name not in Data area */
   return 0;

  case 0x0112U: /* Kernel task: Find next file */
   if (rrpge_m_task_chkdata(p[1], p[2])){ return 1; } /* Name not in Data area */
   return 0;

  case 0x0113U: /* Kernel task: Move a file */
   if (rrpge_m_task_chkdata(p[1], p[2])){ return 1; } /* Name not in Data area */
   if (rrpge_m_task_chkdata(p[3], p[4])){ return 1; } /* Name not in Data area */
   return 0;

  case 0x0601U: /* Kernel task: Read UTF-8 representation of user */
   if (rrpge_m_task_chkdata(p[1], p[2])){ return 1; } /* Main part not in Data area */
   if (rrpge_m_task_chkdata(p[3], p[4])){ return 1; } /* Extended part not in Data area */
   if (rrpge_m_task_chkdata(p[5],   8U)){ return 1; } /* User ID not in Data area */
   return 0;

  case 0x0700U: /* Kernel task: Send data to user */
   if (rrpge_m_task_chkdata(p[1], p[2])){ return 1; } /* Source not in Data area */
   if (rrpge_m_task_chkdata(p[3],   8U)){ return 1; } /* User ID not in Data area */
   return 0;

  case 0x0710U: /* Kernel task: List accessible users */
   if (rrpge_m_task_chkdata(p[1], (auint)(p[2]) << 3)){ return 1; } /* Target not in Data area */
   if (rrpge_m_task_chkdata(p[3],   8U)){ return 1; } /* User ID not in Data area */
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
 auint i;
 uint16* tskp;
 rrpge_cbp_loadbin_t   cbp_loadbin;
 rrpge_cbp_load_t      cbp_load;
 rrpge_cbp_save_t      cbp_save;
 rrpge_cbp_next_t      cbp_next;
 rrpge_cbp_move_t      cbp_move;
 rrpge_cbp_getutf_t    cbp_getutf;
 rrpge_cbp_send_t      cbp_send;
 rrpge_cbp_listusers_t cbp_listusers;


 for (i = 0U; i < 16U; i++){
  if ( ((rrpge_m_edat->st.stat[RRPGE_STA_KTASK + 0xFU + (i << 4)]) == 0x0001U) &&
       ((rrpge_m_edat->tsfl & (1U << i)) == 0) ){

   /* Kernel task is waiting to be dispatched, so do it! It's parameter 0
   ** provides the type to select the appropriate host callback. */

   rrpge_m_edat->tsfl |= (1U << i); /* Task is marked running (rrpge_taskend()
                                    ** will clear this or any reset. */

   if (rrpge_m_taskcheck(&(rrpge_m_edat->st.stat[0]), i)){ /* Check if task has proper parameters */
    rrpge_m_info.hlt |= RRPGE_HLT_FAULT;

   }else{

    /* At this point task parameters are valid, so they can not index out of
    ** the emulated machine's memories. It is so safe to use the parameters to
    ** form memory pointers from. */

    tskp = &(rrpge_m_edat->st.stat[RRPGE_STA_KTASK + (i << 4)]);

    switch (tskp[0]){


     case 0x0100U: /* Start loading binary data page */

      cbp_loadbin.buf = &rrpge_m_edat->st.dram[tskp[1]];
      cbp_loadbin.scw = tskp[2];
      cbp_loadbin.sow = ((auint)(tskp[3]) << 16) + (auint)(tskp[4]);
      rrpge_m_edat->cb_tsk[RRPGE_CB_LOADBIN](rrpge_m_edat, i, &cbp_loadbin);

      break;


     case 0x0110U: /* Start loaing page from file */

      cbp_load.buf = &rrpge_m_edat->st.dram[tskp[1]];
      cbp_load.scb = tskp[2];
      cbp_load.sob = ((auint)(tskp[3]) << 16) + (auint)(tskp[4]);
      cbp_load.nam = &rrpge_m_edat->st.dram[tskp[5]];
      cbp_load.ncw = tskp[6];
      rrpge_m_edat->cb_tsk[RRPGE_CB_LOAD](rrpge_m_edat, i, &cbp_load);

      break;


     case 0x0111U: /* Start saving page into file */

      cbp_save.buf = &rrpge_m_edat->st.dram[tskp[1]];
      cbp_save.tcb = tskp[2];
      cbp_save.tob = ((auint)(tskp[3]) << 16) + (auint)(tskp[4]);
      cbp_save.nam = &rrpge_m_edat->st.dram[tskp[5]];
      cbp_save.ncw = tskp[6];
      rrpge_m_edat->cb_tsk[RRPGE_CB_SAVE](rrpge_m_edat, i, &cbp_save);

      break;


     case 0x0112U: /* Find next file */

      cbp_next.nam = &rrpge_m_edat->st.dram[tskp[1]];
      cbp_next.ncw = tskp[2];
      rrpge_m_edat->cb_tsk[RRPGE_CB_NEXT](rrpge_m_edat, i, &cbp_next);

      break;


     case 0x0113U: /* Move a file */

      cbp_move.tnm = &rrpge_m_edat->st.dram[tskp[1]];
      cbp_move.tcw = tskp[2];
      cbp_move.snm = &rrpge_m_edat->st.dram[tskp[3]];
      cbp_move.scw = tskp[4];
      rrpge_m_edat->cb_tsk[RRPGE_CB_MOVE](rrpge_m_edat, i, &cbp_move);

      break;


     case 0x0601U: /* Get UTF-8 representation of User ID */

      cbp_getutf.nam = &rrpge_m_edat->st.dram[tskp[1]];
      cbp_getutf.ncw = tskp[2];
      cbp_getutf.ext = &rrpge_m_edat->st.dram[tskp[3]];
      cbp_getutf.ecw = tskp[4];
      cbp_getutf.id  = &rrpge_m_edat->st.dram[tskp[5]];
      rrpge_m_edat->cb_tsk[RRPGE_CB_GETUTF](rrpge_m_edat, i, &cbp_getutf);

      break;


     case 0x0700U: /* Send data to user */

      cbp_send.buf = &rrpge_m_edat->st.dram[tskp[1]];
      cbp_send.bcw = tskp[2];
      cbp_send.id  = &rrpge_m_edat->st.dram[tskp[3]];
      rrpge_m_edat->cb_tsk[RRPGE_CB_SEND](rrpge_m_edat, i, &cbp_send);

      break;


     case 0x0710U: /* List accessible users */

      cbp_listusers.buf = &rrpge_m_edat->st.dram[tskp[1]];
      cbp_listusers.bcu = tskp[2];
      cbp_listusers.id  = &rrpge_m_edat->st.dram[tskp[3]];
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
