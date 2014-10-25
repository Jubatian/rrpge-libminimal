/**
**  \file
**  \brief     Collection of implementations for rrpge_aq.h
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.10.25
*/


#include "rgm_main.h"



/* Get network availability - implementation of RRPGE library function */
rrpge_uint32 rrpge_getnetavail(rrpge_object_t* hnd)
{
 return (hnd->st.stat[RRPGE_STA_VARS + 0x1FU] & 1U);
}



/* Get device exceptation - implementation of RRPGE library function */
rrpge_uint32 rrpge_getlastdev(rrpge_object_t* hnd, rrpge_uint32 dev)
{
 if (dev >= 16U){ return 0U; }
 return (hnd->st.stat[RRPGE_STA_VARS + 0x30U + dev]);
}



/* Get allowed device types - implementation of RRPGE library function */
rrpge_uint32 rrpge_getalloweddevs(rrpge_object_t* hnd)
{
 return (hnd->st.stat[RRPGE_STA_VARS + 0x1CU]);
}



/* Get video mode - implementation of RRPGE library function */
rrpge_uint32 rrpge_getvidmode(rrpge_object_t* hnd)
{
 return (hnd->st.stat[RRPGE_STA_VARS + 0x12U] & 1U);
}



/* Get palette entry - implementation of RRPGE library function */
rrpge_uint32 rrpge_getpalentry(rrpge_object_t* hnd, rrpge_uint32 cid)
{
 if (cid >= 256U){ return 0U; }
 return (hnd->st.stat[RRPGE_STA_PAL + cid] & 0xFFFU);
}
