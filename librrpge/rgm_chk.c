/**
**  \file
**  \brief     Application header & Read Only Process Descriptor checks
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#include "rgm_chk.h"
#include "rgm_task.h"



/* State: Zero bits to check for in the first 0x100 words (address, data high,
** data low). Large uniform ranges are not included (checked by code). */
#define STAZC_CT 42U
static const uint8  rrpge_m_chk_stazc[3U * STAZC_CT] = {
 0x4DU, 0xFFU, 0xFFU, /* Reserved */
 0x4EU, 0xFFU, 0xFFU, /* Reserved */
 0x4FU, 0xFFU, 0xFFU, /* Reserved */
 0x52U, 0xFFU, 0xFFU, /* Reserved */
 0x56U, 0xFFU, 0xFFU, /* Reserved */
 0x57U, 0xFFU, 0xF8U, /* Stereoscopic 3D */
 0x5EU, 0xFFU, 0xFFU, /* Reserved */
 0x5FU, 0xFFU, 0xFEU, /* Network availability */
 0x65U, 0xFFU, 0xFFU, /* Reserved */
 0x66U, 0xFFU, 0xFFU, /* Reserved */
 0x67U, 0xFFU, 0xFFU, /* Reserved */
 0x6DU, 0xFFU, 0xFFU, /* Reserved */
 0x6EU, 0xFFU, 0xFFU, /* Reserved */
 0x6FU, 0xFFU, 0xFFU, /* Reserved */
 0xC0U, 0xFFU, 0xFFU, /* UPA, 0x0000 (Audio) */
 0xC8U, 0x80U, 0x00U, /* UPA, FIFO location & size */
 0xC9U, 0xFFU, 0xFEU, /* UPA, FIFO status flags (No suspend for Mixer) */
 0xCAU, 0xFFU, 0xFFU, /* UPA, FIFO address word (read) */
 0xCBU, 0xFFU, 0xFFU, /* UPA, FIFO data word (read) */
 0xCCU, 0x80U, 0x00U, /* UPA, FIFO location & size */
 0xCDU, 0xFFU, 0xFCU, /* UPA, FIFO status flags */
 0xCEU, 0xFFU, 0xFFU, /* UPA, FIFO address word (read) */
 0xCFU, 0xFFU, 0xFFU, /* UPA, FIFO data word (read) */
 0xD4U, 0x80U, 0x80U, /* UPA, GDG shift mode region A */
 0xD5U, 0x80U, 0x80U, /* UPA, GDG shift mode region B */
 0xD7U, 0x08U, 0x00U, /* UPA, GDG display list definition */
 0xE0U, 0xFEU, 0x00U, /* UPA, Pointer 0 address high */
 0xE2U, 0xFEU, 0x00U, /* UPA, Pointer 0 increment high */
 0xE4U, 0xFFU, 0xF0U, /* UPA, Pointer 0 data unit size */
 0xE5U, 0xFFU, 0xFFU, /* UPA, 0x0000 */
 0xE8U, 0xFEU, 0x00U, /* UPA, Pointer 1 address high */
 0xEAU, 0xFEU, 0x00U, /* UPA, Pointer 1 increment high */
 0xECU, 0xFFU, 0xF0U, /* UPA, Pointer 1 data unit size */
 0xEDU, 0xFFU, 0xFFU, /* UPA, 0x0000 */
 0xF0U, 0xFEU, 0x00U, /* UPA, Pointer 2 address high */
 0xF2U, 0xFEU, 0x00U, /* UPA, Pointer 2 increment high */
 0xF4U, 0xFFU, 0xF0U, /* UPA, Pointer 2 data unit size */
 0xF5U, 0xFFU, 0xFFU, /* UPA, 0x0000 */
 0xF8U, 0xFEU, 0x00U, /* UPA, Pointer 3 address high */
 0xFAU, 0xFEU, 0x00U, /* UPA, Pointer 3 increment high */
 0xFCU, 0xFFU, 0xF0U, /* UPA, Pointer 3 data unit size */
 0xFDU, 0xFFU, 0xFFU, /* UPA, 0x0000 */
};




/* Read a byte from word memory */
static auint rrpge_m_chk_getb(uint16 const* d, auint o)
{
 return ((d[o >> 1] >> (((o & 1U) ^ 1U) << 3)) & 0xFFU);
}



/* Checks if an area is plain ASCII7 without control codes (only 0x20 - 0x7E
** must occur). Start and end are in bytes. Returns the location of the
** problem if found, otherwise the return equals the end. The end location is
** not included in the check. */
static auint rrpge_m_chk_ascii7(uint16 const* d, auint s, auint e)
{
 auint i;
 auint b;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if ( (b < 0x20U) || (b > 0x7EU) ) break;
 }
 return i;
}



/* Checks if an area is a valid user ID (character range: [a-z][A-Z][0-9]_ and
** 0x20 space, the latter can not occur as first char). Start and end are in
** bytes (end normally should be start + 16). Returns the location of the
** problem if found, otherwise the return equals the end. The end location is
** not included in the check. */
static auint rrpge_m_chk_userid(uint16 const* d, auint s, auint e)
{
 auint i;
 auint b;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if ( ((b < (auint)('a')) || (b > (auint)('z'))) &&
       ((b < (auint)('A')) || (b > (auint)('Z'))) &&
       ((b < (auint)('0')) || (b > (auint)('9'))) &&
       ( b != (auint)('-')) &&
       ( b != (auint)(' ')) ) break;
  if ( (i == s) && (b == (auint)(' ')) ) break;
 }
 return i;
}



/* Checks if an area is numeric and extracts the numeric information. Start
** and end are in bytes. Returns the location of the problem if found,
** otherwise the return equals the end. The end location is not included in
** the check. */
static auint rrpge_m_chk_numeric(uint16 const* d, auint s, auint e, auint* v)
{
 auint i;
 auint b;
 *v = 0U;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if ( (b < (auint)('0')) || (b > (auint)('9')) ) break;
  *v = ((*v) * 10U) + (b - (auint)('0'));
 }
 return i;
}



/* Checks if an area is uppercase hexadecimal, and extracts the numeric
** information. Start and end are in bytes. Returns the location of the
** problem if found, otherwise the return equals the end. The end location is
** not included in the check. */
static auint rrpge_m_chk_uhex(uint16 const* d, auint s, auint e, auint* v)
{
 auint i;
 auint b;
 *v = 0U;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if       ( (b >= (auint)('0')) && (b <= (auint)('9')) ){
   *v = ((*v) << 4) + (b - (auint)('0'));
  }else if ( (b >= (auint)('A')) && (b <= (auint)('F')) ){
   *v = ((*v) << 4) + (b - (auint)('A') + 10U);
  }else{
   break;
  }
 }
 return i;
}



/* Checks against a given string. This string must be long enough for the
** check. Start and end are in bytes. Returns the location of the problem if
** found, otherwise the return equals the end. The end location is not
** included in the check. */
static auint rrpge_m_chk_str(uint16 const* d, auint s, auint e, uint8 const* c)
{
 auint i;
 auint b;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if (b != (c[i - s] & 0xFFU)) break;
 }
 return i;
}



/* Check application header and return the offset of the application
** descriptor. This is used to provide the rrpge_checkapphead() function, and
** also for the initializer to load the app. descriptor after the check. */
auint rgm_chk_checkapphead(rrpge_uint16 const* d, auint* dof)
{
 auint f;
 auint c;
 auint dum;
 auint smaj;      /* RRPGE specification version */
 auint smin;
 auint spat;

 /* Note: RRPGE specification compatibility may be checked here. Currently not
 ** implemented. */

 f = rrpge_m_chk_str(d,  0U,  2U, (uint8 const*)("RP"));
 if (f != 2U){ goto apph_fault; }

 f = 2U;
 c = (d[f >> 1] >> 8) & 0xFFU;
 if ((c != (auint)('A')) && (c != (auint)('S'))){ goto apph_fault; }

 f = rrpge_m_chk_str(d,  3U, 14U, (uint8 const*)("\n\nAppAuth: "));
 if (f != 14U){ goto apph_fault; }

 f = rrpge_m_chk_userid(d, 14U, 30U);
 if (f != 30U){ goto apph_fault; }

 f = rrpge_m_chk_str(d, 30U, 40U, (uint8 const*)("\nAppName: "));
 if (f != 40U){ goto apph_fault; }

 f = rrpge_m_chk_ascii7(d, 40U, 74U);
 if (f != 74U){ goto apph_fault; }

 f = rrpge_m_chk_str(d, 74U, 84U, (uint8 const*)("\nVersion: "));
 if (f != 84U){ goto apph_fault; }

 f = rrpge_m_chk_numeric(d, 84U, 86U, &dum); /* App. major version */
 if (f != 86U){ goto apph_fault; }

 f = rrpge_m_chk_str(d, 86U, 87U, (uint8 const*)("."));
 if (f != 87U){ goto apph_fault; }

 f = rrpge_m_chk_numeric(d, 87U, 90U, &dum); /* App. minor version */
 if (f != 90U){ goto apph_fault; }

 f = rrpge_m_chk_str(d, 90U, 91U, (uint8 const*)("."));
 if (f != 91U){ goto apph_fault; }

 f = rrpge_m_chk_numeric(d, 91U, 94U, &dum); /* App. patch version */
 if (f != 94U){ goto apph_fault; }

 f = rrpge_m_chk_str(d,  94U, 104U, (uint8 const*)("\nEngSpec: "));
 if (f != 104U){ goto apph_fault; }

 f = rrpge_m_chk_numeric(d, 104U, 106U, &smaj);
 if (f != 106U){ goto apph_fault; }

 f = rrpge_m_chk_str(d, 106U, 107U, (uint8 const*)("."));
 if (f != 107U){ goto apph_fault; }

 f = rrpge_m_chk_numeric(d, 107U, 110U, &smin);
 if (f != 110U){ goto apph_fault; }

 f = rrpge_m_chk_str(d, 110U, 111U, (uint8 const*)("."));
 if (f != 111U){ goto apph_fault; }

 f = rrpge_m_chk_numeric(d, 111U, 114U, &spat);
 if (f != 114U){ goto apph_fault; }

 f = rrpge_m_chk_str(d, 114U, 124U, (uint8 const*)("\nDescOff: "));
 if (f != 124U){ goto apph_fault; }

 f = rrpge_m_chk_uhex(d, 124U, 128U, dof);
 if (f != 128U){ goto apph_fault; }

 return RRPGE_ERR_OK;

apph_fault:

 return RRPGE_ERR_STA + (f >> 1);
}



/* Check application header - Implementation of RRPGE library function */
rrpge_uint32 rrpge_checkapphead(rrpge_uint16 const* d)
{
 auint dum;
 return (rrpge_uint32)(rgm_chk_checkapphead(d, &dum));
}



/* Check Application State - Implementation of RRPGE library function */
rrpge_uint32 rrpge_checkappstate(rrpge_uint16 const* d)
{
 auint f;
 auint i;
 auint dof;
 auint dol;
 auint dsz;
 auint aps;

 f = rrpge_checkapphead(d);
 if (f != RRPGE_ERR_OK){ return f; }

 f = rrpge_m_chk_uhex(d, 124U, 128U, &dof);  /* Descriptor's offset */
 if (f != 128U){ f >>= 1; goto stat_fault; } /* (Should not happen) */

 /* Check elements in the Application Descriptor (reports as invalid state
 ** elements, it is OK even though descriptor faults would be nicer). */

 f = RRPGE_STA_VARS + 0x18U;
 aps = ((d[f     ] & 0xFFFFU) << 16) +
       ((d[f + 1U] & 0xFFFFU));
 if (aps < 64U){ goto stat_fault; }          /* Definitely too small */

 dol = aps;
 if (dol > 0x10000U){ dol = 0x10000U; }      /* App. Descriptor's position limit */

 f = RRPGE_STA_VARS + 0x1BU;
 if ((d[f - 1U] & 0xFFFFU) != 0U){           /* In-data stack */
  if ((d[f] & 0xFFFFU) < 64U){ goto stat_fault; }
  if (((d[f - 1U] & 0xFFFFU) + (d[f] & 0xFFFFU)) > 0x10000U){ goto stat_fault; }
 }

 f = RRPGE_STA_VARS + 0x1DU;
 if ((d[f] & 0x0078U) != 0U){ goto stat_fault; } /* Reserved bits */
 if ( ((d[f] & 0x0300U) == 0U) &&
      ((d[f] & 0x0080U) != 0U) ){ goto stat_fault; } /* No icon, but alternate is set */
 dsz = 12U;
 if ((d[f] & 0x2000U) != 0U){ dsz += 2U; }
 if ((d[f] & 0x1000U) != 0U){ dsz += 2U; }
 if ((d[f] & 0x0300U) != 0U){ dsz += 2U; }
 if ((d[f] & 0x0080U) != 0U){ dsz += 2U; }
 f = 124U >> 1;                              /* Descriptor out of range? */
 if ((dof + dsz) > dol){ goto stat_fault; }

 /* Normal app. state elements */

 /* Colors: must have their high 4 bits clear */

 for (f = RRPGE_STA_PAL; f < RRPGE_STA_PAL + 0x100U; f++){
  if ((d[f] & 0xF000U) != 0U){ goto stat_fault; }
 }

 /* Variables */

 for (f = RRPGE_STA_VARS + 0x30; f < RRPGE_STA_VARS + 0x40U; f++){
  if ((d[f] & 0x07E0U) != 0U){ goto stat_fault; } /* Last device IDs */
 }

 for (f = RRPGE_STA_VARS + 0x40; f < RRPGE_STA_VARS + 0x4FU; f++){
  if ((d[f] & 0xFFFFU) != 0U){ goto stat_fault; } /* Reserved area */
 }

 /* Zero mask checks */

 for (i = 0U; i < STAZC_CT; i++){
  f = rrpge_m_chk_stazc[i * 3U];
  if ( (d[f] & ( ((rrpge_m_chk_stazc[i * 3U + 1U] & 0xFFU) << 8) |
                 ((rrpge_m_chk_stazc[i * 3U + 2U] & 0xFFU)) ) ) != 0U){
   goto stat_fault;
  }
 }

 /* Kernel tasks */

 for (i = 0U; i < 16U; i++){
  if (rrpge_m_taskcheck(d, i)){
   f = RRPGE_STA_KTASK + (i << 4);
   goto stat_fault;
  }
 }

 return RRPGE_ERR_OK;

stat_fault:

 return RRPGE_ERR_STA + f;
}



/* Check state - app. comp. - Implementation of RRPGE library function */
rrpge_uint32 rrpge_isstatecomp(rrpge_uint16 const* sta,
                               rrpge_uint16 const* app)
{
 auint i;

 /* Simple: the headers must match */

 if ((sta[0] & 0xFFFFU) != ((auint)('R') << 8) + ((auint)( 'P'))){ return 0U; }
 if ((sta[1] & 0xFFFFU) != ((auint)('S') << 8) + ((auint)('\n'))){ return 0U; }
 for (i = 2U; i < 64U; i++){
  if ((sta[i] & 0xFFFFU) != (app[i] & 0xFFFFU)){ return 0U; }
 }
 return 1U;
}
