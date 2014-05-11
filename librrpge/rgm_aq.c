/**
**  \file
**  \brief     Collection of implementations for rrpge_aq.h
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.11
*/


#include "rgm_main.h"



/* Get network availability - implementation of RRPGE library function */
rrpge_uint32 rrpge_getnetavail(rrpge_object_t* hnd)
{
 return (hnd->stat.ropd[0xD3FU] & 1U);
}



/* Get device exceptation - implementation of RRPGE library function */
rrpge_uint32 rrpge_getlastdev(rrpge_object_t* hnd, rrpge_uint32 dev)
{
 if (dev >= 16U){ return 0U; }
 return (hnd->stat.ropd[0xEC0U + dev]);
}



/* Get touch sensitive area - implementation of RRPGE library function */
rrpge_uint32 rrpge_gettoucharea(rrpge_object_t* hnd, rrpge_uint32 ari, rrpge_uint32* cor)
{
 if (ari >= 16U){ return 0U; }
 if ( (hnd->stat.ropd[0xEA0 + ari] == 0U) || /* Width */
      (hnd->stat.ropd[0xEB0 + ari] == 0U) ){ /* Height */
  return 0U;
 }
 cor[0] = hnd->stat.ropd[0xE80 + ari]; /* X */
 cor[1] = hnd->stat.ropd[0xE90 + ari]; /* Y */
 cor[2] = hnd->stat.ropd[0xEA0 + ari]; /* Width */
 cor[3] = hnd->stat.ropd[0xEB0 + ari]; /* Height */
 return 1U;
}



/* Get video mode - implementation of RRPGE library function */
rrpge_uint32 rrpge_getvidmode(rrpge_object_t* hnd)
{
 return (hnd->stat.ropd[0xD57U] & 1U);
}



/* Get palette entry - implementation of RRPGE library function */
rrpge_uint32 rrpge_getpalentry(rrpge_object_t* hnd, rrpge_uint32 cid)
{
 if (cid >= 256U){ return 0U; }
 return (hnd->stat.ropd[0xC00U + cid]);
}
