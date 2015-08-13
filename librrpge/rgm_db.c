/**
**  \file
**  \brief     Collection of implementations for rrpge_db.h
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.13
*/


#include "rgm_db.h"
#include "rgm_stat.h"



/* Sets a breakpoint. - implementation of RRPGE library function */
void rrpge_setbreak(rrpge_object_t* hnd, rrpge_iuint adr)
{
 /* !!! Implement */
}



/* Queries if a given address is a breakpoint. - implementation of RRPGE library function */
rrpge_ibool rrpge_isbreak(rrpge_object_t* hnd, rrpge_iuint adr)
{
 /* !!! Implement */
 return 0U;
}



/* Removes a breakpoint. - implementation of RRPGE library function */
void rrpge_rembreak(rrpge_object_t* hnd, rrpge_iuint adr)
{
 /* !!! Implement */
}



/* Gets a value from the PRAM. - implementation of RRPGE library function */
rrpge_iuint rrpge_get_pram(rrpge_object_t* hnd, rrpge_iuint adr)
{
 if (adr >= 0x100000U){ return 0U; }
 return (rrpge_iuint)(hnd->st.pram[adr]);
}



/* Sets a value in the PRAM. - implementation of RRPGE library function */
rrpge_iuint rrpge_set_pram(rrpge_object_t* hnd, rrpge_iuint adr, rrpge_iuint val)
{
 if (adr <  0x100000U){
  hnd->st.pram[adr] = (rrpge_uint32)(val);
 }
 return rrpge_get_pram(hnd, adr);
}



/* Gets a value from the DRAM. - implementation of RRPGE library function */
rrpge_iuint rrpge_get_dram(rrpge_object_t* hnd, rrpge_iuint adr)
{
 if (adr >= 0x10000U){ return 0U; }
 return (rrpge_iuint)(hnd->st.dram[adr]);
}



/* Sets a value in the DRAM. - implementation of RRPGE library function */
rrpge_iuint rrpge_set_dram(rrpge_object_t* hnd, rrpge_iuint adr, rrpge_iuint val)
{
 if (adr <  0x10000U){
  hnd->st.dram[adr] = (rrpge_uint16)(val);
 }
 return rrpge_get_dram(hnd, adr);
}



/* Gets a value from the State. - implementation of RRPGE library function */
rrpge_iuint rrpge_get_state(rrpge_object_t* hnd, rrpge_iuint adr)
{
 return rrpge_m_stat_get(hnd, adr);
}



/* Sets a value in the State. - implementation of RRPGE library function */
rrpge_iuint rrpge_set_state(rrpge_object_t* hnd, rrpge_iuint adr, rrpge_iuint val)
{
 rrpge_m_stat_set(hnd, adr, val);
 return rrpge_m_stat_get(hnd, adr);
}



/* Gets a value from the Stack. - implementation of RRPGE library function */
rrpge_iuint rrpge_get_stack(rrpge_object_t* hnd, rrpge_iuint adr)
{
 /* !!! Temporary implementation, works for now */
 auint sbt;
 auint stp;
 if (rrpge_m_stat_get(hnd, RRPGE_STA_VARS + 0x1AU) == 0U){ /* Separate stack */
  sbt = 0x10000U;
  stp = 0x18000U;
 }else{                   /* Data area stack */
  sbt = rrpge_m_stat_get(hnd, RRPGE_STA_VARS + 0x1BU);
  stp = rrpge_m_stat_get(hnd, RRPGE_STA_VARS + 0x1AU) + sbt;
 }
 if (adr >= (stp - sbt)){ return 0U; }
 return (rrpge_iuint)(hnd->st.dram[adr + sbt]);
}



/* Sets a value in the Stack. - implementation of RRPGE library function */
rrpge_iuint rrpge_set_stack(rrpge_object_t* hnd, rrpge_iuint adr, rrpge_iuint val)
{
 /* !!! Implement */
 return rrpge_get_stack(hnd, adr);
}



/* Requests kernel call parameters after a kernel call. - implementation of RRPGE library function */
rrpge_iuint rrpge_getkcallpar(rrpge_object_t* hnd, rrpge_uint16* buf)
{
 /* !!! Implement */
 return 0U;
}
