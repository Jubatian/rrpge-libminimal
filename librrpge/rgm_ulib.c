/**
**  \file
**  \brief     RRPGE User Library binary
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.02.03
*/


#include "rgm_ulib.h"


/* The RRPGE User Library */
const uint16 rrpge_m_ulib[RRPGE_M_ULIB_SIZE] = {
 0x8D23U, 0xE130U, 0x8D23U, 0xE132U, 0x8D23U, 0xE12CU, 0x8D23U, 0xE12EU,
 0x8D23U, 0xE128U, 0x8D23U, 0xE12AU, 0x8D23U, 0xE122U, 0x8D23U, 0xE126U,
 0x8D23U, 0xE105U, 0x8D23U, 0xE120U, 0xC000U, 0xC000U, 0xC000U, 0xC000U,
 0x8D23U, 0xE0E7U, 0x8D23U, 0xE103U, 0x8D23U, 0xE0D4U, 0xC000U, 0xC000U,
 0x8D23U, 0xE170U, 0x8D23U, 0xE1BAU, 0x8D23U, 0xE1DDU, 0x8D23U, 0xE1D5U,
 0x8D23U, 0xE134U, 0x8D23U, 0xE16BU, 0x8D23U, 0xE209U, 0x8D23U, 0xE21FU,
 0x8D23U, 0xE232U, 0x8D23U, 0xE237U, 0x8D23U, 0xE35BU, 0x8D23U, 0xE38AU,
 0x8D23U, 0xE3D8U, 0x8D23U, 0xE402U, 0x8D23U, 0xE43DU, 0x8D23U, 0xE48CU,
 0x8D23U, 0xE23EU, 0x8D23U, 0xE26CU, 0x8D23U, 0xE30DU, 0x8D23U, 0xE312U,
 0x8D23U, 0xE319U, 0x8D23U, 0xE321U, 0x8D23U, 0xE328U, 0x8D23U, 0xE32FU,
 0x8D23U, 0xE28FU, 0x8D23U, 0xE2A7U, 0x8D23U, 0xE334U, 0x8D23U, 0xE339U,
 0x8D23U, 0xE340U, 0x8D23U, 0xE348U, 0x8D23U, 0xE34FU, 0x8D23U, 0xE356U,
 0x8D23U, 0xE2B8U, 0x8D23U, 0xE2C5U, 0x8D23U, 0xE2DFU, 0x8D23U, 0xE2ECU,
 0x8D23U, 0xE2F6U, 0x8D23U, 0xE303U, 0x8D23U, 0xE49FU, 0x8D23U, 0xE505U,
 0x8D23U, 0xE4B8U, 0x8D23U, 0xE53DU, 0x8D23U, 0xE4C1U, 0x8D23U, 0xE546U,
 0x8D23U, 0xE4DBU, 0x8D23U, 0xE5C0U, 0x8D23U, 0xE4DEU, 0x8D23U, 0xE5F3U,
 0x8D23U, 0xE692U, 0x8D23U, 0xE694U, 0x8D23U, 0xE6BBU, 0x8D23U, 0xE6C4U,
 0x8D23U, 0xE66DU, 0x8D23U, 0xE677U, 0x8D23U, 0xE6E2U, 0x8D23U, 0xE6EBU,
 0x8D23U, 0xE844U, 0x8D23U, 0xE876U, 0x8D23U, 0xE908U, 0x8D23U, 0xE918U,
 0x8D23U, 0xE92AU, 0x8D23U, 0xE93BU, 0x8D23U, 0xE94EU, 0x8D23U, 0xE959U,
 0x8D23U, 0xE97BU, 0xC000U, 0xC000U, 0x8D23U, 0xE983U, 0x8D23U, 0xE986U,
 0x8D23U, 0xE98AU, 0x8D23U, 0xE999U, 0x8D23U, 0xE9E2U, 0x8D23U, 0xEA5CU,
 0x8D23U, 0xEA67U, 0x8D23U, 0xEA86U, 0x8D23U, 0xEA72U, 0x8D23U, 0xEA91U,
 0x8D23U, 0xEAC7U, 0x8D23U, 0xEACFU, 0x8D23U, 0xEAD4U, 0x8D23U, 0xEBB2U,
 0x8D23U, 0xEBB8U, 0x8D23U, 0xEBBEU, 0x8D23U, 0xEBDCU, 0x8D23U, 0xEBFBU,
 0x8D23U, 0xEC17U, 0x8D23U, 0xEC2DU, 0x8D23U, 0xEC31U, 0x8D23U, 0xEC55U,
 0x8D23U, 0xEC5BU, 0x8D23U, 0xEC7DU, 0x03D0U, 0x47C3U, 0x2BC3U, 0x0BE0U,
 0xC020U, 0x0291U, 0x00BBU, 0x0292U, 0x00BBU, 0x0293U, 0x00BBU, 0x0294U,
 0x00BBU, 0x0295U, 0x00BBU, 0x0BC1U, 0x02B7U, 0x0BC1U, 0x8980U, 0x03C4U,
 0x1211U, 0x1252U, 0x6A44U, 0x3A04U, 0x1313U, 0x1354U, 0x6B44U, 0x3B04U,
 0x02B7U, 0x03D0U, 0x47C3U, 0x2BC3U, 0x0BE0U, 0xC020U, 0x003BU, 0x007BU,
 0x013BU, 0x017BU, 0x00BBU, 0x0BC1U, 0x02B7U, 0x0BC1U, 0x0211U, 0x0252U,
 0x0313U, 0x0354U, 0x8980U, 0x03CCU, 0x87E4U, 0x03C4U, 0x1211U, 0x1252U,
 0x6A44U, 0x3A04U, 0x02B7U, 0x03D0U, 0x47C3U, 0x2BC3U, 0x0BE0U, 0xC020U,
 0x003BU, 0x007BU, 0x0200U, 0x003BU, 0x0201U, 0x0272U, 0x4647U, 0x2A31U,
 0x003BU, 0x00BBU, 0x0BC1U, 0x02B7U, 0x0BC1U, 0x0211U, 0x0252U, 0x8980U,
 0x03CCU, 0x87E5U, 0x03C3U, 0x1211U, 0x1252U, 0x87E5U, 0x03CBU, 0x87FCU,
 0x03C2U, 0x87FAU, 0x03CAU, 0x87F8U, 0x03C1U, 0x87F6U, 0x03C9U, 0x87F4U,
 0x03C0U, 0x87F2U, 0x03C8U, 0x87F0U, 0x03D1U, 0x6BC4U, 0x01E8U, 0xC039U,
 0x03D0U, 0x3BC4U, 0x01E8U, 0xC038U, 0x03C0U, 0x01E8U, 0xC03AU, 0xA937U,
 0x01E8U, 0xC03BU, 0x03C4U, 0x01E8U, 0xC03CU, 0x03E0U, 0xC03FU, 0x06C4U,
 0x0010U, 0x0212U, 0x0293U, 0xACB2U, 0x8406U, 0x003BU, 0x003BU, 0x003BU,
 0x003BU, 0x0E84U, 0xAC72U, 0x8404U, 0x003BU, 0x003BU, 0x0E82U, 0xAC32U,
 0x8403U, 0x003BU, 0x0E81U, 0xBA80U, 0x840CU, 0x003BU, 0x003BU, 0x003BU,
 0x003BU, 0x003BU, 0x003BU, 0x003BU, 0x003BU, 0x0E88U, 0xB280U, 0x87F6U,
 0x0210U, 0x06C6U, 0x8980U, 0x03D0U, 0x0010U, 0x0211U, 0x0292U, 0x87DCU,
 0x03D1U, 0x6BC4U, 0x01E8U, 0xC039U, 0x03D0U, 0x3BC4U, 0x01E8U, 0xC038U,
 0x03C0U, 0x01E8U, 0xC03AU, 0xA937U, 0x01E8U, 0xC03BU, 0x03C4U, 0x01E8U,
 0xC03CU, 0x03D2U, 0x0293U, 0x0191U, 0x03A0U, 0xC03FU, 0x8012U, 0x8221U,
 0xE466U, 0x0010U, 0xACB2U, 0x840AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU,
 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x0E84U, 0xAC72U, 0x8406U, 0x023BU,
 0x003AU, 0x023BU, 0x003AU, 0x0E82U, 0xAC32U, 0x8404U, 0x023BU, 0x003AU,
 0x0E81U, 0xBA80U, 0x8414U, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU,
 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU,
 0x003AU, 0x023BU, 0x003AU, 0x0E88U, 0xB280U, 0x87EEU, 0x0210U, 0x0391U,
 0x8212U, 0x8980U, 0x03D2U, 0x6BC4U, 0x01E8U, 0xC039U, 0x03D1U, 0x3BC4U,
 0x01E8U, 0xC038U, 0x03C0U, 0x01E8U, 0xC03AU, 0xA937U, 0x01E8U, 0xC03BU,
 0x03C4U, 0x01E8U, 0xC03CU, 0x03D0U, 0x0293U, 0x0191U, 0x03A0U, 0xC03FU,
 0x13B7U, 0x8012U, 0x8221U, 0xC666U, 0x87B5U, 0x03D1U, 0x0191U, 0x0390U,
 0x0292U, 0x8012U, 0x8221U, 0xE666U, 0x87ADU, 0x03D3U, 0x6BC4U, 0x01E8U,
 0xC039U, 0x03D2U, 0x3BC4U, 0x01E8U, 0xC038U, 0x03C0U, 0x01E8U, 0xC03AU,
 0xA937U, 0x01E8U, 0xC03BU, 0x03C4U, 0x01E8U, 0xC03CU, 0x03D1U, 0x6BC4U,
 0x01E8U, 0xC031U, 0x03D0U, 0x3BC4U, 0x01E8U, 0xC030U, 0x03C0U, 0x01E8U,
 0xC032U, 0xA937U, 0x01E8U, 0xC033U, 0x03C4U, 0x01E8U, 0xC034U, 0x0294U,
 0x0191U, 0x03E0U, 0xC03FU, 0x03A0U, 0xC037U, 0x8012U, 0x8221U, 0xC466U,
 0x8781U, 0x0280U, 0xB894U, 0x8411U, 0x03E3U, 0xFFF0U, 0x8923U, 0xE1DDU,
 0xC010U, 0xC011U, 0xC012U, 0xC013U, 0xC077U, 0x49D1U, 0x0890U, 0x49D3U,
 0x0892U, 0x4DD5U, 0x0894U, 0x87EEU, 0x0295U, 0x0094U, 0x87BFU, 0x0280U,
 0xB893U, 0x840EU, 0x03E3U, 0xFFF0U, 0x8923U, 0xE134U, 0xC010U, 0xC011U,
 0xC012U, 0xC077U, 0x49D1U, 0x0890U, 0x4DD4U, 0x0893U, 0x87F1U, 0x0294U,
 0x0093U, 0x8703U, 0x03C6U, 0x69D1U, 0x39D0U, 0x0292U, 0x8409U, 0x8923U,
 0xE23EU, 0xC050U, 0x47E0U, 0xC7FCU, 0x6BCAU, 0x8980U, 0x0290U, 0x4683U,
 0x27C3U, 0x2BF2U, 0x47E0U, 0xC7FCU, 0x47D0U, 0x43F2U, 0x8980U, 0x8281U,
 0x03E3U, 0xFAF0U, 0x8405U, 0x02BBU, 0x01D0U, 0x8972U, 0x03D0U, 0xB3EBU,
 0xFAEFU, 0x87FAU, 0xA82BU, 0xFADEU, 0x8980U, 0x8281U, 0xA02BU, 0xFADEU,
 0x03E3U, 0xFAE0U, 0x8405U, 0x02BBU, 0x01D0U, 0x8972U, 0x03D0U, 0xB3EBU,
 0xFAEEU, 0x87FAU, 0x8980U, 0x8923U, 0xE040U, 0xC050U, 0x02A8U, 0xC017U,
 0x46A0U, 0xF000U, 0x43F2U, 0x8980U, 0x8923U, 0xE263U, 0xC050U, 0x01E8U,
 0xC017U, 0x01EBU, 0xFAFFU, 0x47C3U, 0x2683U, 0x4491U, 0x41D1U, 0x8923U,
 0xE263U, 0xC051U, 0x01EBU, 0xFAFEU, 0x03E3U, 0xFAD0U, 0x8405U, 0x02BBU,
 0x01D0U, 0x8972U, 0x03D0U, 0xB3EBU, 0xFADFU, 0x87FAU, 0x8963U, 0xE247U,
 0xA7E8U, 0xC017U, 0x87FEU, 0x03D2U, 0x01E8U, 0xC016U, 0x8419U, 0xA42BU,
 0xFADEU, 0x8963U, 0xE255U, 0x8406U, 0x8880U, 0xC020U, 0xC500U, 0xC060U,
 0xD388U, 0xA428U, 0xC00DU, 0x87F9U, 0x8923U, 0xE263U, 0xC06BU, 0xFAFFU,
 0x13EBU, 0xFAFEU, 0x01E8U, 0xC017U, 0x01EBU, 0xFAFFU, 0x87A1U, 0xA42BU,
 0xFADEU, 0x8409U, 0x03EBU, 0xFAFEU, 0x8980U, 0x8880U, 0xC020U, 0xC500U,
 0xC060U, 0xD388U, 0xA7E8U, 0xC017U, 0x87F9U, 0x8963U, 0xE255U, 0x87F3U,
 0x8923U, 0xE2C5U, 0xC050U, 0x03EBU, 0xFAEFU, 0xBBE3U, 0xFAFEU, 0x8405U,
 0x0290U, 0x00BBU, 0x01EBU, 0xFAEFU, 0x8980U, 0x8282U, 0x0290U, 0x8010U,
 0x0191U, 0x0684U, 0x03A3U, 0xFAEFU, 0x03E3U, 0xFAF0U, 0x8403U, 0xBABBU,
 0x8408U, 0xB3FAU, 0x87FDU, 0x8409U, 0x02BBU, 0x0FC2U, 0x00BBU, 0x0BC1U,
 0xB3FAU, 0x87FBU, 0x0FC1U, 0x01FAU, 0x8210U, 0x0391U, 0x8980U, 0x8923U,
 0xE2ECU, 0xC050U, 0x03EBU, 0xFAEEU, 0xBBE3U, 0xFAEEU, 0x8405U, 0x0290U,
 0x00BBU, 0x01EBU, 0xFAEEU, 0x8980U, 0x8282U, 0x0290U, 0x8010U, 0x0191U,
 0x0684U, 0x03A3U, 0xFAEEU, 0x03E3U, 0xFAE0U, 0x87DCU, 0x8923U, 0xE303U,
 0xC050U, 0x03EBU, 0xFADFU, 0xBBE3U, 0xFADEU, 0x8405U, 0x0290U, 0x00BBU,
 0x01EBU, 0xFADFU, 0x8980U, 0x8282U, 0x0290U, 0x8010U, 0x0191U, 0x0684U,
 0x03A3U, 0xFADFU, 0x03E3U, 0xFAD0U, 0x87C5U, 0x8283U, 0x03E8U, 0xC017U,
 0x01D2U, 0x844AU, 0x8286U, 0x0294U, 0x0095U, 0x03E8U, 0xC017U, 0x01D4U,
 0x8472U, 0x8287U, 0x0294U, 0x1295U, 0x0096U, 0x03E8U, 0xC017U, 0x01D4U,
 0x84B8U, 0x8285U, 0x0293U, 0x0094U, 0x03E8U, 0xC017U, 0x01D3U, 0x84DBU,
 0x8286U, 0x0294U, 0x0095U, 0x03E8U, 0xC017U, 0x01D4U, 0x850FU, 0x8281U,
 0x03E8U, 0xC017U, 0x01D0U, 0x8559U, 0x8283U, 0x8963U, 0xE2A7U, 0x01D2U,
 0x8423U, 0x8286U, 0x0294U, 0x0095U, 0x8963U, 0xE2A7U, 0x01D4U, 0x844BU,
 0x8287U, 0x0294U, 0x1295U, 0x0096U, 0x8963U, 0xE2A7U, 0x01D4U, 0x8491U,
 0x8285U, 0x0293U, 0x0094U, 0x8963U, 0xE2A7U, 0x01D3U, 0x84B4U, 0x8286U,
 0x0294U, 0x0095U, 0x8963U, 0xE2A7U, 0x01D4U, 0x84E8U, 0x8281U, 0x8963U,
 0xE2A7U, 0x01D0U, 0x8532U, 0x8287U, 0x0013U, 0x0054U, 0x00D5U, 0x0116U,
 0x0212U, 0x4603U, 0x26C3U, 0x2AF0U, 0x46E0U, 0xC7FCU, 0xA752U, 0x0A01U,
 0x0A07U, 0x6811U, 0x0272U, 0x0290U, 0x2A85U, 0x0891U, 0x46D2U, 0x6ACEU,
 0x0332U, 0x4AD1U, 0x1B31U, 0x0241U, 0x6A70U, 0x0204U, 0x03E0U, 0xC030U,
 0x013BU, 0x00FBU, 0x00BBU, 0x007BU, 0x003BU, 0x0BC3U, 0xA933U, 0x013BU,
 0x00FBU, 0x00BBU, 0x007BU, 0x003BU, 0x03F1U, 0x0213U, 0x0254U, 0x02D5U,
 0x0316U, 0x8980U, 0x8289U, 0x0017U, 0x00D8U, 0x03D0U, 0x2FCCU, 0x47C7U,
 0x0BE0U, 0xC018U, 0x02FBU, 0xA533U, 0x02C0U, 0x46CFU, 0x2AC1U, 0x0AC1U,
 0x00D6U, 0x03E0U, 0xC0C8U, 0xAF54U, 0x2BC1U, 0xAFD5U, 0x8409U, 0x0215U,
 0x0812U, 0xA7D2U, 0x8433U, 0x2216U, 0x0C10U, 0x0200U, 0x0015U, 0xBFD5U,
 0x842DU, 0x0237U, 0x0E15U, 0xBE12U, 0x0012U, 0x8923U, 0xE35BU, 0xC013U,
 0xC015U, 0xC054U, 0x2683U, 0x0203U, 0x4692U, 0x4612U, 0x03E0U, 0xC016U,
 0x0FF0U, 0x2A02U, 0x0FF0U, 0x0210U, 0x02D1U, 0x8C37U, 0x0E84U, 0x0028U,
 0xC037U, 0x00E8U, 0xC03FU, 0x0A16U, 0x0028U, 0xC037U, 0x00E8U, 0xC03FU,
 0x0A16U, 0x0028U, 0xC037U, 0x00E8U, 0xC03FU, 0x0A16U, 0x0028U, 0xC037U,
 0x00E8U, 0xC03FU, 0x0A16U, 0xB280U, 0x87EAU, 0x0217U, 0x02D8U, 0x8980U,
 0x8289U, 0x0017U, 0x00D8U, 0x0216U, 0x1215U, 0x03D0U, 0x2FCCU, 0x47C7U,
 0x0BE0U, 0xC018U, 0x02FBU, 0x46CFU, 0x2AC1U, 0x4AC1U, 0x00D6U, 0xA714U,
 0x0281U, 0x03E0U, 0xC280U, 0x2FF2U, 0xBFF0U, 0x87E8U, 0xAFF0U, 0x840BU,
 0x03D0U, 0x2BC5U, 0x47C7U, 0x1FE3U, 0xFFFDU, 0x02D6U, 0x2AF7U, 0x0AF0U,
 0xB6C0U, 0x87DCU, 0x2A32U, 0x4620U, 0xC3FFU, 0x02E3U, 0xFC00U, 0x44D1U,
 0x4011U, 0x8798U, 0x8287U, 0x0015U, 0x00D6U, 0x03E0U, 0xC0C8U, 0xAF53U,
 0x2BC1U, 0xAFD4U, 0x8407U, 0x0214U, 0x0812U, 0xA7D2U, 0x842CU, 0x0200U,
 0x0014U, 0xBFD4U, 0x8428U, 0x0237U, 0x0E14U, 0xBE12U, 0x0012U, 0x8923U,
 0xE35BU, 0xC000U, 0xC014U, 0xC053U, 0x2683U, 0x0203U, 0x4692U, 0x4612U,
 0x03E0U, 0xC012U, 0x2A02U, 0x0FF0U, 0x0210U, 0x02D1U, 0x8C37U, 0x0E84U,
 0x0028U, 0xC037U, 0x00E8U, 0xC03FU, 0x0028U, 0xC037U, 0x00E8U, 0xC03FU,
 0x0028U, 0xC037U, 0x00E8U, 0xC03FU, 0x0028U, 0xC037U, 0x00E8U, 0xC03FU,
 0xB280U, 0x87EEU, 0x0215U, 0x02D6U, 0x8980U, 0x8287U, 0x0016U, 0x03E0U,
 0xC0C8U, 0xAF54U, 0x2BC1U, 0xAFD5U, 0x840AU, 0x0215U, 0x0812U, 0xA7D2U,
 0x8442U, 0x2A01U, 0x4C11U, 0x0C90U, 0x0200U, 0x0015U, 0xBFD5U, 0x843BU,
 0x0237U, 0x0E15U, 0xBE12U, 0x0012U, 0x8923U, 0xE35BU, 0xC013U, 0xC015U,
 0xC054U, 0x8923U, 0xE105U, 0xC001U, 0xC010U, 0xC051U, 0x2683U, 0x0203U,
 0x4692U, 0x4612U, 0x03E0U, 0xC022U, 0x2A03U, 0x0FF0U, 0x8C37U, 0x0E84U,
 0x0228U, 0xC02FU, 0x0028U, 0xC037U, 0x0228U, 0xC02FU, 0x0028U, 0xC03FU,
 0x0228U, 0xC02FU, 0x0028U, 0xC037U, 0x0228U, 0xC02FU, 0x0028U, 0xC03FU,
 0x0228U, 0xC02FU, 0x0028U, 0xC037U, 0x0228U, 0xC02FU, 0x0028U, 0xC03FU,
 0x0228U, 0xC02FU, 0x0028U, 0xC037U, 0x0228U, 0xC02FU, 0x0028U, 0xC03FU,
 0xB280U, 0x87DEU, 0x0216U, 0x8980U, 0x0290U, 0x4683U, 0x27C3U, 0x2BF2U,
 0x47E0U, 0xC7FCU, 0x45D0U, 0x03E0U, 0xCC80U, 0x2BF2U, 0x028AU, 0x6890U,
 0x8923U, 0xE134U, 0xC032U, 0xC010U, 0xC000U, 0xC077U, 0x8980U, 0xA42BU,
 0xFACCU, 0x8980U, 0xA82BU, 0xFACCU, 0x02A8U, 0xC017U, 0x03C4U, 0xA772U,
 0x03C8U, 0x4683U, 0x2BF2U, 0x02ABU, 0xFACBU, 0xBFF2U, 0x02B7U, 0x00ABU,
 0xFAC8U, 0x0AABU, 0xFACAU, 0xBFF2U, 0x02B7U, 0x00ABU, 0xFAC9U, 0x8980U,
 0x0290U, 0x00ABU, 0xFACBU, 0x0291U, 0x00ABU, 0xFACAU, 0xA02BU, 0xFACCU,
 0x8980U, 0x03E3U, 0xE339U, 0xA02BU, 0xFACCU, 0xAC13U, 0x840BU, 0x02ABU,
 0xFAC9U, 0xBAABU, 0xFAC8U, 0x8980U, 0x0E81U, 0x0093U, 0x00ABU, 0xFAC9U,
 0x8D37U, 0x02ABU, 0xFAC8U, 0xBAABU, 0xFAC9U, 0x8980U, 0x0093U, 0x0A81U,
 0x00ABU, 0xFAC8U, 0x8D37U, 0x03E3U, 0xE340U, 0x87E6U, 0x03E3U, 0xE34FU,
 0x87E3U, 0x8285U, 0x0012U, 0x0053U, 0x00D4U, 0x0211U, 0x4603U, 0x26C3U,
 0x2AF0U, 0x46E0U, 0xC7FCU, 0xA751U, 0x0A01U, 0x0A07U, 0x6810U, 0x0272U,
 0x46D1U, 0x6ACEU, 0x0A72U, 0x4AD0U, 0x0A72U, 0x0010U, 0x0280U, 0x0204U,
 0x03E0U, 0xC038U, 0x007BU, 0x00FBU, 0x00BBU, 0x00BBU, 0x003BU, 0x03C1U,
 0x2BD0U, 0x0212U, 0x0253U, 0x02D4U, 0x8980U, 0xA42BU, 0xFACDU, 0x8980U,
 0xA82BU, 0xFACDU, 0x8284U, 0x0010U, 0x00D1U, 0x0192U, 0x8013U, 0x02A8U,
 0xC017U, 0x03C4U, 0xA772U, 0x03C8U, 0x4683U, 0x2BF2U, 0x022BU, 0xFACFU,
 0x02B0U, 0xBFF0U, 0x0237U, 0x03B0U, 0x2B88U, 0x4236U, 0x0AABU, 0xFACEU,
 0xBFF2U, 0x02B7U, 0x03B2U, 0x2B88U, 0x42B6U, 0x8221U, 0xE666U, 0x03A3U,
 0xF800U, 0x03E3U, 0xF8C8U, 0x02E0U, 0xC0C8U, 0x0AF6U, 0x003AU, 0x00BBU,
 0x003AU, 0x00BBU, 0x003AU, 0x00BBU, 0x003AU, 0x00BBU, 0xB3B3U, 0x87F7U,
 0x0210U, 0x02D1U, 0x0392U, 0x8213U, 0x8980U, 0x0290U, 0x00ABU, 0xFACFU,
 0x0291U, 0x00ABU, 0xFACEU, 0xA02BU, 0xFACDU, 0x8980U, 0x828FU, 0x0017U,
 0x0058U, 0x00D9U, 0x011AU, 0x015BU, 0x019CU, 0x801DU, 0x805EU, 0x8963U,
 0xE2A7U, 0x01D6U, 0x03D0U, 0x2FCCU, 0x47C7U, 0x0BE0U, 0xC018U, 0x02FBU,
 0x46CFU, 0x2AC1U, 0x0AC1U, 0x00D5U, 0x03E0U, 0xC0C8U, 0xAF56U, 0x2BC1U,
 0xAFD4U, 0x8409U, 0x0214U, 0x0812U, 0xA7D2U, 0x8452U, 0x2215U, 0x0C10U,
 0x0200U, 0x0014U, 0xBFD4U, 0x844CU, 0x0237U, 0x0E14U, 0xBE12U, 0x0012U,
 0xBA00U, 0x8446U, 0xA02BU, 0xFACDU, 0x8923U, 0xE4E1U, 0xC014U, 0xC056U,
 0x8260U, 0xD111U, 0x0323U, 0xF000U, 0x0B14U, 0x0374U, 0x0B60U, 0xC190U,
 0x0210U, 0x0251U, 0x0912U, 0x02F7U, 0x03A0U, 0xC039U, 0x03E0U, 0xC03FU,
 0xA413U, 0x841AU, 0x8221U, 0xC48CU, 0x02B8U, 0xBAB9U, 0x8411U, 0x2A85U,
 0x10BAU, 0x08BAU, 0x003BU, 0xA93AU, 0x007BU, 0x00BAU, 0x0281U, 0x08B8U,
 0x48FAU, 0x08A8U, 0xC038U, 0x0A15U, 0xB312U, 0x87EFU, 0x8419U, 0x0B01U,
 0x87F8U, 0x0B41U, 0x840FU, 0x8221U, 0xC4C8U, 0x02B9U, 0xBAB8U, 0x87FAU,
 0x0E81U, 0x00B9U, 0x2A85U, 0x10BAU, 0x08BAU, 0x003BU, 0xA93AU, 0x007BU,
 0x00BAU, 0x48FAU, 0x08A8U, 0xC038U, 0x0A15U, 0xB312U, 0x87EFU, 0x0217U,
 0x0258U, 0x02D9U, 0x031AU, 0x035BU, 0x039CU, 0x821DU, 0x825EU, 0x8980U,
 0x828FU, 0x0017U, 0x0058U, 0x00D9U, 0x011AU, 0x015BU, 0x019CU, 0x801DU,
 0x805EU, 0x0215U, 0x1214U, 0x8963U, 0xE2A7U, 0x01D6U, 0x03D0U, 0x2FCCU,
 0x47C7U, 0x0BE0U, 0xC018U, 0x02FBU, 0x46CFU, 0x2AC1U, 0x4AC1U, 0x00D5U,
 0xA716U, 0x0281U, 0x03E0U, 0xC280U, 0x2FF2U, 0xBFF0U, 0x87D9U, 0xAFF0U,
 0x840BU, 0x03D0U, 0x2BC5U, 0x47C7U, 0x1FE3U, 0xFFFDU, 0x02D5U, 0x2AF7U,
 0x0AF0U, 0xB6C0U, 0x87CDU, 0x2A32U, 0x4620U, 0xC3FFU, 0x02E3U, 0xFC00U,
 0x44D1U, 0x4011U, 0x876AU, 0x828EU, 0x0016U, 0x0057U, 0x00D8U, 0x0119U,
 0x015AU, 0x019BU, 0x801CU, 0x805DU, 0x8963U, 0xE2A7U, 0x01D5U, 0x03E0U,
 0xC0C8U, 0xAF55U, 0x2BC1U, 0xAFD4U, 0x840AU, 0x0214U, 0x0812U, 0xA7D2U,
 0x845CU, 0x2A01U, 0x4C11U, 0x0C90U, 0x0200U, 0x0014U, 0xBFD4U, 0x8455U,
 0x0237U, 0x0E14U, 0xBE12U, 0x0012U, 0xBA00U, 0x844FU, 0xA02BU, 0xFACDU,
 0x8923U, 0xE4E1U, 0xC014U, 0xC055U, 0x8923U, 0xE105U, 0xC002U, 0xC010U,
 0xC051U, 0x8260U, 0xD111U, 0x0323U, 0xF000U, 0x0B14U, 0x0374U, 0x0B60U,
 0xC190U, 0x0912U, 0x02F7U, 0x03A0U, 0xC039U, 0x03E0U, 0xC03FU, 0xA413U,
 0x841DU, 0x8221U, 0xC48CU, 0x0228U, 0xC037U, 0x0268U, 0xC037U, 0x02B8U,
 0xBAB9U, 0x8410U, 0x2A85U, 0x10BAU, 0x08BAU, 0x003BU, 0xA93AU, 0x007BU,
 0x00BAU, 0x0281U, 0x08B8U, 0x48FAU, 0x08A8U, 0xC038U, 0xB312U, 0x87ECU,
 0x841CU, 0x0B01U, 0x87F9U, 0x0B41U, 0x8413U, 0x8221U, 0xC4C8U, 0x0228U,
 0xC037U, 0x0268U, 0xC037U, 0x02B9U, 0xBAB8U, 0x87F6U, 0x0E81U, 0x00B9U,
 0x2A85U, 0x10BAU, 0x08BAU, 0x003BU, 0xA93AU, 0x007BU, 0x00BAU, 0x48FAU,
 0x08A8U, 0xC038U, 0xB312U, 0x87ECU, 0x0216U, 0x0257U, 0x02D8U, 0x0319U,
 0x035AU, 0x039BU, 0x821CU, 0x825DU, 0x8980U, 0x03D1U, 0x63D3U, 0x11D1U,
 0x33D2U, 0x11D3U, 0x23D0U, 0x0BD3U, 0x02B7U, 0x03D1U, 0x8980U, 0x0292U,
 0xB280U, 0x8404U, 0x0293U, 0xBA81U, 0x8413U, 0x8923U, 0xE08EU, 0xC012U,
 0xC053U, 0x01D3U, 0x0092U, 0x63D1U, 0x03D2U, 0x73D1U, 0x12B7U, 0x13D3U,
 0x73D0U, 0x03D2U, 0x73D0U, 0x0092U, 0x1BD3U, 0x0A92U, 0x8980U, 0x03D1U,
 0x0290U, 0x8980U, 0x03D0U, 0x8404U, 0x03E1U, 0xC000U, 0x0BD0U, 0x6FC7U,
 0x43E3U, 0xFE00U, 0xBA80U, 0x8412U, 0xBBE3U, 0xFF00U, 0x8411U, 0xBBE3U,
 0xFFFFU, 0x8414U, 0x0090U, 0x02BBU, 0x03FBU, 0x12B7U, 0x6290U, 0x1290U,
 0x66B2U, 0x63F2U, 0x0A90U, 0x03F2U, 0x8980U, 0x03FBU, 0x8980U, 0x0BC1U,
 0x03FBU, 0x67F7U, 0x63F2U, 0x67F2U, 0x8980U, 0x03FBU, 0x67F7U, 0x66B2U,
 0x63F2U, 0x67F2U, 0x8980U, 0x8923U, 0xE694U, 0xC050U, 0x11D0U, 0x8923U,
 0xE692U, 0xC077U, 0x0290U, 0x8980U, 0x03D0U, 0x6FC8U, 0x2BC1U, 0x43E3U,
 0xFC00U, 0xBA80U, 0x8415U, 0x8284U, 0x0013U, 0x6490U, 0x023BU, 0x02BBU,
 0x6290U, 0x7210U, 0x1090U, 0x6491U, 0x02BBU, 0x03FBU, 0x13F2U, 0x6291U,
 0x73D1U, 0x0092U, 0x1BF0U, 0x0A90U, 0x0A92U, 0x0213U, 0x8980U, 0x02BBU,
 0x03FBU, 0x8980U, 0x27C0U, 0x17D0U, 0x6690U, 0x22B7U, 0xB290U, 0x8980U,
 0x0280U, 0x0BC1U, 0x8980U, 0x8286U, 0x1050U, 0x1011U, 0x0114U, 0x0155U,
 0xB240U, 0x8407U, 0xBE20U, 0xC100U, 0x840DU, 0xBE21U, 0xC000U, 0x8432U,
 0xBE60U, 0xD000U, 0x8468U, 0xBE62U, 0xC000U, 0x8409U, 0x0280U, 0x03C1U,
 0x853FU, 0xBC02U, 0x8414U, 0x0280U, 0x03C1U, 0x853AU, 0x0371U, 0x0B41U,
 0x27C0U, 0x17F5U, 0x6770U, 0x6731U, 0xB200U, 0x0F01U, 0x6377U, 0x3337U,
 0x4F70U, 0x5F31U, 0x0A81U, 0x0BF2U, 0x0280U, 0x852AU, 0x2640U, 0x1670U,
 0x6771U, 0x2370U, 0xB370U, 0x8403U, 0x0A41U, 0x0340U, 0x2B48U, 0x5770U,
 0x0332U, 0x2B08U, 0x6B48U, 0x0A72U, 0x03F5U, 0x1730U, 0x4BF4U, 0x0AB1U,
 0x8517U, 0x2640U, 0x1670U, 0x6771U, 0x2370U, 0xB370U, 0x8403U, 0x0A41U,
 0x0340U, 0x2B42U, 0x5770U, 0x0332U, 0x2B02U, 0x6B4EU, 0x0A72U, 0x03F5U,
 0x5730U, 0x0372U, 0x2B42U, 0x6B0CU, 0x0A72U, 0x4BF4U, 0x0A72U, 0x5770U,
 0x0332U, 0x2B02U, 0x6B4AU, 0x0A72U, 0x4BF5U, 0x0A72U, 0x5730U, 0x0372U,
 0x2B42U, 0x2B08U, 0x4BF4U, 0x0A72U, 0x5770U, 0x0332U, 0x2B02U, 0x2B46U,
 0x4BF5U, 0x0A72U, 0x5730U, 0x0372U, 0x2B42U, 0x2B04U, 0x4BF4U, 0x0A72U,
 0x5770U, 0x0332U, 0x2B02U, 0x2B42U, 0x4BF5U, 0x0A72U, 0x1730U, 0x4BF4U,
 0x0AB1U, 0x84DEU, 0x03F0U, 0x0331U, 0x6F03U, 0x3FC3U, 0x0340U, 0xBFE0U,
 0xE3A6U, 0x8430U, 0xBFE1U, 0xF071U, 0x8408U, 0xBFE2U, 0xC59EU, 0x8402U,
 0x8476U, 0x0360U, 0xCF53U, 0x8478U, 0xBFE0U, 0xFC88U, 0x8412U, 0xBFE1U,
 0xD16EU, 0x8408U, 0xBFE1U, 0xDF59U, 0x0360U, 0xC344U, 0x0B60U, 0xD236U,
 0x846BU, 0xBFE1U, 0xC5FFU, 0x0360U, 0xC41BU, 0x0B60U, 0xD926U, 0x8464U,
 0xBFE0U, 0xEE04U, 0x8408U, 0xBFE0U, 0xF4A8U, 0x0360U, 0xC510U, 0x0B60U,
 0xE1D4U, 0x845AU, 0xBFE0U, 0xE86BU, 0x0360U, 0xC62AU, 0x0B60U, 0xEC80U,
 0x8453U, 0xBFE0U, 0xCEEEU, 0x8426U, 0xBFE0U, 0xD678U, 0x8412U, 0xBFE0U,
 0xDC19U, 0x8408U, 0xBFE0U, 0xDF98U, 0x0360U, 0xC75FU, 0x0B60U, 0xF971U,
 0x8443U, 0xBFE0U, 0xD918U, 0x0360U, 0xC8B9U, 0x0B61U, 0xC8E1U, 0x843CU,
 0xBFE0U, 0xD235U, 0x8408U, 0xBFE0U, 0xD436U, 0x0360U, 0xCA2EU, 0x0B61U,
 0xDB22U, 0x8432U, 0xBFE0U, 0xD074U, 0x0360U, 0xCBFCU, 0x0B61U, 0xF076U,
 0x842BU, 0xBFE0U, 0xCA68U, 0x8412U, 0xBFE0U, 0xCC62U, 0x8408U, 0xBFE0U,
 0xCD94U, 0x0360U, 0xCDA5U, 0x0B62U, 0xC925U, 0x841EU, 0xBFE0U, 0xCB56U,
 0x0360U, 0xCF44U, 0x0B62U, 0xE558U, 0x8417U, 0xBFE0U, 0xC8D3U, 0x8408U,
 0xBFE0U, 0xC98FU, 0x0360U, 0xD171U, 0x0B63U, 0xC4BDU, 0x840DU, 0xBFE0U,
 0xC826U, 0x0360U, 0xD33CU, 0x0B63U, 0xE7FFU, 0x8406U, 0x03F1U, 0x0BC1U,
 0x2740U, 0x1777U, 0x2F42U, 0x6A03U, 0x3A43U, 0x0052U, 0x0013U, 0x6E41U,
 0x3E01U, 0x6671U, 0x6630U, 0xB200U, 0x0E41U, 0x03F0U, 0x63F5U, 0x03F1U,
 0x33F5U, 0x63F5U, 0x0B72U, 0x03F0U, 0x63F5U, 0x03F1U, 0x33F5U, 0x63F5U,
 0x0B72U, 0x6235U, 0x3275U, 0x6B42U, 0x0332U, 0x03C1U, 0x4E13U, 0x5E52U,
 0xAC32U, 0x841AU, 0x4A13U, 0x1A52U, 0x6DD2U, 0x3DD3U, 0x4E13U, 0x5E52U,
 0xAC32U, 0x841AU, 0x4A13U, 0x1A52U, 0x6DD2U, 0x3DD3U, 0x4E13U, 0x5E52U,
 0xAC32U, 0x841AU, 0x4A13U, 0x1A52U, 0x6DD2U, 0x3DD3U, 0x4E13U, 0x5E52U,
 0xAC32U, 0x841AU, 0x841BU, 0x4B48U, 0x0B32U, 0x6DD2U, 0x3DD3U, 0x4E13U,
 0x5E52U, 0xA432U, 0x87E8U, 0x4B44U, 0x0B32U, 0x6DD2U, 0x3DD3U, 0x4E13U,
 0x5E52U, 0xA432U, 0x87E8U, 0x4B42U, 0x0B32U, 0x6DD2U, 0x3DD3U, 0x4E13U,
 0x5E52U, 0xA432U, 0x8403U, 0x4B41U, 0x0B32U, 0x02B4U, 0x03F5U, 0x0250U,
 0x0211U, 0x0314U, 0x0355U, 0x8980U, 0x03C1U, 0x49D0U, 0xB280U, 0x842DU,
 0x0290U, 0x03C0U, 0xBCA1U, 0xC001U, 0x8405U, 0xBCA0U, 0xD001U, 0xA9B7U,
 0x8405U, 0xA9F7U, 0xBCA2U, 0xD001U, 0xA9B7U, 0xA977U, 0x02B7U, 0x22B2U,
 0xBC90U, 0xA177U, 0xA937U, 0x02B7U, 0x22B2U, 0xBC90U, 0xA137U, 0xA8F7U,
 0x02B7U, 0x22B2U, 0xBC90U, 0xA0F7U, 0xA8B7U, 0x02B7U, 0x22B2U, 0xBC90U,
 0xA0B7U, 0xA877U, 0x02B7U, 0x22B2U, 0xBC90U, 0xA077U, 0xA837U, 0x02B7U,
 0x22B2U, 0xBC90U, 0xA037U, 0x8980U, 0x27C0U, 0x8980U, 0x8284U, 0x0012U,
 0x0053U, 0x0210U, 0x03C1U, 0x49D1U, 0x03C0U, 0x59D0U, 0xB280U, 0x8487U,
 0xBC21U, 0xC000U, 0x8408U, 0xBC20U, 0xD000U, 0x840DU, 0xBC20U, 0xC400U,
 0xAB77U, 0x8412U, 0xABF7U, 0xBC22U, 0xD000U, 0x840AU, 0xBC21U, 0xE400U,
 0xAB77U, 0x840AU, 0xABB7U, 0xBC20U, 0xE400U, 0xAB77U, 0x8405U, 0xABB7U,
 0xBC23U, 0xC400U, 0xAB77U, 0xAB37U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA337U, 0xAAF7U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA2F7U, 0xAAB7U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA2B7U, 0xAA77U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA277U, 0xAA37U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA237U, 0xA9F7U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA1F7U, 0xA9B7U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA1B7U, 0xA977U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA177U, 0xA937U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA137U, 0xA8F7U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA0F7U, 0xA8B7U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA0B7U, 0xA877U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA077U, 0xA837U, 0x0237U, 0x6230U, 0x0272U, 0x4E11U,
 0x5E50U, 0xBA80U, 0xA037U, 0x0212U, 0x0253U, 0x8980U, 0x27C0U, 0x87FCU,
 0x03D0U, 0x2680U, 0x00BBU, 0x00BBU, 0x0294U, 0x2A8CU, 0x00BBU, 0x0291U,
 0x00BBU, 0x00BBU, 0x0292U, 0x00BBU, 0x00BBU, 0x0293U, 0x00BBU, 0x8980U,
 0x03D0U, 0x2680U, 0x00BBU, 0x00BBU, 0x0296U, 0x2A8CU, 0x00BBU, 0x0291U,
 0x00BBU, 0x0293U, 0x00BBU, 0x0292U, 0x00BBU, 0x0294U, 0x00BBU, 0x0295U,
 0x00BBU, 0x8980U, 0x03D0U, 0x0291U, 0x00BBU, 0x0292U, 0x00BBU, 0x0296U,
 0x2A8CU, 0x00BBU, 0x0293U, 0x00BBU, 0x00BBU, 0x0294U, 0x00BBU, 0x00BBU,
 0x0295U, 0x00BBU, 0x8980U, 0x03D0U, 0x0291U, 0x00BBU, 0x0292U, 0x00BBU,
 0x0298U, 0x2A8CU, 0x00BBU, 0x0293U, 0x00BBU, 0x0295U, 0x00BBU, 0x0294U,
 0x00BBU, 0x0296U, 0x00BBU, 0x0297U, 0x00BBU, 0x8980U, 0x8963U, 0xE2A7U,
 0x03D0U, 0xAC2BU, 0xFABFU, 0x0BC1U, 0x0BC3U, 0x02BBU, 0x0BC1U, 0x03FBU,
 0x8980U, 0x8963U, 0xE2A7U, 0x03D0U, 0x02A2U, 0xC000U, 0x00A8U, 0xC00EU,
 0x02BBU, 0x00A8U, 0xC00FU, 0x02BBU, 0x00A8U, 0xC00FU, 0x02BBU, 0xAC2BU,
 0xFABFU, 0x0BC1U, 0x42BBU, 0x00A8U, 0xC00FU, 0x0BC1U, 0x02BBU, 0x00A8U,
 0xC00FU, 0xA42BU, 0xFABFU, 0x0BC1U, 0x02BBU, 0x00A8U, 0xC00FU, 0x0280U,
 0x00A8U, 0xC00FU, 0x87D6U, 0x03D0U, 0x0BC7U, 0x02BBU, 0x0FC6U, 0x03FBU,
 0x2FCCU, 0x12B7U, 0x8980U, 0xA82BU, 0xFABFU, 0x8980U, 0x0281U, 0x50ABU,
 0xFABFU, 0x8980U, 0x0291U, 0x2A89U, 0x03D2U, 0x47E0U, 0xC1FFU, 0x42B7U,
 0x03D0U, 0x00BBU, 0x0293U, 0x00BBU, 0x0294U, 0x00BBU, 0x0295U, 0x00BBU,
 0x8980U, 0x03D0U, 0x02A2U, 0xC00AU, 0x00A8U, 0xC00EU, 0x02BBU, 0x2E89U,
 0xBA80U, 0xA9F2U, 0x00A8U, 0xC00FU, 0x03E2U, 0xC018U, 0x01E8U, 0xC00EU,
 0x00A8U, 0xC00FU, 0x03D0U, 0x0090U, 0x0280U, 0x00A8U, 0xC00FU, 0x02A2U,
 0xC017U, 0x00A8U, 0xC00EU, 0x02BBU, 0x46A0U, 0xC1FFU, 0xBA80U, 0xAA72U,
 0x00A8U, 0xC00FU, 0x2290U, 0x00ABU, 0xFABCU, 0x02A2U, 0xC012U, 0x00A8U,
 0xC00EU, 0x02BBU, 0x00A8U, 0xC00FU, 0x0280U, 0x00A8U, 0xC00FU, 0x02A3U,
 0xFF00U, 0x00A8U, 0xC00FU, 0x02BBU, 0x00ABU, 0xFABEU, 0x02BBU, 0x03F2U,
 0x2FC5U, 0x01EBU, 0xFABDU, 0x03F2U, 0x47E0U, 0xC01FU, 0x01E8U, 0xC00FU,
 0x0090U, 0x2E88U, 0xA832U, 0x03F2U, 0x2BC8U, 0xA610U, 0x43F2U, 0x01E8U,
 0xC00FU, 0x8980U, 0x03E2U, 0xC01CU, 0x01E8U, 0xC00EU, 0x0291U, 0x00A8U,
 0xC00FU, 0x0292U, 0x00A8U, 0xC00FU, 0x02ABU, 0xFABDU, 0x4687U, 0x0A81U,
 0x8C32U, 0x8451U, 0x8445U, 0x8437U, 0x8428U, 0x841FU, 0x8416U, 0x840BU,
 0x0290U, 0x2E88U, 0x46A0U, 0xC0E0U, 0x00A8U, 0xC00FU, 0x0290U, 0x46A0U,
 0xDFFFU, 0x844AU, 0x0290U, 0x2E88U, 0x46A0U, 0xC0F0U, 0x00A8U, 0xC00FU,
 0x0290U, 0x46A0U, 0xCFFFU, 0x8440U, 0x0290U, 0x2E88U, 0x00A8U, 0xC00FU,
 0x0290U, 0x46A0U, 0xC0FFU, 0x8438U, 0x0290U, 0x2E8CU, 0x00A8U, 0xC00FU,
 0x0290U, 0x46A0U, 0xCFFFU, 0x8430U, 0x0290U, 0x2E8DU, 0xB280U, 0x0AA0U,
 0xC018U, 0x2A88U, 0xB280U, 0xAB72U, 0x00A8U, 0xC00FU, 0x0290U, 0x46A0U,
 0xDFFFU, 0x8422U, 0x0290U, 0x2E8CU, 0xB280U, 0xA932U, 0x2A88U, 0xB280U,
 0xAB72U, 0x00A8U, 0xC00FU, 0x0290U, 0x46A0U, 0xCFFFU, 0x8415U, 0x0290U,
 0x2E8BU, 0x2A88U, 0xB280U, 0xAB72U, 0x00A8U, 0xC00FU, 0x0290U, 0x46A0U,
 0xC7FFU, 0x840AU, 0x0280U, 0xA7D0U, 0x02A1U, 0xE000U, 0x00A8U, 0xC00FU,
 0x0290U, 0x46A1U, 0xFFFFU, 0x22ABU, 0xFABCU, 0x0AABU, 0xFABEU, 0x03E2U,
 0xC01AU, 0x01E8U, 0xC00EU, 0x00A8U, 0xC00FU, 0x02A2U, 0xC01FU, 0x00A8U,
 0xC00EU, 0x00A8U, 0xC00FU, 0x8980U, 0x03E2U, 0xC01CU, 0x01E8U, 0xC00EU,
 0x0291U, 0x00A8U, 0xC00FU, 0x0280U, 0x00A8U, 0xC00FU, 0x8786U, 0x03D0U,
 0x02BBU, 0x03F2U, 0x2FC9U, 0xBBC0U, 0xA9F7U, 0x46A0U, 0xC1FFU, 0xBA80U,
 0xAA72U, 0x8980U, 0x03D0U, 0x0BC5U, 0x0296U, 0x00BBU, 0x0297U, 0x00BBU,
 0x0298U, 0x00BBU, 0x03D0U, 0x0291U, 0x00BBU, 0x0292U, 0x00BBU, 0x0293U,
 0x00BBU, 0x0294U, 0x00BBU, 0x0295U, 0x00BBU, 0x8980U, 0x03D0U, 0x0BC5U,
 0x02A3U, 0xE0ACU, 0x00BBU, 0x02A3U, 0xE0AAU, 0x00BBU, 0x02A3U, 0xE0B0U,
 0x87E9U, 0x0280U, 0x00ABU, 0xFAA9U, 0x00ABU, 0xFAAAU, 0x00ABU, 0xFAABU,
 0x8923U, 0xE09EU, 0xC051U, 0x8923U, 0xE0A0U, 0xC051U, 0x01EBU, 0xFAA5U,
 0x03C1U, 0x2BF2U, 0x17EBU, 0xFAA5U, 0x2BC1U, 0x01EBU, 0xFAA6U, 0x03D0U,
 0x02BBU, 0x0BC5U, 0x893BU, 0xC072U, 0x03D0U, 0x02BBU, 0x0BC6U, 0x893BU,
 0xC072U, 0x01EBU, 0xFAA7U, 0x00ABU, 0xFAA8U, 0x03D0U, 0x0BC1U, 0x02BBU,
 0x00ABU, 0xFAA0U, 0x02BBU, 0x00ABU, 0xFAA1U, 0x02BBU, 0x00ABU, 0xFAA2U,
 0x02BBU, 0x00ABU, 0xFAA3U, 0x02BBU, 0x00ABU, 0xFAA4U, 0x8980U, 0x0280U,
 0x00ABU, 0xFAA9U, 0x0293U, 0x03D2U, 0x01EBU, 0xFAAAU, 0x87C8U, 0x0293U,
 0x00ABU, 0xFAA9U, 0x0294U, 0x87F8U, 0x828EU, 0x1012U, 0x1053U, 0x011BU,
 0x015CU, 0x019DU, 0xBA00U, 0x8480U, 0xBA40U, 0x847EU, 0x02ABU, 0xFAA5U,
 0x0099U, 0x02ABU, 0xFAA6U, 0x009AU, 0x0290U, 0x22ABU, 0xFAA7U, 0x0AABU,
 0xFAAAU, 0x5699U, 0x0094U, 0x0291U, 0x22ABU, 0xFAA8U, 0x0AABU, 0xFAABU,
 0x569AU, 0x0095U, 0xBA01U, 0x846EU, 0x222BU, 0xFAA7U, 0xBC19U, 0x0219U,
 0x0A14U, 0xBC19U, 0x0E19U, 0x0016U, 0x226BU, 0xFAA8U, 0xBC5AU, 0x025AU,
 0x0A55U, 0xBC5AU, 0x0E5AU, 0x0057U, 0x0290U, 0x56ABU, 0xFAA0U, 0x0090U,
 0x0291U, 0x56ABU, 0xFAA1U, 0x0091U, 0x0280U, 0x00A8U, 0xC03AU, 0xA932U,
 0x00A8U, 0xC03BU, 0x0284U, 0x00A8U, 0xC03CU, 0x032BU, 0xFAA0U, 0x6B04U,
 0x0372U, 0x0211U, 0x622BU, 0xFAA0U, 0x0272U, 0x4A2BU, 0xFAA3U, 0x1A6BU,
 0xFAA2U, 0x03B0U, 0x03F1U, 0x4B90U, 0x0BF2U, 0x6B84U, 0x3BC4U, 0x01E8U,
 0xC038U, 0x01A8U, 0xC039U, 0x6A04U, 0x3A44U, 0x4A34U, 0x1A75U, 0x0394U,
 0x0295U, 0x2299U, 0x0098U, 0x03D8U, 0x0BF6U, 0x892BU, 0xFAA4U, 0xC028U,
 0xC03FU, 0xC037U, 0xC06BU, 0xFAA9U, 0xB028U, 0xC039U, 0x8408U, 0xB068U,
 0xC038U, 0x8405U, 0x4D28U, 0xC039U, 0x1D68U, 0xC038U, 0x0BABU, 0xFAA7U,
 0xBD99U, 0x0F99U, 0xB396U, 0x87E8U, 0x0291U, 0x0A81U, 0xBCABU, 0xFAA1U,
 0x0EABU, 0xFAA1U, 0x0091U, 0x0295U, 0x0AABU, 0xFAA8U, 0xBC9AU, 0x0E9AU,
 0x0095U, 0xB297U, 0x87BFU, 0x0212U, 0x0253U, 0x031BU, 0x035CU, 0x039DU,
 0x8980U, 0x226BU, 0xFAA8U, 0xBC5AU, 0x025AU, 0x0A55U, 0xBC5AU, 0x0E5AU,
 0x0057U, 0x0290U, 0x56ABU, 0xFAA0U, 0x0090U, 0x0291U, 0x56ABU, 0xFAA1U,
 0x0091U, 0x022BU, 0xFAA0U, 0x6A04U, 0x00A8U, 0xC03AU, 0x0028U, 0xC03BU,
 0x0284U, 0x00A8U, 0xC03CU, 0x0391U, 0x63ABU, 0xFAA0U, 0x03F2U, 0x4BABU,
 0xFAA3U, 0x1BEBU, 0xFAA2U, 0x4B90U, 0x0BF2U, 0x6B84U, 0x3BC4U, 0x01E8U,
 0xC038U, 0x01A8U, 0xC039U, 0x022BU, 0xFAA0U, 0x622BU, 0xFAA1U, 0x0272U,
 0x6A04U, 0x3A44U, 0x03D5U, 0x23D9U, 0x0BD4U, 0x892BU, 0xFAA4U, 0xC028U,
 0xC03FU, 0xC037U, 0xC06BU, 0xFAA9U, 0x0291U, 0x0A81U, 0xBCABU, 0xFAA1U,
 0x840BU, 0x0091U, 0x0295U, 0x0AABU, 0xFAA8U, 0xBC9AU, 0x0E9AU, 0x0095U,
 0xB297U, 0x87E9U, 0x87B1U, 0x0EABU, 0xFAA1U, 0x4C28U, 0xC039U, 0x1C68U,
 0xC038U, 0x87F0U, 0x03D0U, 0x0BC1U, 0x02BBU, 0x03FBU, 0x13F2U, 0x8980U,
 0x03D0U, 0x02BBU, 0x0BC6U, 0x893BU, 0xC072U, 0x8980U, 0x03D0U, 0x0BC1U,
 0x0010U, 0x0291U, 0x023BU, 0x56B0U, 0x0091U, 0x0292U, 0x56BBU, 0x0052U,
 0x6232U, 0x0272U, 0x4A11U, 0x1A7BU, 0x4A3BU, 0x0A72U, 0x6A04U, 0x3A44U,
 0x0068U, 0xC038U, 0x0028U, 0xC039U, 0x0282U, 0x00A8U, 0xC03CU, 0x03E8U,
 0xC03EU, 0x0210U, 0x0252U, 0x8980U, 0x03D0U, 0x0BC1U, 0x0010U, 0x0291U,
 0x023BU, 0x56B0U, 0x0091U, 0x0292U, 0x56BBU, 0x0052U, 0x6232U, 0x0272U,
 0x4A11U, 0x1A7BU, 0x4A3BU, 0x0A72U, 0x6A04U, 0x3A44U, 0x0068U, 0xC038U,
 0x0028U, 0xC039U, 0x0282U, 0x00A8U, 0xC03CU, 0x03D3U, 0x01E8U, 0xC03EU,
 0x0210U, 0x0252U, 0x8980U, 0x8284U, 0x1191U, 0x8012U, 0x0013U, 0x03D0U,
 0x0BC3U, 0x023BU, 0x03FBU, 0x6BC4U, 0x3A04U, 0x02B0U, 0x0686U, 0x4783U,
 0x2B83U, 0x0BA0U, 0xC020U, 0x00BAU, 0x01FAU, 0x0200U, 0x003AU, 0xA930U,
 0x003AU, 0x0204U, 0x003AU, 0x0391U, 0x8212U, 0x0213U, 0x8980U, 0x03D0U,
 0x0291U, 0x00BBU, 0x0292U, 0x00BBU, 0x0295U, 0x00BBU, 0x0296U, 0x00BBU,
 0x0280U, 0x00BBU, 0x00BBU, 0x0298U, 0xA832U, 0x00BBU, 0x0293U, 0x00BBU,
 0x0294U, 0x00BBU, 0x0297U, 0x00BBU, 0x8980U, 0x03D0U, 0x0BC6U, 0xA83BU,
 0x8980U, 0x8285U, 0x03D0U, 0x02BBU, 0x0090U, 0x0BC1U, 0x02BBU, 0x0091U,
 0x02BBU, 0x0092U, 0x0BC2U, 0x02BBU, 0x0094U, 0x8923U, 0xEBB8U, 0xC050U,
 0x0090U, 0x01D3U, 0x03D2U, 0x2BCEU, 0x47C1U, 0x0BE0U, 0xC014U, 0x03FBU,
 0x2FC8U, 0xAC94U, 0x0BD3U, 0x0BD3U, 0x0FC1U, 0x17D3U, 0x0291U, 0xAC54U,
 0x0A90U, 0x0A90U, 0x0E81U, 0x1690U, 0x8980U, 0x03D0U, 0x0BC4U, 0x02BBU,
 0x03FBU, 0x12B7U, 0x8980U, 0x8284U, 0x1211U, 0x1252U, 0x00D3U, 0x03D0U,
 0x0BC2U, 0x02FBU, 0xBE73U, 0x8411U, 0x03D0U, 0x8923U, 0xEBB8U, 0xC07BU,
 0x0AF2U, 0x0EC1U, 0x16F2U, 0x03F1U, 0x0BF2U, 0x0FC1U, 0x17F2U, 0xBAF7U,
 0x8404U, 0x03D0U, 0x0BC6U, 0xA83BU, 0x03D0U, 0x0BC2U, 0x007BU, 0x0BC5U,
 0x003BU, 0x0211U, 0x0252U, 0x02D3U, 0x8980U, 0x82A0U, 0xC012U, 0x001EU,
 0x005FU, 0x012CU, 0xC010U, 0x016CU, 0xC011U, 0x03D0U, 0x033BU, 0x037BU,
 0x02BBU, 0xBA80U, 0x84E1U, 0x0094U, 0x02BBU, 0x0095U, 0x02BBU, 0x0096U,
 0x02BBU, 0x0097U, 0x02BBU, 0x009CU, 0x03D5U, 0x2BCEU, 0x47C1U, 0x0BE0U,
 0xC014U, 0x023BU, 0x2E08U, 0xBA00U, 0x84CFU, 0x8923U, 0xEBB8U, 0xC074U,
 0x0098U, 0x01D9U, 0xAC9CU, 0x0A19U, 0x0A19U, 0x0E01U, 0x1619U, 0x001AU,
 0x0254U, 0xAC5CU, 0x0A58U, 0x0A58U, 0x0E41U, 0x1658U, 0x005BU, 0x8923U,
 0xEA91U, 0xC034U, 0xC075U, 0x8963U, 0xE2A7U, 0x01D3U, 0x0299U, 0x2A82U,
 0xAF13U, 0x2A81U, 0x0211U, 0x1632U, 0x0252U, 0x1658U, 0x03D0U, 0x0BC4U,
 0x003BU, 0x007BU, 0xA43BU, 0x8435U, 0x03D6U, 0x0BDAU, 0x01D8U, 0x03D7U,
 0x0BDBU, 0x01DDU, 0x0330U, 0x0371U, 0x0F16U, 0x0F57U, 0xA7F4U, 0x840AU,
 0xA7F5U, 0x8403U, 0x0059U, 0x8412U, 0x6775U, 0x005DU, 0x0297U, 0x0099U,
 0x840DU, 0xA7F5U, 0x8405U, 0x6734U, 0x0018U, 0x0059U, 0x8407U, 0x6734U,
 0x6775U, 0x005DU, 0x0018U, 0x0297U, 0x0099U, 0xBD1AU, 0x8412U, 0xBD5BU,
 0x8410U, 0x03DBU, 0x0FF5U, 0x8923U, 0xEAD4U, 0xC018U, 0xC019U, 0xC034U,
 0xC077U, 0x8923U, 0xEAD4U, 0xC030U, 0xC01DU, 0xC01AU, 0xC075U, 0x8407U,
 0x8923U, 0xEAD4U, 0xC030U, 0xC031U, 0xC01AU, 0xC05BU, 0x03D0U, 0x0BC6U,
 0xA03BU, 0x0FC1U, 0x033BU, 0x02BBU, 0x0098U, 0x02BBU, 0x0099U, 0x02BBU,
 0x009DU, 0x0280U, 0xB298U, 0xBA9DU, 0x845FU, 0x03D3U, 0x47C3U, 0xA753U,
 0x0BC1U, 0x0284U, 0x2AB7U, 0xBE98U, 0x8457U, 0x03E0U, 0xC190U, 0xA753U,
 0x2FC1U, 0xBFD9U, 0x8451U, 0x0299U, 0x0A94U, 0x0BC1U, 0xBFF2U, 0x844CU,
 0xAFD5U, 0xA7DDU, 0x8449U, 0x0295U, 0x0A9DU, 0x0E81U, 0xA7F2U, 0x8444U,
 0x03D1U, 0xA713U, 0x2BC1U, 0x47E0U, 0xC3FFU, 0x0200U, 0xA674U, 0xABF0U,
 0x4723U, 0xFC00U, 0x4337U, 0x0352U, 0x575DU, 0x0372U, 0x0B55U, 0x4370U,
 0x0295U, 0x0A9DU, 0x009CU, 0x401CU, 0x8923U, 0xE35BU, 0xC018U, 0xC019U,
 0xC053U, 0x2683U, 0x0203U, 0x4694U, 0x4614U, 0x03E0U, 0xC01EU, 0x0270U,
 0x2A03U, 0x0E31U, 0x0FF0U, 0x8C37U, 0x0E84U, 0x0168U, 0xC037U, 0x0128U,
 0xC03FU, 0x0B41U, 0xBB5CU, 0x0F5DU, 0x0168U, 0xC037U, 0x0128U, 0xC03FU,
 0x0B41U, 0xBB5CU, 0x0F5DU, 0x0168U, 0xC037U, 0x0128U, 0xC03FU, 0x0B41U,
 0xBB5CU, 0x0F5DU, 0x0168U, 0xC037U, 0x0128U, 0xC03FU, 0x0B41U, 0xBB5CU,
 0x0F5DU, 0xB280U, 0x87E2U, 0x021EU, 0x025FU, 0x032CU, 0xC010U, 0x036CU,
 0xC011U, 0x8980U
};



/* 0xFA00 - 0xFAFF initializers. First byte is offset low, then high & low of data */
const uint8  rrpge_m_ulib_idat[RRPGE_M_ULIB_ICNT * 3U] = {
 0xACU, 0x02U, 0x0CU,
 0xADU, 0x00U, 0x0FU,
 0xAEU, 0xC8U, 0x00U,
 0xAFU, 0x00U, 0x20U,
 0xB0U, 0x02U, 0x0CU,
 0xB1U, 0x00U, 0x0FU,
 0xB2U, 0xC8U, 0x00U,
 0xB3U, 0x01U, 0xAAU,
 0xB4U, 0x04U, 0x0CU,
 0xB5U, 0x00U, 0x0FU,
 0xB6U, 0xC8U, 0x00U,
 0xB7U, 0x00U, 0x31U,
 0xB8U, 0x04U, 0x0CU,
 0xB9U, 0x00U, 0x0FU,
 0xBAU, 0xC8U, 0x00U,
 0xBBU, 0x01U, 0xBDU,
 0xC0U, 0xFFU, 0xFFU,
 0xC1U, 0xFFU, 0xFFU,
 0xC2U, 0xF0U, 0x00U,
 0xC7U, 0x00U, 0x50U,
 0xD0U, 0xE0U, 0xA4U,
 0xDFU, 0xFAU, 0xD1U,
 0xEEU, 0xFAU, 0xE0U,
 0xEFU, 0xFAU, 0xF3U,
 0xF0U, 0xE0U, 0x6CU,
 0xF1U, 0xE0U, 0x6EU,
 0xF2U, 0xE0U, 0xA6U
};
