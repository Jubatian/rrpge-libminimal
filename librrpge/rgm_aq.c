/**
**  \file
**  \brief     Collection of implementations for rrpge_aq.h
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.15
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



/* Get allowed device types - implementation of RRPGE library function */
rrpge_uint32 rrpge_getalloweddevs(rrpge_object_t* hnd)
{
 rrpge_uint32 r = 0U;
 if ((hnd->stat.ropd[0xBC1U] & 0x0800U) != 0U){
  r |= (auint)(1U) << (hnd->stat.ropd[0xBC1U] >> 12);
 }
 if ((hnd->stat.ropd[0xBC4U] & 0x0400U) == 0U){
  return r;
 }
 if ((hnd->stat.ropd[0xBC5U] & 0x8000U) == 0U){
  return r;
 }
 r |= hnd->stat.ropd[0xBD0U];
 return r;
}



/* Get touch sensitive area - implementation of RRPGE library function */
rrpge_uint32 rrpge_gettoucharea(rrpge_object_t* hnd, rrpge_uint32 ari, rrpge_uint32* cor)
{
 cor[0] = 0U;
 cor[1] = 0U;
 cor[2] = 0U;
 cor[3] = 0U;
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
