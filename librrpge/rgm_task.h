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


#ifndef RRPGE_M_TASK_H
#define RRPGE_M_TASK_H


#include "rgm_info.h"


/* Checks an offset & size pair if it is within the Data RAM. Returns
** nonzero if it is not within. */
RRPGE_M_FASTCALL auint rrpge_m_task_chkdata(auint off, auint len);


/* Checks a kernel task for parameter validity. This is used within the
** kernel calls, and also needs to be used outside when validating external
** input (for example starting from a restored state which could be faulty).
** Returns nonzero if the parameters are erratic. It does not write any of
** the data structures. 'n' is the task to check (0-15, unchecked). Returns
** 1 if not valid, 0 otherwise. 'd' is the application state to check. */
auint rrpge_m_taskcheck(uint16 const* d, auint n);


/* Schedule kernel tasks if any is waiting to be started. This includes all
** preparatory actions for the particular task (such as clearing memories),
** calling the appropriate handler (callback), and setting task state
** indicating it is scheduled. Technically this is a part of rrpge_run(),
** excepts rrpge_m_info to be set up, it is just seperated for manageability.
** It generates RRPGE_HLT_FAULT causes if it fails (shouldn't happen). */
void rrpge_m_tasksched(void);


#endif
