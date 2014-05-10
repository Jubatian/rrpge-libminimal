/**
**  \file
**  \brief     LibRRPGE standard header package - data check methods
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.10
*/


#ifndef RRPGE_CH_H
#define RRPGE_CH_H


#include "rrpge_tp.h"



/**
**  \brief     Check application header.
**
**  Checks the Application Header for validity. The provided page may be an
**  application header directly loaded from a binary, a state save header or
**  a combined Read Only Process Descriptor. Returns fault code (RRPGE_ERR_INV
**  group) if a problem is found, otherwise RRPGE_ERR_OK.
**
**  \param[in]   d     The binary to check (4096 elements).
**  \return            Fault code.
*/
rrpge_uint32 rrpge_checkapphead(rrpge_uint16 const* d);



/**
**  \brief     Check Read Only Process Descriptor.
**
**  Checks a full Read Only Process Descriptor for validity including the
**  application header. Returns fault code (RRPGE_ERR_INV group) if a problem
**  is found, otherwise RRPGE_ERR_OK.
**
**  \param[in]   d     The ROPD page to check (4096 elements).
**  \return            Fault code.
*/
rrpge_uint32 rrpge_checkropd(rrpge_uint16 const* d);



/**
**  \brief     State save compatibility check.
**
**  Checks state - application compatibility. A state and an application is
**  only compatible if the application headers match (except the type
**  indicator) until 0xC00.
**
**  \param[in]   sta   The state save to check, app. header (4096 elements).
**  \param[in]   app   The application to check against, app. header (4096 elements).
**  \return            Result of check: 1: compatible, 0: not compatible.
*/
rrpge_uint32 rrpge_isstatecomp(rrpge_uint16 const* sta,
                               rrpge_uint16 const* app);


#endif
