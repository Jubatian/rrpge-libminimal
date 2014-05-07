/**
**  \file
**  \brief     LibRRPGE standard header package - data check methods
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#ifndef RRPGE_CH_H
#define RRPGE_CH_H


#include "rrpge_tp.h"



/**
**  \brief     Check and import binary header info.
**
**  Checks common binary header (first 96 bytes of binaries), and returns
**  information on them. Returns fault code (RRPGE_ERR_INV group) if a problem
**  is found, otherwise RRPGE_ERR_OK, and the informations properly filled. It
**  can be used for any binary (application, state save, nonvolatile save).
**
**  \param[in]   d     The binary to check and import from (48 elements).
**  \param[out]  inf   The information structure to fill.
**  \return            Fault code.
*/
rrpge_uint32 rrpge_checkcommhead(rrpge_uint16 const* d, rrpge_header_t* inf);



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
**  only compatible if their author, name and version precisely match. Returns
**  1 if they are compatible, 0 otherwise. Does not check the type of the
**  header provided for the application.
**
**  \param[in]   sta   The state save to check.
**  \param[in]   app   The application to check against.
**  \return            Result of check: 1: compatible, 0: not compatible.
*/
rrpge_uint32 rrpge_isstatecomp(rrpge_header_t const* sta,
                               rrpge_header_t const* app);


#endif
