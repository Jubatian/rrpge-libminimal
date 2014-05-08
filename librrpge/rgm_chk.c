/**
**  \file
**  \brief     Application header & Read Only Process Descriptor checks
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.08
*/


#include "rgm_chk.h"
#include "rgm_task.h"



/* Read a byte from word memory */
static uint8 rrpge_m_chk_getb(uint16 const* d, auint o)
{
 return (uint8)(d[o >> 1] >> (((o & 1U) ^ 1U) << 3));
}



/* Checks if an area is plain ASCII7 without control codes (only 0x20 - 0x7E
** must occur). Start and end are in bytes. Returns the location of the
** problem if found, otherwise the return equals the end. The end location is
** not included in the check. */
static auint rrpge_m_chk_ascii7(uint16 const* d, auint s, auint e)
{
 auint i;
 uint8 b;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if ( (b < 0x20U) || (b > 0x7EU) ) break;
 }
 return i;
}



/* Checks if an area is plain ASCII7 without control codes except newline
** (only 0x0A and 0x20 - 0x7E must occur). Start and end are in bytes. Returns
** the location of the problem if found, otherwise the return equals the end.
** The end location is not included in the check. */
/* !!! Currently unused, will be used for checking the license field !!! */
static auint rrpge_m_chk_ascii7nl(uint16 const* d, auint s, auint e)
{
 auint i;
 uint8 b;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if ( ((b < 0x20U) && (b != 0x0AU)) || (b > 0x7EU) ) break;
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
 uint8 b;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if ( ((b < (uint8)('a')) || (b > (uint8)('z'))) &&
       ((b < (uint8)('A')) || (b > (uint8)('Z'))) &&
       ((b < (uint8)('0')) || (b > (uint8)('9'))) &&
       ( b != (uint8)('-')) &&
       ( b != (uint8)(' ')) ) break;
  if ( (i == s) && (b == (uint8)(' ')) ) break;
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
 uint8 b;
 *v = 0U;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if ( (b < (uint8)('0')) || (b > (uint8)('9')) ) break;
  *v = ((*v) * 10U) + (auint)(b - (uint8)('0'));
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
 uint8 b;
 for (i = s; i < e; i++){
  b = rrpge_m_chk_getb(d, i);
  if (b != c[i - s]) break;
 }
 return i;
}



/* Check and import header info - Implementation of RRPGE library function */
rrpge_uint32 rrpge_checkcommhead(rrpge_uint16 const* d, rrpge_header_t* inf)
{
 auint i;
 auint f;
 uint8 c;

 f = rrpge_m_chk_str(d,  0U,  2U, (uint8 const*)("RP"));
 if (f != 2U) goto commh_fault;

 f = 2U;
 c = (uint8)(d[f >> 1] >> 8);
 if ((c != 'A') && (c != 'N') && (c != 'S')) goto commh_fault;
 inf->tp = c;

 f = rrpge_m_chk_str(d,  3U, 14U, (uint8 const*)("\n\nAppAuth: "));
 if (f != 14U) goto commh_fault;

 f = rrpge_m_chk_userid(d, 14U, 30U);
 if (f != 30U) goto commh_fault;
 for (i = 0; i < 16U; i++){ inf->auth[i] = rrpge_m_chk_getb(d, i + 14U); }

 f = rrpge_m_chk_str(d, 30U, 40U, (uint8 const*)("\nAppName: "));
 if (f != 40U) goto commh_fault;

 f = rrpge_m_chk_ascii7(d, 40U, 74U);
 if (f != 74U) goto commh_fault;
 for (i = 0; i < 34U; i++){ inf->name[i] = rrpge_m_chk_getb(d, i + 40U); }

 f = rrpge_m_chk_str(d, 74U, 84U, (uint8 const*)("\nVersion: "));
 if (f != 84U) goto commh_fault;

 f = rrpge_m_chk_numeric(d, 84U, 86U, &(inf->vmaj));
 if (f != 86U) goto commh_fault;

 f = rrpge_m_chk_str(d, 86U, 87U, (uint8 const*)("."));
 if (f != 87U) goto commh_fault;

 f = rrpge_m_chk_numeric(d, 87U, 90U, &(inf->vmin));
 if (f != 90U) goto commh_fault;

 f = rrpge_m_chk_str(d, 90U, 91U, (uint8 const*)("."));
 if (f != 91U) goto commh_fault;

 f = rrpge_m_chk_numeric(d, 91U, 94U, &(inf->vpat));
 if (f != 94U) goto commh_fault;

 f = rrpge_m_chk_str(d, 94U, 96U, (uint8 const*)("\nE"));
 if (f != 96U) goto commh_fault;

 return RRPGE_ERR_OK;

commh_fault:

 return RRPGE_ERR_INV + (f >> 1);
}



/* Check application header - Implementation of RRPGE library function */
rrpge_uint32 rrpge_checkapphead(rrpge_uint16 const* d)
{
 auint f;
 rrpge_header_t dum;
 auint smaj;      /* RRPGE specification version */
 auint smin;
 auint spat;

 f = rrpge_checkcommhead(d, &dum);
 if (f != RRPGE_ERR_OK) return f;

 f = rrpge_m_chk_str(d,  96U, 104U, (uint8 const*)("ngSpec: "));
 if (f != 104U) goto apph_fault;

 f = rrpge_m_chk_numeric(d, 104U, 106U, &smaj);
 if (f != 106U) goto apph_fault;

 f = rrpge_m_chk_str(d, 106U, 107U, (uint8 const*)("."));
 if (f != 107U) goto apph_fault;

 f = rrpge_m_chk_numeric(d, 107U, 110U, &smin);
 if (f != 110U) goto apph_fault;

 f = rrpge_m_chk_str(d, 110U, 111U, (uint8 const*)("."));
 if (f != 111U) goto apph_fault;

 f = rrpge_m_chk_numeric(d, 111U, 114U, &spat);
 if (f != 114U) goto apph_fault;

 f = rrpge_m_chk_str(d, 114U, 124U, (uint8 const*)("\nLicense: "));
 if (f != 124U) goto apph_fault;

 /* !!! License field's validity checking is skipped for now !!! */
 /* !!! Later the specification version's appropriateness will also have to be
 ** checked !!! */

 f = 0xBC0U;
 if (d[f] > 1U) goto apph_fault;                /* 0xBC0: RRPGE variant */

 f++;
 if ( (d[f] & 0x8FF0U) != 0U ) goto apph_fault; /* 0xBC1: Peripheral req. */

 f++;
 if ( (d[f] & 0x8000U) != 0U ) goto apph_fault; /* 0xBC2: Data high, sound, code pg. */

 return RRPGE_ERR_OK;

apph_fault:

 return RRPGE_ERR_INV + f;
}



/* Check ROPD - Implementation of RRPGE library function */
rrpge_uint32 rrpge_checkropd(rrpge_uint16 const* d)
{
 auint f;
 auint i;

 f = rrpge_checkapphead(d);
 if (f != RRPGE_ERR_OK) return f;

 /* Read & Write pages: they must be in range and if a VRAM page is banked in
 ** for write, it needs to be banked in for read as well the same place. */
 for (f = 0xD00U; f < 0xD20U; f++){
  i = d[f];
  if ( ((i < 0x4000U) || (i >= 0x40E1U)) && /* Data RAM and ROPD */
       ((i < 0x8000U) || (i >= 0x8080U)) && /* Video RAM */
       (i != 0x7FFFU) &&                    /* Audio peripheral */
       (i != 0xBFFFU) ){                    /* Video peripheral */
   goto ropd_fault;
  }
 }
 for (f = 0xD10U; f < 0xD20U; f++){         /* Write pages */
  i = d[f];
  if ( (i == 0x40E0U) ||                    /* ROPD */
       ( ((i >= 0x8000U) && (i < 0x8080)) &&
         (i != d[f - 0x10U]) ) ){           /* VRAM page banked in, but not for read */
   goto ropd_fault;
  }
 }

 f = 0xD20U;
 if (d[f] > 400U) goto ropd_fault;          /* Video line IT, raster line */

 for (f = 0xD21U; f < 0xD30U; f++){         /* Reserved areas */
  if (d[f] != 0) goto ropd_fault;
 }

 for (f = 0xD32U; f < 0xD3FU; f++){         /* Reserved areas */
  if (d[f] != 0) goto ropd_fault;
 }

 f = 0xD3F;
 if (d[f] > 1U) goto ropd_fault;            /* Network availability */

 for (f = 0xD4DU; f < 0xD50U; f++){         /* Reserved areas */
  if (d[f] != 0) goto ropd_fault;
 }

 f = 0xD52;
 if ((d[f] & 0xFF80U) != 0) goto ropd_fault;   /* There shouldn't be this many cycles */

 f = 0xD54;
 if ((d[f] & 0xFF80U) != 0) goto ropd_fault;   /* There shouldn't be this many cycles */

 f = 0xD56;
 if (d[f] > 1U) goto ropd_fault;            /* Current audio buffer lo/hi */

 f = 0xD57;
 if (d[f] != 0) goto ropd_fault;            /* Reserved area */

 f = 0xD6D;
 if (d[f] != 0) goto ropd_fault;            /* Reserved area */

 f = 0xD6E;
 if (d[f] > 3U) goto ropd_fault;            /* Interrupt control (video) */

 f = 0xD6F;
 if (d[f] > 3U) goto ropd_fault;            /* Interrupt control (audio) */

 f = 0xD7D;
 if (d[f] != 0) goto ropd_fault;            /* Reserved area */

 for (i = 0; i < 16U; i++){                 /* Kernel tasks */
  if (rrpge_m_taskcheck(d, i)){
   f = 0xD80U + (i << 4);
   goto ropd_fault;
  }
 }

 for (f = 0xEAAU; f < 0xED0U; f++){         /* Reserved areas */
  if (d[f] != 0) goto ropd_fault;
 }

 return RRPGE_ERR_OK;

ropd_fault:

 return RRPGE_ERR_INV + f;
}




/* For compatibility check functions: Checks if app name, author, and the
** major version matches. Returns 1 if they do. */
static auint rrpge_m_checkcomp(rrpge_header_t const* h0, rrpge_header_t const* h1)
{
 auint i;

 for (i = 0U; i < 16U; i++){
  if ((h0->auth[i]) != (h1->auth[i])) return 0;
 }
 for (i = 0U; i < 36U; i++){
  if ((h0->name[i]) != (h1->name[i])) return 0;
 }
 if ((h0->vmaj) != (h1->vmaj)) return 0;

 return 1;
}



/* Check state - app. comp. - Implementation of RRPGE library function */
rrpge_uint32 rrpge_isstatecomp(rrpge_header_t const* sta,
                               rrpge_header_t const* app)
{
 if (!rrpge_m_checkcomp(sta, app)) return 0;
 if ((sta->tp) != (auint)('S'))    return 0;
 if ((sta->vmin) != (app->vmin))   return 0;
 if ((sta->vpat) != (app->vpat))   return 0;
 return 1;
}
