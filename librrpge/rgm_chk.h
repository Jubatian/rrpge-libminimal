/**
**  \file
**  \brief     Application header & Read Only Process Descriptor checks
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.10.04
*/


#ifndef RRPGE_M_CHK_H
#define RRPGE_M_CHK_H


#include "rgm_info.h"
#include "rrpge_ch.h"


/* Check application header and return the offset of the application
** descriptor. This is used to provide the rrpge_checkapphead() function, and
** also for the initializer to load the app. descriptor after the check. */
auint rgm_chk_checkapphead(rrpge_uint16 const* d, auint* dof);


#endif
