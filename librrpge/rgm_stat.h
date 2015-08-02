/**
**  \file
**  \brief     Application state interface
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.02
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
*/


#ifndef RRPGE_M_STAT_H
#define RRPGE_M_STAT_H

#include "rgm_info.h"


/* Get handler function for a cell group in the state. The address passed to
** it is relative to the base address used when setting up the handler, it may
** be ignored for functions providing a single cell. */
typedef auint rrpge_m_stat_geth_t (rrpge_object_t* hnd, auint adr);


/* Set handler function for a cell group in the state. The address passed to
** it is relative to the base address used when setting up the handler, it may
** be ignored for functions providing a single cell. The return value is the
** value which was stored (which may differ from what was passed). */
typedef auint rrpge_m_stat_seth_t (rrpge_object_t* hnd, auint adr, auint val);


/* Retrieve the content of a cell in the state */
auint rrpge_m_stat_get(rrpge_object_t* hnd, auint adr);


/* Sets the content of a cell in the state. This function should also validate
** it truncating as necessary to fit. The return value should be the stored
** result. */
auint rrpge_m_stat_set(rrpge_object_t* hnd, auint adr, auint val);


/* Add a handler functions for a group of cells. */
void  rrpge_m_stat_addhandler(rrpge_m_stat_geth_t* geth, rrpge_m_stat_seth_t* seth,
                              auint adr, auint len);


#endif
