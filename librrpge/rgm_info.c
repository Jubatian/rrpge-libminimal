/**
**  \file
**  \brief     Global information structure.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
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
