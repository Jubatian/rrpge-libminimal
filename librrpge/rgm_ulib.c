/**
**  \file
**  \brief     RRPGE User Library binary
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.11.21
*/


#include "rgm_ulib.h"


/* The RRPGE User Library */
const uint16 rrpge_m_ulib[RRPGE_M_ULIB_SIZE] = {
 0x8D23U, 0xF0DCU, 0x8D23U, 0xF0DEU, 0x8D23U, 0xF0D8U, 0x8D23U, 0xF0DAU,
 0x8D23U, 0xF0D4U, 0x8D23U, 0xF0D6U, 0x8D23U, 0xF0CEU, 0x8D23U, 0xF0D2U,
 0x8D23U, 0xF0B1U, 0x8D23U, 0xF0CCU, 0xC000U, 0xC000U, 0xC000U, 0xC000U,
 0x8D23U, 0xF093U, 0x8D23U, 0xF0AFU, 0x8D23U, 0xF080U, 0xC000U, 0xC000U,
 0x8D23U, 0xF11CU, 0x8D23U, 0xF166U, 0x8D23U, 0xF189U, 0x8D23U, 0xF181U,
 0x8D23U, 0xF0E0U, 0x8D23U, 0xF117U, 0x8D23U, 0xF1B5U, 0x8D23U, 0xF1CBU,
 0x8D23U, 0xF1DEU, 0x8D23U, 0xF1E3U, 0x8D23U, 0xF30BU, 0x8D23U, 0xF33AU,
 0x8D23U, 0xF388U, 0x8D23U, 0xF3B2U, 0x8D23U, 0xF3EDU, 0x8D23U, 0xF43CU,
 0x8D23U, 0xF1EAU, 0x8D23U, 0xF218U, 0x8D23U, 0xF2BDU, 0x8D23U, 0xF2C2U,
 0x8D23U, 0xF2C9U, 0x8D23U, 0xF2D1U, 0x8D23U, 0xF2D8U, 0x8D23U, 0xF2DFU,
 0x8D23U, 0xF231U, 0x8D23U, 0xF24DU, 0x8D23U, 0xF2E4U, 0x8D23U, 0xF2E9U,
 0x8D23U, 0xF2F0U, 0x8D23U, 0xF2F8U, 0x8D23U, 0xF2FFU, 0x8D23U, 0xF306U,
 0x8D23U, 0xF259U, 0x8D23U, 0xF266U, 0x8D23U, 0xF27BU, 0x8D23U, 0xF288U,
 0x8D23U, 0xF29DU, 0x8D23U, 0xF2ABU, 0x8D23U, 0xF44FU, 0x8D23U, 0xF4B5U,
 0x8D23U, 0xF468U, 0x8D23U, 0xF4EDU, 0x8D23U, 0xF471U, 0x8D23U, 0xF4F6U,
 0x8D23U, 0xF48BU, 0x8D23U, 0xF570U, 0x8D23U, 0xF48EU, 0x8D23U, 0xF5A3U,
 0x03D0U, 0x47C3U, 0x2BC3U, 0x0BE0U, 0xC020U, 0x0291U, 0x00BBU, 0x0292U,
 0x00BBU, 0x0293U, 0x00BBU, 0x0294U, 0x00BBU, 0x0295U, 0x00BBU, 0x0BC1U,
 0x02B7U, 0x0BC1U, 0x8980U, 0x03C4U, 0x1211U, 0x1252U, 0x6A44U, 0x3A04U,
 0x1313U, 0x1354U, 0x6B44U, 0x3B04U, 0x02B7U, 0x03D0U, 0x47C3U, 0x2BC3U,
 0x0BE0U, 0xC020U, 0x003BU, 0x007BU, 0x013BU, 0x017BU, 0x00BBU, 0x0BC1U,
 0x02B7U, 0x0BC1U, 0x0211U, 0x0252U, 0x0313U, 0x0354U, 0x8980U, 0x03CCU,
 0x87E4U, 0x03C4U, 0x1211U, 0x1252U, 0x6A44U, 0x3A04U, 0x02B7U, 0x03D0U,
 0x47C3U, 0x2BC3U, 0x0BE0U, 0xC020U, 0x003BU, 0x007BU, 0x0200U, 0x003BU,
 0x0201U, 0x0272U, 0x4647U, 0x2A31U, 0x003BU, 0x00BBU, 0x0BC1U, 0x02B7U,
 0x0BC1U, 0x0211U, 0x0252U, 0x8980U, 0x03CCU, 0x87E5U, 0x03C3U, 0x1211U,
 0x1252U, 0x87E5U, 0x03CBU, 0x87FCU, 0x03C2U, 0x87FAU, 0x03CAU, 0x87F8U,
 0x03C1U, 0x87F6U, 0x03C9U, 0x87F4U, 0x03C0U, 0x87F2U, 0x03C8U, 0x87F0U,
 0x03D1U, 0x6BC4U, 0x01E8U, 0xC039U, 0x03D0U, 0x3BC4U, 0x01E8U, 0xC038U,
 0x03C0U, 0x01E8U, 0xC03AU, 0xA937U, 0x01E8U, 0xC03BU, 0x03C4U, 0x01E8U,
 0xC03CU, 0x03E0U, 0xC03FU, 0x06C4U, 0x0010U, 0x0212U, 0x0293U, 0xACB2U,
 0x8406U, 0x003BU, 0x003BU, 0x003BU, 0x003BU, 0x0E84U, 0xAC72U, 0x8404U,
 0x003BU, 0x003BU, 0x0E82U, 0xAC32U, 0x8403U, 0x003BU, 0x0E81U, 0xBA80U,
 0x840CU, 0x003BU, 0x003BU, 0x003BU, 0x003BU, 0x003BU, 0x003BU, 0x003BU,
 0x003BU, 0x0E88U, 0xB280U, 0x87F6U, 0x0210U, 0x06C6U, 0x8980U, 0x03D0U,
 0x0010U, 0x0211U, 0x0292U, 0x87DCU, 0x03D1U, 0x6BC4U, 0x01E8U, 0xC039U,
 0x03D0U, 0x3BC4U, 0x01E8U, 0xC038U, 0x03C0U, 0x01E8U, 0xC03AU, 0xA937U,
 0x01E8U, 0xC03BU, 0x03C4U, 0x01E8U, 0xC03CU, 0x03D2U, 0x0293U, 0x0191U,
 0x03A0U, 0xC03FU, 0x8012U, 0x8221U, 0xE466U, 0x0010U, 0xACB2U, 0x840AU,
 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU,
 0x0E84U, 0xAC72U, 0x8406U, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x0E82U,
 0xAC32U, 0x8404U, 0x023BU, 0x003AU, 0x0E81U, 0xBA80U, 0x8414U, 0x023BU,
 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU,
 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x023BU, 0x003AU, 0x0E88U,
 0xB280U, 0x87EEU, 0x0210U, 0x0391U, 0x8212U, 0x8980U, 0x03D2U, 0x6BC4U,
 0x01E8U, 0xC039U, 0x03D1U, 0x3BC4U, 0x01E8U, 0xC038U, 0x03C0U, 0x01E8U,
 0xC03AU, 0xA937U, 0x01E8U, 0xC03BU, 0x03C4U, 0x01E8U, 0xC03CU, 0x03D0U,
 0x0293U, 0x0191U, 0x03A0U, 0xC03FU, 0x13B7U, 0x8012U, 0x8221U, 0xC666U,
 0x87B5U, 0x03D1U, 0x0191U, 0x0390U, 0x0292U, 0x8012U, 0x8221U, 0xE666U,
 0x87ADU, 0x03D3U, 0x6BC4U, 0x01E8U, 0xC039U, 0x03D2U, 0x3BC4U, 0x01E8U,
 0xC038U, 0x03C0U, 0x01E8U, 0xC03AU, 0xA937U, 0x01E8U, 0xC03BU, 0x03C4U,
 0x01E8U, 0xC03CU, 0x03D1U, 0x6BC4U, 0x01E8U, 0xC031U, 0x03D0U, 0x3BC4U,
 0x01E8U, 0xC030U, 0x03C0U, 0x01E8U, 0xC032U, 0xA937U, 0x01E8U, 0xC033U,
 0x03C4U, 0x01E8U, 0xC034U, 0x0294U, 0x0191U, 0x03E0U, 0xC03FU, 0x03A0U,
 0xC037U, 0x8012U, 0x8221U, 0xC466U, 0x8781U, 0x0280U, 0xB894U, 0x8411U,
 0x03E3U, 0xFFF0U, 0x8923U, 0xF024U, 0xC010U, 0xC011U, 0xC012U, 0xC013U,
 0xC077U, 0x49D1U, 0x0890U, 0x49D3U, 0x0892U, 0x4DD5U, 0x0894U, 0x87EEU,
 0x0295U, 0x0094U, 0x865AU, 0x0280U, 0xB893U, 0x840EU, 0x03E3U, 0xFFF0U,
 0x8923U, 0xF028U, 0xC010U, 0xC011U, 0xC012U, 0xC077U, 0x49D1U, 0x0890U,
 0x4DD4U, 0x0893U, 0x87F1U, 0x0294U, 0x0093U, 0x864BU, 0x03C6U, 0x69D1U,
 0x39D0U, 0x0292U, 0x8409U, 0x8923U, 0xF1EAU, 0xC050U, 0x47E0U, 0xC7FCU,
 0x6BCAU, 0x8980U, 0x0290U, 0x4683U, 0x27C3U, 0x2BF2U, 0x47E0U, 0xC7FCU,
 0x47D0U, 0x43F2U, 0x8980U, 0x8281U, 0x03E3U, 0xFAF0U, 0x8405U, 0x02BBU,
 0x01D0U, 0x8972U, 0x03D0U, 0xB3EBU, 0xFAEFU, 0x87FAU, 0xA82BU, 0xFAFDU,
 0x8980U, 0x8281U, 0x03E3U, 0xFAE0U, 0x8405U, 0x02BBU, 0x01D0U, 0x8972U,
 0x03D0U, 0xB3EBU, 0xFAEEU, 0x87FAU, 0xA02BU, 0xFAFDU, 0x8980U, 0x8923U,
 0xF040U, 0xC050U, 0x02A8U, 0xC017U, 0x46A0U, 0xF000U, 0x43F2U, 0x8980U,
 0x8923U, 0xF20FU, 0xC050U, 0x01E8U, 0xC017U, 0x01EBU, 0xFAFFU, 0x47C3U,
 0x2683U, 0x4491U, 0x41D1U, 0x8923U, 0xF20FU, 0xC051U, 0x01EBU, 0xFAFEU,
 0x8963U, 0xF1F3U, 0xA7E8U, 0xC017U, 0x87FEU, 0x03D2U, 0x01E8U, 0xC016U,
 0x841DU, 0xA42BU, 0xFAFDU, 0x8963U, 0xF201U, 0xA428U, 0xC00DU, 0x87FEU,
 0x8923U, 0xF20FU, 0xC06BU, 0xFAFFU, 0x13EBU, 0xFAFEU, 0x01E8U, 0xC017U,
 0x01EBU, 0xFAFFU, 0x03E3U, 0xF990U, 0x02BBU, 0x12BBU, 0x0FC2U, 0x00BBU,
 0x0BC1U, 0xB3E3U, 0xF9A0U, 0x87F9U, 0x87A7U, 0xA42BU, 0xFAFDU, 0x8404U,
 0x03EBU, 0xFAFEU, 0x8980U, 0xA7E8U, 0xC017U, 0x87FEU, 0x8963U, 0xF201U,
 0x87F8U, 0x8923U, 0xF266U, 0xC050U, 0x03EBU, 0xFAEFU, 0xBBE3U, 0xFAFDU,
 0x8405U, 0x0290U, 0x00BBU, 0x01EBU, 0xFAEFU, 0x8980U, 0x03E3U, 0xFAF0U,
 0x0290U, 0x8403U, 0xBABBU, 0x8409U, 0xB3EBU, 0xFAEFU, 0x87FCU, 0x840BU,
 0x02BBU, 0x0FC2U, 0x00BBU, 0x0BC1U, 0xB3EBU, 0xFAEFU, 0x87FAU, 0x0FC1U,
 0x01EBU, 0xFAEFU, 0x8980U, 0x8923U, 0xF288U, 0xC050U, 0x03EBU, 0xFAEEU,
 0xBBE3U, 0xFAEEU, 0x8405U, 0x0290U, 0x00BBU, 0x01EBU, 0xFAEEU, 0x8980U,
 0x03E3U, 0xFAE0U, 0x0290U, 0x8403U, 0xBABBU, 0x8409U, 0xB3EBU, 0xFAEEU,
 0x87FCU, 0x840BU, 0x02BBU, 0x0FC2U, 0x00BBU, 0x0BC1U, 0xB3EBU, 0xFAEEU,
 0x87FAU, 0x0FC1U, 0x01EBU, 0xFAEEU, 0x8980U, 0x03D0U, 0x47C3U, 0x2BC2U,
 0x0BE3U, 0xF990U, 0x0293U, 0x00BBU, 0x0291U, 0x00BBU, 0x0294U, 0x00BBU,
 0x0292U, 0x00BBU, 0x8980U, 0xA42BU, 0xFAFDU, 0x840AU, 0x03D0U, 0x47C3U,
 0x2BC2U, 0x0BE3U, 0xF990U, 0x02BBU, 0x0BC1U, 0x03FBU, 0x8980U, 0xA7E8U,
 0xC017U, 0x87FEU, 0x8963U, 0xF201U, 0x87F2U, 0x8283U, 0x03E8U, 0xC017U,
 0x01D2U, 0x844AU, 0x8286U, 0x0294U, 0x0095U, 0x03E8U, 0xC017U, 0x01D4U,
 0x8472U, 0x8287U, 0x0294U, 0x1295U, 0x0096U, 0x03E8U, 0xC017U, 0x01D4U,
 0x84B8U, 0x8285U, 0x0293U, 0x0094U, 0x03E8U, 0xC017U, 0x01D3U, 0x84DBU,
 0x8286U, 0x0294U, 0x0095U, 0x03E8U, 0xC017U, 0x01D4U, 0x850FU, 0x8281U,
 0x03E8U, 0xC017U, 0x01D0U, 0x8559U, 0x8283U, 0x8963U, 0xF24DU, 0x01D2U,
 0x8423U, 0x8286U, 0x0294U, 0x0095U, 0x8963U, 0xF24DU, 0x01D4U, 0x844BU,
 0x8287U, 0x0294U, 0x1295U, 0x0096U, 0x8963U, 0xF24DU, 0x01D4U, 0x8491U,
 0x8285U, 0x0293U, 0x0094U, 0x8963U, 0xF24DU, 0x01D3U, 0x84B4U, 0x8286U,
 0x0294U, 0x0095U, 0x8963U, 0xF24DU, 0x01D4U, 0x84E8U, 0x8281U, 0x8963U,
 0xF24DU, 0x01D0U, 0x8532U, 0x8287U, 0x0013U, 0x0054U, 0x00D5U, 0x0116U,
 0x0212U, 0x4603U, 0x26C3U, 0x2AF0U, 0x46E0U, 0xC7FCU, 0xA752U, 0x0A01U,
 0x0A07U, 0x6811U, 0x0272U, 0x0290U, 0x2A85U, 0x0891U, 0x46D2U, 0x6ACEU,
 0x0332U, 0x4AD1U, 0x1B31U, 0x0241U, 0x6A70U, 0x0204U, 0x03E0U, 0xC030U,
 0x013BU, 0x00FBU, 0x00BBU, 0x007BU, 0x003BU, 0x0BC3U, 0xA933U, 0x013BU,
 0x00FBU, 0x00BBU, 0x007BU, 0x003BU, 0x03F1U, 0x0213U, 0x0254U, 0x02D5U,
 0x0316U, 0x8980U, 0x8289U, 0x0017U, 0x00D8U, 0x03D0U, 0x2FCCU, 0x47C7U,
 0x0BE0U, 0xC018U, 0x02FBU, 0xA533U, 0x02C0U, 0x46CFU, 0x2AC1U, 0x0AC1U,
 0x00D6U, 0x03E0U, 0xC0C8U, 0xAF54U, 0x2BC1U, 0xAFD5U, 0x8409U, 0x0215U,
 0x0812U, 0xA7D2U, 0x8433U, 0x2216U, 0x0C10U, 0x0200U, 0x0015U, 0xBFD5U,
 0x842DU, 0x0237U, 0x0E15U, 0xBE12U, 0x0012U, 0x8923U, 0xF30BU, 0xC013U,
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
 0xF30BU, 0xC000U, 0xC014U, 0xC053U, 0x2683U, 0x0203U, 0x4692U, 0x4612U,
 0x03E0U, 0xC012U, 0x2A02U, 0x0FF0U, 0x0210U, 0x02D1U, 0x8C37U, 0x0E84U,
 0x0028U, 0xC037U, 0x00E8U, 0xC03FU, 0x0028U, 0xC037U, 0x00E8U, 0xC03FU,
 0x0028U, 0xC037U, 0x00E8U, 0xC03FU, 0x0028U, 0xC037U, 0x00E8U, 0xC03FU,
 0xB280U, 0x87EEU, 0x0215U, 0x02D6U, 0x8980U, 0x8287U, 0x0016U, 0x03E0U,
 0xC0C8U, 0xAF54U, 0x2BC1U, 0xAFD5U, 0x840AU, 0x0215U, 0x0812U, 0xA7D2U,
 0x8442U, 0x2A01U, 0x4C11U, 0x0C90U, 0x0200U, 0x0015U, 0xBFD5U, 0x843BU,
 0x0237U, 0x0E15U, 0xBE12U, 0x0012U, 0x8923U, 0xF30BU, 0xC013U, 0xC015U,
 0xC054U, 0x8923U, 0xF0B1U, 0xC001U, 0xC010U, 0xC051U, 0x2683U, 0x0203U,
 0x4692U, 0x4612U, 0x03E0U, 0xC022U, 0x2A03U, 0x0FF0U, 0x8C37U, 0x0E84U,
 0x0228U, 0xC02FU, 0x0028U, 0xC037U, 0x0228U, 0xC02FU, 0x0028U, 0xC03FU,
 0x0228U, 0xC02FU, 0x0028U, 0xC037U, 0x0228U, 0xC02FU, 0x0028U, 0xC03FU,
 0x0228U, 0xC02FU, 0x0028U, 0xC037U, 0x0228U, 0xC02FU, 0x0028U, 0xC03FU,
 0x0228U, 0xC02FU, 0x0028U, 0xC037U, 0x0228U, 0xC02FU, 0x0028U, 0xC03FU,
 0xB280U, 0x87DEU, 0x0216U, 0x8980U, 0x0290U, 0x4683U, 0x27C3U, 0x2BF2U,
 0x47E0U, 0xC7FCU, 0x45D0U, 0x03E0U, 0xCC80U, 0x2BF2U, 0x028AU, 0x6890U,
 0x8923U, 0xF0E0U, 0xC032U, 0xC010U, 0xC000U, 0xC077U, 0x8980U, 0xA42BU,
 0xFADCU, 0x8980U, 0xA82BU, 0xFADCU, 0x02A8U, 0xC017U, 0x03C4U, 0xA772U,
 0x03C8U, 0x4683U, 0x2BF2U, 0x02ABU, 0xFADBU, 0xBFF2U, 0x02B7U, 0x00ABU,
 0xFAD8U, 0x0AABU, 0xFADAU, 0xBFF2U, 0x02B7U, 0x00ABU, 0xFAD9U, 0x8980U,
 0x0290U, 0x00ABU, 0xFADBU, 0x0291U, 0x00ABU, 0xFADAU, 0xA02BU, 0xFADCU,
 0x8980U, 0x03E3U, 0xF2E9U, 0xA02BU, 0xFADCU, 0xAC13U, 0x840BU, 0x02ABU,
 0xFAD9U, 0xBAABU, 0xFAD8U, 0x8980U, 0x0E81U, 0x0093U, 0x00ABU, 0xFAD9U,
 0x8D37U, 0x02ABU, 0xFAD8U, 0xBAABU, 0xFAD9U, 0x8980U, 0x0093U, 0x0A81U,
 0x00ABU, 0xFAD8U, 0x8D37U, 0x03E3U, 0xF2F0U, 0x87E6U, 0x03E3U, 0xF2FFU,
 0x87E3U, 0x8285U, 0x0012U, 0x0053U, 0x00D4U, 0x0211U, 0x4603U, 0x26C3U,
 0x2AF0U, 0x46E0U, 0xC7FCU, 0xA751U, 0x0A01U, 0x0A07U, 0x6810U, 0x0272U,
 0x46D1U, 0x6ACEU, 0x0A72U, 0x4AD0U, 0x0A72U, 0x0010U, 0x0280U, 0x0204U,
 0x03E0U, 0xC038U, 0x007BU, 0x00FBU, 0x00BBU, 0x00BBU, 0x003BU, 0x03C1U,
 0x2BD0U, 0x0212U, 0x0253U, 0x02D4U, 0x8980U, 0xA42BU, 0xFADDU, 0x8980U,
 0xA82BU, 0xFADDU, 0x8284U, 0x0010U, 0x00D1U, 0x0192U, 0x8013U, 0x02A8U,
 0xC017U, 0x03C4U, 0xA772U, 0x03C8U, 0x4683U, 0x2BF2U, 0x022BU, 0xFADFU,
 0x02B0U, 0xBFF0U, 0x0237U, 0x03B0U, 0x2B88U, 0x4236U, 0x0AABU, 0xFADEU,
 0xBFF2U, 0x02B7U, 0x03B2U, 0x2B88U, 0x42B6U, 0x8221U, 0xE666U, 0x03A3U,
 0xF800U, 0x03E3U, 0xF8C8U, 0x02E0U, 0xC0C8U, 0x0AF6U, 0x003AU, 0x00BBU,
 0x003AU, 0x00BBU, 0x003AU, 0x00BBU, 0x003AU, 0x00BBU, 0xB3B3U, 0x87F7U,
 0x0210U, 0x02D1U, 0x0392U, 0x8213U, 0x8980U, 0x0290U, 0x00ABU, 0xFADFU,
 0x0291U, 0x00ABU, 0xFADEU, 0xA02BU, 0xFADDU, 0x8980U, 0x828FU, 0x0017U,
 0x0058U, 0x00D9U, 0x011AU, 0x015BU, 0x019CU, 0x801DU, 0x805EU, 0x8963U,
 0xF24DU, 0x01D6U, 0x03D0U, 0x2FCCU, 0x47C7U, 0x0BE0U, 0xC018U, 0x02FBU,
 0x46CFU, 0x2AC1U, 0x0AC1U, 0x00D5U, 0x03E0U, 0xC0C8U, 0xAF56U, 0x2BC1U,
 0xAFD4U, 0x8409U, 0x0214U, 0x0812U, 0xA7D2U, 0x8452U, 0x2215U, 0x0C10U,
 0x0200U, 0x0014U, 0xBFD4U, 0x844CU, 0x0237U, 0x0E14U, 0xBE12U, 0x0012U,
 0xBA00U, 0x8446U, 0xA02BU, 0xFADDU, 0x8923U, 0xF491U, 0xC014U, 0xC056U,
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
 0x805EU, 0x0215U, 0x1214U, 0x8963U, 0xF24DU, 0x01D6U, 0x03D0U, 0x2FCCU,
 0x47C7U, 0x0BE0U, 0xC018U, 0x02FBU, 0x46CFU, 0x2AC1U, 0x4AC1U, 0x00D5U,
 0xA716U, 0x0281U, 0x03E0U, 0xC280U, 0x2FF2U, 0xBFF0U, 0x87D9U, 0xAFF0U,
 0x840BU, 0x03D0U, 0x2BC5U, 0x47C7U, 0x1FE3U, 0xFFFDU, 0x02D5U, 0x2AF7U,
 0x0AF0U, 0xB6C0U, 0x87CDU, 0x2A32U, 0x4620U, 0xC3FFU, 0x02E3U, 0xFC00U,
 0x44D1U, 0x4011U, 0x876AU, 0x828EU, 0x0016U, 0x0057U, 0x00D8U, 0x0119U,
 0x015AU, 0x019BU, 0x801CU, 0x805DU, 0x8963U, 0xF24DU, 0x01D5U, 0x03E0U,
 0xC0C8U, 0xAF55U, 0x2BC1U, 0xAFD4U, 0x840AU, 0x0214U, 0x0812U, 0xA7D2U,
 0x845CU, 0x2A01U, 0x4C11U, 0x0C90U, 0x0200U, 0x0014U, 0xBFD4U, 0x8455U,
 0x0237U, 0x0E14U, 0xBE12U, 0x0012U, 0xBA00U, 0x844FU, 0xA02BU, 0xFADDU,
 0x8923U, 0xF491U, 0xC014U, 0xC055U, 0x8923U, 0xF0B1U, 0xC002U, 0xC010U,
 0xC051U, 0x8260U, 0xD111U, 0x0323U, 0xF000U, 0x0B14U, 0x0374U, 0x0B60U,
 0xC190U, 0x0912U, 0x02F7U, 0x03A0U, 0xC039U, 0x03E0U, 0xC03FU, 0xA413U,
 0x841DU, 0x8221U, 0xC48CU, 0x0228U, 0xC037U, 0x0268U, 0xC037U, 0x02B8U,
 0xBAB9U, 0x8410U, 0x2A85U, 0x10BAU, 0x08BAU, 0x003BU, 0xA93AU, 0x007BU,
 0x00BAU, 0x0281U, 0x08B8U, 0x48FAU, 0x08A8U, 0xC038U, 0xB312U, 0x87ECU,
 0x841CU, 0x0B01U, 0x87F9U, 0x0B41U, 0x8413U, 0x8221U, 0xC4C8U, 0x0228U,
 0xC037U, 0x0268U, 0xC037U, 0x02B9U, 0xBAB8U, 0x87F6U, 0x0E81U, 0x00B9U,
 0x2A85U, 0x10BAU, 0x08BAU, 0x003BU, 0xA93AU, 0x007BU, 0x00BAU, 0x48FAU,
 0x08A8U, 0xC038U, 0xB312U, 0x87ECU, 0x0216U, 0x0257U, 0x02D8U, 0x0319U,
 0x035AU, 0x039BU, 0x821CU, 0x825DU, 0x8980U
};



/* 0xFA00 - 0xFAFF initializers. First byte is offset low, then high & low of data */
const uint8  rrpge_m_ulib_idat[RRPGE_M_ULIB_ICNT * 3U] = {
 0xEEU, 0xFAU, 0xE0U,
 0xEFU, 0xFAU, 0xF2U,
 0xF0U, 0xF0U, 0x6CU,
 0xF1U, 0xF0U, 0x6EU,
};
