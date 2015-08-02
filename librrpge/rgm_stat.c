/**
**  \file
**  \brief     Application state interface
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
*/


#include "rgm_stat.h"


/* Length of state */
#define STAT_LEN 0x400U


/* Initialized? */
static auint rrpge_m_stat_isinit = 0U;


/* Table of get handlers */
static rrpge_m_stat_geth_t* rrpge_m_stat_getf[STAT_LEN];
/* Base offsets for each get handler */
static auint                rrpge_m_stat_getb[STAT_LEN];
/* Table of set handlers */
static rrpge_m_stat_seth_t* rrpge_m_stat_setf[STAT_LEN];
/* Base offsets for each set handler */
static auint                rrpge_m_stat_setb[STAT_LEN];
/* Memory mapped cell map */
static uint32               rrpge_m_stat_ismm[(STAT_LEN + 31U) >> 5];



/* !!! These are supposed to be the defaults */
/* Default get handler */
/* static auint rrpge_m_stat_def_get(rrpge_object_t* hnd, auint adr){ return 0U; } */
/* Default set handler */
/* static auint rrpge_m_stat_def_set(rrpge_object_t* hnd, auint adr, auint val){ return 0U; } */

/* !!! Temporary default handlers to provide the state array for elements
** which weren't refactored yet. Once it is done, that array will be removed
** and here the normal default handler will take precedence. */
/* Default get handler */
static auint rrpge_m_stat_def_get(rrpge_object_t* hnd, auint adr)
{
 return (hnd->st.stat[adr]);
}
/* Default set handler */
static auint rrpge_m_stat_def_set(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->st.stat[adr] = (val & 0xFFFFU);
 return (val & 0xFFFFU);
}


/* Initializator */
static void rrpge_m_stat_init(void)
{
 auint i;

 for (i = 0U; i < STAT_LEN; i++){
  rrpge_m_stat_getf[i] = &rrpge_m_stat_def_get;
  rrpge_m_stat_getb[i] = 0U;
  rrpge_m_stat_setf[i] = &rrpge_m_stat_def_set;
  rrpge_m_stat_setb[i] = 0U;
  rrpge_m_stat_ismm[i >> 5] = 0U;
 }

 rrpge_m_stat_isinit = 1U;
}



/* Retrieve the content of a cell in the state */
auint rrpge_m_stat_get(rrpge_object_t* hnd, auint adr)
{
 if (adr >= STAT_LEN){ return 0U; }

 return rrpge_m_stat_getf[adr](hnd, adr - rrpge_m_stat_getb[adr]);
}



/* Sets the content of a cell in the state. This function should also validate
** it truncating as necessary to fit. The return value should be the stored
** result. */
auint rrpge_m_stat_set(rrpge_object_t* hnd, auint adr, auint val)
{
 if (adr >= STAT_LEN){ return 0U; }

 return rrpge_m_stat_setf[adr](hnd, adr - rrpge_m_stat_setb[adr], val);
}



/* Export the content of a cell in the state. The same as rrpge_m_stat_get()
** except that it doesn't access memory mapped registers in the state. */
auint rrpge_m_stat_load(rrpge_object_t* hnd, auint adr)
{
 if (adr >= STAT_LEN){ return 0U; }
 if ((rrpge_m_stat_ismm[adr >> 5] & (1U << (adr & 0x1FU))) != 0U){ return 0U; }

 return rrpge_m_stat_getf[adr](hnd, adr - rrpge_m_stat_getb[adr]);
}



/* Import the content of a cell in the state. The same as rrpge_m_stat_set()
** except that it doesn't access memory mapped registers in the state. */
auint rrpge_m_stat_save(rrpge_object_t* hnd, auint adr, auint val)
{
 if (adr >= STAT_LEN){ return 0U; }
 if ((rrpge_m_stat_ismm[adr >> 5] & (1U << (adr & 0x1FU))) != 0U){ return 0U; }

 return rrpge_m_stat_setf[adr](hnd, adr - rrpge_m_stat_setb[adr], val);
}



/* Add a handler functions for a group of cells. */
void  rrpge_m_stat_addhandler(rrpge_m_stat_geth_t* geth, rrpge_m_stat_seth_t* seth,
                              auint adr, auint len)
{
 auint i;

 if (!rrpge_m_stat_isinit){ rrpge_m_stat_init(); }

 if (adr >= STAT_LEN){ return; }
 if (adr + len >= STAT_LEN){ len = STAT_LEN - adr; }

 for (i = adr; i < (adr + len); i++){
  rrpge_m_stat_getf[i] = geth;
  rrpge_m_stat_getb[i] = adr;
  rrpge_m_stat_setf[i] = seth;
  rrpge_m_stat_setb[i] = adr;
 }
}



/* Set memory-mapped register cells in the state. These cells will be ignored
** (not set, always read as zero) when using the rrpge_m_stat_load() and
** rrpge_m_stat_save() functions on them. Nonzero sets memory-mapped. */
void  rrpge_m_stat_setmmcell(auint adr, auint ismm)
{
 if (!rrpge_m_stat_isinit){ rrpge_m_stat_init(); }

 if (adr >= STAT_LEN){ return; }

 if (ismm){
  rrpge_m_stat_ismm[adr >> 5] |=  (1U << (adr & 0x1FU));
 }else{
  rrpge_m_stat_ismm[adr >> 5] &= ~(1U << (adr & 0x1FU));
 }
}
