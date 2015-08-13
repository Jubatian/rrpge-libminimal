/**
**  \file
**  \brief     Application state interface
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.13
*/


#include "rgm_stat.h"


/* Length of state */
#define STAT_LEN 0x400U


/* Initialized? */
static auint rrpge_m_stat_isinit = 0U;


/* Table of read handlers */
static rrpge_m_stat_readh_t*  rrpge_m_stat_readf[STAT_LEN];
/* Base offsets for each get handler */
static auint                  rrpge_m_stat_readb[STAT_LEN];
/* Table of write handlers */
static rrpge_m_stat_writeh_t* rrpge_m_stat_writef[STAT_LEN];
/* Base offsets for each set handler */
static auint                  rrpge_m_stat_writeb[STAT_LEN];
/* Table of get handlers */
static rrpge_m_stat_geth_t*   rrpge_m_stat_getf[STAT_LEN];
/* Base offsets for each get handler */
static auint                  rrpge_m_stat_getb[STAT_LEN];
/* Table of set handlers */
static rrpge_m_stat_seth_t*   rrpge_m_stat_setf[STAT_LEN];
/* Base offsets for each set handler */
static auint                  rrpge_m_stat_setb[STAT_LEN];



/* !!! These are supposed to be the defaults */
/* Default read handler */
/* RRPGE_M_FASTCALL static auint rrpge_m_stat_def_read(rrpge_object_t* hnd, auint adr, auint rmw){ return 0U; } */
/* Default write handler */
/* RRPGE_M_FASTCALL static void  rrpge_m_stat_def_write(rrpge_object_t* hnd, auint adr, auint val){} */
/* Default get handler */
RRPGE_M_FASTCALL static auint rrpge_m_stat_def_get(rrpge_object_t* hnd, auint adr){
 if (adr >= STAT_LEN){ return 0U; }
 return rrpge_m_stat_readf[adr](hnd, adr - rrpge_m_stat_readb[adr], 0U);
}
/* Default set handler (can use the normal write function) */
#define rrpge_m_stat_def_set rrpge_m_stat_write

/* !!! Temporary default handlers to provide the state array for elements
** which weren't refactored yet. Once it is done, that array will be removed
** and here the normal default handler will take precedence. */
/* Default read handler */
RRPGE_M_FASTCALL static auint rrpge_m_stat_def_read(rrpge_object_t* hnd, auint adr, auint rmw)
{
 return (hnd->st.stat[adr]);
}
/* Default write handler */
RRPGE_M_FASTCALL static void  rrpge_m_stat_def_write(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->st.stat[adr] = (val & 0xFFFFU);
}


/* Initializator */
static void rrpge_m_stat_init(void)
{
 auint i;

 for (i = 0U; i < STAT_LEN; i++){
  rrpge_m_stat_readf[i]  = &rrpge_m_stat_def_read;
  rrpge_m_stat_readb[i]  = 0U;
  rrpge_m_stat_writef[i] = &rrpge_m_stat_def_write;
  rrpge_m_stat_writeb[i] = 0U;
  rrpge_m_stat_getf[i]   = &rrpge_m_stat_def_get;
  rrpge_m_stat_getb[i]   = 0U;
  rrpge_m_stat_setf[i]   = &rrpge_m_stat_def_set;
  rrpge_m_stat_setb[i]   = 0U;
 }

 rrpge_m_stat_isinit = 1U;
}



/* Retrieve the content of a cell in the state using the get handler. */
RRPGE_M_FASTCALL auint rrpge_m_stat_get(rrpge_object_t* hnd, auint adr)
{
 if (adr >= STAT_LEN){ return 0U; }

 return rrpge_m_stat_getf[adr](hnd, adr - rrpge_m_stat_getb[adr]);
}



/* Sets the content of a cell in the state using the set handler. This
** function should also validate it truncating as necessary to fit. */
RRPGE_M_FASTCALL void  rrpge_m_stat_set(rrpge_object_t* hnd, auint adr, auint val)
{
 if (adr >= STAT_LEN){ return; }

 rrpge_m_stat_setf[adr](hnd, adr - rrpge_m_stat_setb[adr], val);
}



/* Retrieves the content of a cell using bus access (read handler). */
RRPGE_M_FASTCALL auint rrpge_m_stat_read(rrpge_object_t* hnd, auint adr, auint rmw)
{
 if (adr >= STAT_LEN){ return 0U; }

 return rrpge_m_stat_readf[adr](hnd, adr - rrpge_m_stat_readb[adr], rmw);
}



/* Sets the content of a cell using bus access (write handler). */
RRPGE_M_FASTCALL void  rrpge_m_stat_write(rrpge_object_t* hnd, auint adr, auint val)
{
 if (adr >= STAT_LEN){ return; }

 rrpge_m_stat_writef[adr](hnd, adr - rrpge_m_stat_writeb[adr], val);
}



/* Add Read and Write handler functions for a group of cells. */
void  rrpge_m_stat_add_rw_handler(rrpge_m_stat_readh_t* readh,
                                  rrpge_m_stat_writeh_t* writeh,
                                  auint adr, auint len)
{
 auint i;

 if (!rrpge_m_stat_isinit){ rrpge_m_stat_init(); }

 if (adr >= STAT_LEN){ return; }
 if (adr + len >= STAT_LEN){ len = STAT_LEN - adr; }

 for (i = adr; i < (adr + len); i++){
  rrpge_m_stat_readf[i]  = readh;
  rrpge_m_stat_readb[i]  = adr;
  rrpge_m_stat_writef[i] = writeh;
  rrpge_m_stat_writeb[i] = adr;
 }
}



/* Add access handler functions for a group of cells. Unless provided, the
** appropriate Read and Write functions are used (with rmw signal off). */
void  rrpge_m_stat_add_ac_handler(rrpge_m_stat_geth_t* geth,
                                  rrpge_m_stat_seth_t* seth,
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
