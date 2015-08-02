/**
**  \file
**  \brief     Global information structure.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.01.08
**
**
** The global structure's fields are used within servicing one RRPGE library
** call (and not across calls, so the library is able to handle more emulation
** instances simultaneously as it is supposed to). The reason behind using
** this architecture is performance.
**
** Some of the values are just copies from the read only process descriptor,
** if modified, updated before return. The reason is that the process
** descriptor comes through an indirection by an RRPGE library interface data
** pointer, using that directly multiple times through non native sized (16bit
** instead of 32/64) is slower.
*/


#include "rgm_info.h"


rrpge_m_info_t  rrpge_m_info;
rrpge_object_t* rrpge_m_edat;


/* Empty allocator and deallocator for non-initialized behavior */

void* rrpge_m_malloc_def(rrpge_iuint siz){ return RRPGE_M_NULL; }
void  rrpge_m_free_def(void* ptr){}

/* Allocator and deallocator */

rrpge_malloc_t* rrpge_m_malloc = &rrpge_m_malloc_def;
rrpge_free_t*   rrpge_m_free   = &rrpge_m_free_def;
