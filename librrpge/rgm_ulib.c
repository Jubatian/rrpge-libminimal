/**
**  \file
**  \brief     RRPGE User Library binary
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.11.06
*/


#include "rgm_ulib.h"


/* The RRPGE User Library */
const uint16 rrpge_m_ulib[RRPGE_M_ULIB_SIZE] = {
 0x8D23U, 0xF0A2U, 0x8D23U, 0xF0A4U, 0x8D23U, 0xF09EU, 0x8D23U, 0xF0A0U,
 0x8D23U, 0xF09AU, 0x8D23U, 0xF09CU, 0x8D23U, 0xF092U, 0x8D23U, 0xF098U,
 0x8D23U, 0xF070U, 0x8D23U, 0xF090U, 0xC000U, 0xC000U, 0xC000U, 0xC000U,
 0x8D23U, 0xF049U, 0x8D23U, 0xF06EU, 0x8D23U, 0xF030U, 0xC000U, 0xC000U,
 0x8D23U, 0xF0EBU, 0x8D23U, 0xF139U, 0x8D23U, 0xF163U, 0x8D23U, 0xF158U,
 0x8D23U, 0xF0A6U, 0x8D23U, 0xF0E3U, 0x8D23U, 0xF194U, 0x8D23U, 0xF1BFU,
 0x03ECU, 0xC000U, 0x47C3U, 0x2BC3U, 0x0BE0U, 0xC020U, 0x02ACU, 0xC001U,
 0x00BBU, 0x02ACU, 0xC002U, 0x00BBU, 0x02ACU, 0xC003U, 0x00BBU, 0x02ACU,
 0xC004U, 0x00BBU, 0x02ACU, 0xC005U, 0x00BBU, 0x0BC1U, 0x02B7U, 0x0BC1U,
 0x8980U, 0x03C4U, 0x122CU, 0xC001U, 0x126CU, 0xC002U, 0x6A44U, 0x3A04U,
 0x132CU, 0xC003U, 0x136CU, 0xC004U, 0x6B44U, 0x3B04U, 0x02B7U, 0x03ECU,
 0xC000U, 0x47C3U, 0x2BC3U, 0x0BE0U, 0xC020U, 0x003BU, 0x007BU, 0x013BU,
 0x017BU, 0x00BBU, 0x0BC1U, 0x02B7U, 0x0BC1U, 0x022CU, 0xC001U, 0x026CU,
 0xC002U, 0x032CU, 0xC003U, 0x036CU, 0xC004U, 0x8980U, 0x03CCU, 0x87DBU,
 0x03C4U, 0x122CU, 0xC001U, 0x126CU, 0xC002U, 0x6A44U, 0x3A04U, 0x02B7U,
 0x03ECU, 0xC000U, 0x47C3U, 0x2BC3U, 0x0BE0U, 0xC020U, 0x003BU, 0x007BU,
 0x0200U, 0x003BU, 0x0201U, 0x0272U, 0x4647U, 0x2A31U, 0x003BU, 0x00BBU,
 0x0BC1U, 0x02B7U, 0x0BC1U, 0x022CU, 0xC001U, 0x026CU, 0xC002U, 0x8980U,
 0x03CCU, 0x87E0U, 0x03C3U, 0x122CU, 0xC001U, 0x126CU, 0xC002U, 0x87E0U,
 0x03CBU, 0x87FAU, 0x03C2U, 0x87F8U, 0x03CAU, 0x87F6U, 0x03C1U, 0x87F4U,
 0x03C9U, 0x87F2U, 0x03C0U, 0x87F0U, 0x03C8U, 0x87EEU, 0x03ECU, 0xC001U,
 0x6BC4U, 0x01E8U, 0xC039U, 0x03ECU, 0xC000U, 0x3BC4U, 0x01E8U, 0xC038U,
 0x03C0U, 0x01E8U, 0xC03AU, 0xA937U, 0x01E8U, 0xC03BU, 0x03C4U, 0x01E8U,
 0xC03CU, 0x03E0U, 0xC03FU, 0x06E0U, 0xC004U, 0x0010U, 0x022CU, 0xC002U,
 0x02ACU, 0xC003U, 0xACB2U, 0x8406U, 0x003BU, 0x003BU, 0x003BU, 0x003BU,
 0x0E84U, 0xAC72U, 0x8404U, 0x003BU, 0x003BU, 0x0E82U, 0xAC32U, 0x8403U,
 0x003BU, 0x0E81U, 0xBA80U, 0x840CU, 0x003BU, 0x003BU, 0x003BU, 0x003BU,
 0x003BU, 0x003BU, 0x003BU, 0x003BU, 0x0E88U, 0xB280U, 0x87F6U, 0x0210U,
 0x06E0U, 0xC006U, 0x8980U, 0x03ECU, 0xC000U, 0x0010U, 0x022CU, 0xC001U,
 0x02ACU, 0xC002U, 0x87D8U, 0x03ECU, 0xC001U, 0x6BC4U, 0x01E8U, 0xC039U,
 0x03ECU, 0xC000U, 0x3BC4U, 0x01E8U, 0xC038U, 0x03C0U, 0x01E8U, 0xC03AU,
 0xA937U, 0x01E8U, 0xC03BU, 0x03C4U, 0x01E8U, 0xC03CU, 0x03ECU, 0xC002U,
 0x02ACU, 0xC003U, 0x0191U, 0x03A0U, 0xC03FU, 0x8012U, 0x8221U, 0xE466U,
 0x0010U, 0xACB2U, 0x840AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU,
 0x003AU, 0x023BU, 0x003AU, 0x0E84U, 0xAC72U, 0x8406U, 0x023BU, 0x003AU,
 0x023BU, 0x003AU, 0x0E82U, 0xAC32U, 0x8404U, 0x023BU, 0x003AU, 0x0E81U,
 0xBA80U, 0x8414U, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU,
 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU,
 0x023BU, 0x003AU, 0x0E88U, 0xB280U, 0x87EEU, 0x0210U, 0x0391U, 0x8212U,
 0x8980U, 0x03ECU, 0xC002U, 0x6BC4U, 0x01E8U, 0xC039U, 0x03ECU, 0xC001U,
 0x3BC4U, 0x01E8U, 0xC038U, 0x03C0U, 0x01E8U, 0xC03AU, 0xA937U, 0x01E8U,
 0xC03BU, 0x03C4U, 0x01E8U, 0xC03CU, 0x03ECU, 0xC000U, 0x02ACU, 0xC003U,
 0x0191U, 0x03A0U, 0xC03FU, 0x13B7U, 0x8012U, 0x8221U, 0xC666U, 0x87B1U,
 0x03ECU, 0xC001U, 0x0191U, 0x03ACU, 0xC000U, 0x02ACU, 0xC002U, 0x8012U,
 0x8221U, 0xE666U, 0x87A6U, 0x03ECU, 0xC003U, 0x6BC4U, 0x01E8U, 0xC039U,
 0x03ECU, 0xC002U, 0x3BC4U, 0x01E8U, 0xC038U, 0x03C0U, 0x01E8U, 0xC03AU,
 0xA937U, 0x01E8U, 0xC03BU, 0x03C4U, 0x01E8U, 0xC03CU, 0x03ECU, 0xC001U,
 0x6BC4U, 0x01E8U, 0xC031U, 0x03ECU, 0xC000U, 0x3BC4U, 0x01E8U, 0xC030U,
 0x03C0U, 0x01E8U, 0xC032U, 0xA937U, 0x01E8U, 0xC033U, 0x03C4U, 0x01E8U,
 0xC034U, 0x02ACU, 0xC004U, 0x0191U, 0x03E0U, 0xC03FU, 0x03A0U, 0xC037U,
 0x8012U, 0x8221U, 0xC466U, 0x8775U, 0x0280U, 0xB8ACU, 0xC004U, 0x841BU,
 0x03E3U, 0xFFF0U, 0x8923U, 0xF024U, 0xC02CU, 0xC000U, 0xC02CU, 0xC001U,
 0xC02CU, 0xC002U, 0xC02CU, 0xC003U, 0xC077U, 0x49ECU, 0xC001U, 0x08ACU,
 0xC000U, 0x49ECU, 0xC003U, 0x08ACU, 0xC002U, 0x4DECU, 0xC005U, 0x08ACU,
 0xC004U, 0x87E3U, 0x8923U, 0xF024U, 0xC02CU, 0xC000U, 0xC02CU, 0xC001U,
 0xC02CU, 0xC002U, 0xC02CU, 0xC003U, 0xC06CU, 0xC005U, 0x8980U, 0x0280U,
 0xB8ACU, 0xC003U, 0x8415U, 0x03E3U, 0xFFF0U, 0x8923U, 0xF028U, 0xC02CU,
 0xC000U, 0xC02CU, 0xC001U, 0xC02CU, 0xC002U, 0xC077U, 0x49ECU, 0xC001U,
 0x08ACU, 0xC000U, 0x4DECU, 0xC004U, 0x08ACU, 0xC003U, 0x87E9U, 0x8923U,
 0xF028U, 0xC02CU, 0xC000U, 0xC02CU, 0xC001U, 0xC02CU, 0xC002U, 0xC06CU,
 0xC004U, 0x8980U
};