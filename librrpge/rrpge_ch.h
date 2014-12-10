/**
**  \file
**  \brief     LibRRPGE standard header package - data check methods
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#ifndef RRPGE_CH_H
#define RRPGE_CH_H


#include "rrpge_tp.h"



/**
**  \brief     Check application header.
**
**  Checks the Application Header (first 64 words of Application binary or
**  state) for validity. The provided area may be an application header
**  directly loaded from a binary, a state save header or an application
**  state. Returns fault code (RRPGE_ERR_INV group) if a problem is found,
**  otherwise RRPGE_ERR_OK.
**
**  \param[in]   d     The area to check (64 elements).
**  \return            Fault code.
*/
rrpge_iuint rrpge_checkapphead(rrpge_uint16 const* d);



/**
**  \brief     Check application state.
**
**  Checks a full Application State for validity including the application
**  header. Returns fault code (RRPGE_ERR_INV group) if a problem is found,
**  otherwise RRPGE_ERR_OK. Note that if this check passes on an emulator
**  state, that emulator state is possible to be emulated (rrpge_attachstate()
**  will succeed).
**
**  \param[in]   d     The state to check (1024 elements).
**  \return            Fault code.
*/
rrpge_iuint rrpge_checkappstate(rrpge_uint16 const* d);



/**
**  \brief     State save compatibility check.
**
**  Checks state - application compatibility. A state and an application is
**  only compatible if the application headers match (first 64 words, except
**  the type indicator).
**
**  \param[in]   sta   The state save to check, app. header (64 elements).
**  \param[in]   app   The application to check against, app. header (64 elements).
**  \return            Result of check: 1: compatible, 0: not compatible.
*/
rrpge_ibool rrpge_isstatecomp(rrpge_uint16 const* sta,
                              rrpge_uint16 const* app);


#endif
