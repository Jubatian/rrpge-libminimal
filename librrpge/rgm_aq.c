/**
**  \file
**  \brief     Collection of implementations for rrpge_aq.h
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#include "rgm_main.h"



/* Get network availability - implementation of RRPGE library function */
rrpge_ibool rrpge_getnetavail(rrpge_object_t* hnd)
{
 return ((hnd->st.stat[RRPGE_STA_VARS + 0x1FU] & 1U) != 0U);
}



/* Get device exceptation - implementation of RRPGE library function */
rrpge_iuint rrpge_getlastdev(rrpge_object_t* hnd, rrpge_iuint dev)
{
 if (dev >= 16U){ return 0U; }
 return ((hnd->st.stat[RRPGE_STA_VARS + 0x30U + dev]) & 0xFFFFU);
}



/* Get allowed device types - implementation of RRPGE library function */
rrpge_iuint rrpge_getalloweddevs(rrpge_object_t* hnd)
{
 return ((hnd->st.stat[RRPGE_STA_VARS + 0x1CU]) & 0xFFFFU);
}



/* Get video mode - implementation of RRPGE library function */
rrpge_iuint rrpge_getvidmode(rrpge_object_t* hnd)
{
 return ((hnd->st.stat[RRPGE_STA_UPA_G + 0x7U] >> 12) & 1U);
}



/* Get stereoscopic 3D output properties - implementation of RRPGE library function */
rrpge_iuint rrpge_getst3dprops(rrpge_object_t* hnd)
{
 return (hnd->st.stat[RRPGE_STA_VARS + 0x17U] & 7U);
}



/* Get palette entry - implementation of RRPGE library function */
rrpge_iuint rrpge_getpalentry(rrpge_object_t* hnd, rrpge_iuint cid)
{
 if (cid >= 256U){ return 0U; }
 return (hnd->st.stat[RRPGE_STA_PAL + cid] & 0xFFFU);
}
