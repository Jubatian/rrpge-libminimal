/**
**  \file
**  \brief     Application state interface
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.13
**
**
**  Provides interface for the Application State as defined in the RRPGE
**  specification, in the range 0x000 - 0x3FF.
**
**  The interface is designed around a function table: every component of the
**  library may submit functions to this table to complete it for generating a
**  complete application state.
**
**  The existence of this component is mixed: it is necessary to provide the
**  proper interface, however it also exposes a hard to track interface over
**  which many components may depend. The same time assuming the RRPGE
**  specification is fixed, it lessens dependency on internal interfaces since
**  it can substitute many of those.
**
**  By default every location ignores writes and returns zero.
**
**  Note that it assumes at least one call to rrpge_m_stat_add_rw_handler()
**  or rrpge_m_stat_ac_handler() before use, which should be utilized to
**  initialize.
*/


#ifndef RRPGE_M_STAT_H
#define RRPGE_M_STAT_H

#include "rgm_info.h"


/* Get handler function for a cell group in the state. The address passed to
** it is relative to the base address used when setting up the handler, it may
** be ignored for functions providing a single cell. */
typedef RRPGE_M_FASTCALL auint rrpge_m_stat_geth_t (rrpge_object_t* hnd, auint adr);


/* Set handler function for a cell group in the state. The address passed to
** it is relative to the base address used when setting up the handler, it may
** be ignored for functions providing a single cell. */
typedef RRPGE_M_FASTCALL void  rrpge_m_stat_seth_t (rrpge_object_t* hnd, auint adr, auint val);


/* Read handler function for a cell group in the state. Like the get handler,
** but should be used to execute reads performed on the emulated bus. As such
** it also takes the 'rmw' signal (Nonzero for a read-modify-write access'
** read component) */
typedef RRPGE_M_FASTCALL auint rrpge_m_stat_readh_t (rrpge_object_t* hnd, auint adr, auint rmw);


/* Write handler function for a cell group in the state. Like the set handler,
** but should be used to execute reads performed on the emulated bus. */
typedef RRPGE_M_FASTCALL void  rrpge_m_stat_writeh_t (rrpge_object_t* hnd, auint adr, auint val);


/* Retrieve the content of a cell in the state using the get handler. */
RRPGE_M_FASTCALL auint rrpge_m_stat_get(rrpge_object_t* hnd, auint adr);


/* Sets the content of a cell in the state using the set handler. This
** function should also validate it truncating as necessary to fit. The
** return value should be the stored result. */
RRPGE_M_FASTCALL void  rrpge_m_stat_set(rrpge_object_t* hnd, auint adr, auint val);


/* Retrieves the content of a cell using bus access (read handler). */
RRPGE_M_FASTCALL auint rrpge_m_stat_read(rrpge_object_t* hnd, auint adr, auint rmw);


/* Sets the content of a cell using bus access (write handler). */
RRPGE_M_FASTCALL void  rrpge_m_stat_write(rrpge_object_t* hnd, auint adr, auint val);


/* Add Read and Write handler functions for a group of cells. */
void  rrpge_m_stat_add_rw_handler(rrpge_m_stat_readh_t* readh,
                                  rrpge_m_stat_writeh_t* writeh,
                                  auint adr, auint len);


/* Add access handler functions for a group of cells. Unless provided, the
** appropriate Read and Write functions are used (with rmw signal off). */
void  rrpge_m_stat_add_ac_handler(rrpge_m_stat_geth_t* geth,
                                  rrpge_m_stat_seth_t* seth,
                                  auint adr, auint len);


#endif
