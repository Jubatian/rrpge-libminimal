/**
**  \file
**  \brief     Initialization resources & preparation
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.27
*/


#include "rgm_ires.h"


/* ROPD, CPU Read & Write pages */
static const uint16 rrpge_m_ires_cpurw[32] = {
 0x40E0U, 0x7FFFU, 0xBFFFU, 0x4001U, 0x4002U, 0x4003U, 0x4004U, 0x4005U,
 0x8000U, 0x8001U, 0x8002U, 0x8003U, 0x8004U, 0x8005U, 0x8006U, 0x8007U,
 0x7FFFU, 0x7FFFU, 0xBFFFU, 0x4001U, 0x4002U, 0x4003U, 0x4004U, 0x4005U,
 0x8000U, 0x8001U, 0x8002U, 0x8003U, 0x8004U, 0x8005U, 0x8006U, 0x8007U
};

/* 1st quarter for the wave sine table */
static const uint8  rrpge_m_ires_sinq8[64] = {
 0x81U, 0x84U, 0x87U, 0x8AU, 0x8EU, 0x91U, 0x94U, 0x97U,
 0x9AU, 0x9DU, 0xA0U, 0xA3U, 0xA6U, 0xA9U, 0xACU, 0xAFU,
 0xB2U, 0xB5U, 0xB7U, 0xBAU, 0xBDU, 0xC0U, 0xC2U, 0xC5U,
 0xC8U, 0xCAU, 0xCDU, 0xCFU, 0xD2U, 0xD4U, 0xD6U, 0xD9U,
 0xDBU, 0xDDU, 0xDFU, 0xE1U, 0xE3U, 0xE5U, 0xE7U, 0xE9U,
 0xEAU, 0xECU, 0xEEU, 0xEFU, 0xF1U, 0xF2U, 0xF3U, 0xF5U,
 0xF6U, 0xF7U, 0xF8U, 0xF9U, 0xFAU, 0xFBU, 0xFCU, 0xFCU,
 0xFDU, 0xFDU, 0xFEU, 0xFEU, 0xFFU, 0xFFU, 0xFFU, 0xFFU
};

/* 1st quarter for the hq (512 element, 2 complement) sine table */
static const uint16 rrpge_m_ires_sinq16[128] = {
 0x0000U, 0x00C9U, 0x0192U, 0x025BU, 0x0323U, 0x03ECU, 0x04B5U, 0x057DU,
 0x0645U, 0x070DU, 0x07D5U, 0x089CU, 0x0964U, 0x0A2AU, 0x0AF1U, 0x0BB6U,
 0x0C7CU, 0x0D41U, 0x0E05U, 0x0EC9U, 0x0F8CU, 0x104FU, 0x1111U, 0x11D3U,
 0x1294U, 0x1354U, 0x1413U, 0x14D1U, 0x158FU, 0x164CU, 0x1708U, 0x17C3U,
 0x187DU, 0x1937U, 0x19EFU, 0x1AA6U, 0x1B5DU, 0x1C12U, 0x1CC6U, 0x1D79U,
 0x1E2BU, 0x1EDCU, 0x1F8BU, 0x2039U, 0x20E7U, 0x2192U, 0x223DU, 0x22E6U,
 0x238EU, 0x2434U, 0x24DAU, 0x257DU, 0x261FU, 0x26C0U, 0x275FU, 0x27FDU,
 0x2899U, 0x2934U, 0x29CDU, 0x2A65U, 0x2AFAU, 0x2B8EU, 0x2C21U, 0x2CB2U,
 0x2D41U, 0x2DCEU, 0x2E5AU, 0x2EE3U, 0x2F6BU, 0x2FF1U, 0x3076U, 0x30F8U,
 0x3179U, 0x31F7U, 0x3274U, 0x32EEU, 0x3367U, 0x33DEU, 0x3453U, 0x34C6U,
 0x3536U, 0x35A5U, 0x3612U, 0x367CU, 0x36E5U, 0x374BU, 0x37AFU, 0x3811U,
 0x3871U, 0x38CFU, 0x392AU, 0x3983U, 0x39DAU, 0x3A2FU, 0x3A82U, 0x3AD2U,
 0x3B20U, 0x3B6CU, 0x3BB6U, 0x3BFDU, 0x3C42U, 0x3C84U, 0x3CC5U, 0x3D02U,
 0x3D3EU, 0x3D77U, 0x3DAEU, 0x3DE2U, 0x3E14U, 0x3E44U, 0x3E71U, 0x3E9CU,
 0x3EC5U, 0x3EEBU, 0x3F0EU, 0x3F2FU, 0x3F4EU, 0x3F6AU, 0x3F84U, 0x3F9CU,
 0x3FB1U, 0x3FC3U, 0x3FD3U, 0x3FE1U, 0x3FECU, 0x3FF4U, 0x3FFBU, 0x3FFEU
};

/* High 12 entries for the audio frequency table */
static const uint32 rrpge_m_ires_ft[12] = {
  55678343U,  58989149U,  62496826U,  66213081U,  70150316U,  74321671U,
  78741067U,  83423255U,  88383859U,  93639437U,  99207528U, 105106715U
};

/* Palette */
static const uint16 rrpge_m_ires_pal[256] = {

 0x000U, 0xAAAU, 0x555U, 0xFFFU, 0x229U, 0x593U, 0xA42U, 0xEDBU,
 0x35EU, 0x85FU, 0x430U, 0xBC4U, 0x6AFU, 0x073U, 0x730U, 0xA83U,

 0x000U, 0x777U, 0x222U, 0xCCCU, 0x24AU, 0x6B1U, 0xD84U, 0x73DU,
 0x38EU, 0xEABU, 0x760U, 0x140U, 0x8CFU, 0x180U, 0xB68U, 0xBA4U,

 0x025U, 0x6AAU, 0x197U, 0xFF6U, 0x009U, 0x790U, 0xC53U, 0xED5U,
 0x057U, 0x62AU, 0x300U, 0x9A2U, 0x68EU, 0x450U, 0x602U, 0x708U,
 0x407U, 0x479U, 0xA18U, 0x8F6U, 0x079U, 0x6D4U, 0xEA5U, 0xA3BU,
 0x2BBU, 0xF6FU, 0x533U, 0x250U, 0x5ECU, 0x490U, 0xB4DU, 0xC6FU,

 0x65EU, 0xE4CU, 0xA60U, 0x0A1U, 0x20AU, 0xA6EU, 0xA20U, 0x51BU,
 0x020U, 0x9FFU, 0x900U, 0xA8AU, 0x748U, 0x1B5U, 0xDEFU, 0x356U,
 0x90AU, 0x772U, 0x5CFU, 0x279U, 0xBE4U, 0x102U, 0x8D3U, 0x427U,
 0x037U, 0x305U, 0x940U, 0x884U, 0x3D6U, 0x650U, 0x3BDU, 0x04AU,
 0x29BU, 0x3C1U, 0xD28U, 0x18CU, 0xDA2U, 0xC81U, 0x81CU, 0xDC2U,
 0xA3EU, 0x16CU, 0xA1CU, 0xAB0U, 0x2DDU, 0x627U, 0xDF5U, 0xB9FU,
 0x470U, 0x509U, 0xB70U, 0x574U, 0xC15U, 0x43EU, 0x045U, 0x733U,
 0x68AU, 0x955U, 0xFB4U, 0x7FEU, 0xE39U, 0x990U, 0xD63U, 0xD9FU,

 0x906U, 0x3F9U, 0x70CU, 0x2AEU, 0x92EU, 0x7F3U, 0x614U, 0xAC9U,
 0xD3FU, 0x6D1U, 0x7BBU, 0x0BCU, 0xF77U, 0xF63U, 0xBF7U, 0x5A7U,
 0xF4FU, 0xC1AU, 0x06AU, 0xCA7U, 0x30CU, 0x031U, 0xC40U, 0xFAFU,
 0x274U, 0x3A8U, 0x84AU, 0x1D7U, 0x02CU, 0x9A6U, 0x95BU, 0x382U,
 0xCC7U, 0xE1DU, 0x13EU, 0x27FU, 0xAD0U, 0xDE1U, 0x62FU, 0xF82U,
 0x440U, 0x088U, 0x972U, 0x31DU, 0x415U, 0xACEU, 0x0C0U, 0x7B5U,
 0xFB8U, 0x0AAU, 0x9F2U, 0x500U, 0x242U, 0xD01U, 0xDF9U, 0x00DU,
 0x827U, 0x8AFU, 0x204U, 0xB93U, 0x5A0U, 0x2FFU, 0x487U, 0xB1EU,
 0x5FFU, 0x05EU, 0xE50U, 0x51FU, 0x1F9U, 0xA0EU, 0x3E0U, 0xEB0U,
 0x5F1U, 0x247U, 0xFF1U, 0x330U, 0xC88U, 0x0ECU, 0x861U, 0x969U,
 0xA29U, 0xD90U, 0x0CEU, 0x751U, 0x73AU, 0xB02U, 0x253U, 0xB8DU,
 0x460U, 0xF11U, 0xF8BU, 0x0D3U, 0x70AU, 0xD0EU, 0xC76U, 0x896U,
 0x077U, 0x09FU, 0xF30U, 0xF60U, 0x80FU, 0x0FFU, 0x07FU, 0x10FU,
 0x934U, 0x638U, 0xEFCU, 0x57FU, 0x757U, 0x212U, 0x207U, 0x05BU,
 0x437U, 0xA4FU, 0x32FU, 0x423U, 0x068U, 0x46BU, 0x57BU, 0x9C7U,
 0x98FU, 0x028U, 0x96CU, 0x7F0U, 0x780U, 0xA0BU, 0x359U, 0x0AEU
};



/* Read a byte from word memory */
/* !!! Left unused for now !!! */
static uint8 rrpge_m_ires_getb(uint16 const* d, auint o)
{
 return (uint8)(d[o >> 1] >> (((o & 1U) ^ 1U) << 3));
}



/* Write a byte to word memory */
static void rrpge_m_ires_setb(uint16* d, auint o, uint8 v)
{
 auint oh = o >> 1;
 auint ol = ((o & 1U) ^ 1U) << 3;

 d[oh] = (uint16)((d[oh] & (0xFF00U >> ol)) | ((auint)(v) << ol));
}



/* Initializes graphics peripheral as required after arbitrary file loads and
** saves (and also as required when resetting). This includes the peripheral
** area, zeroing the first 32 video RAM pages, and resetting all cycle
** counters & display list data (0xD50 - 0xD5F area). */
void rrpge_m_ires_initg(rrpge_object_t* obj)
{
 auint i;

 /* Reset all video peripheral related to zero */
 for (i = 0xEE0U; i < 0x1000U; i++){
  obj->stat.ropd[i] = 0;
 }

 /* Init video peripheral data where necessary */
 obj->stat.ropd[0xEE0U] = 0xFFFFU; /* Write mask high */
 obj->stat.ropd[0xEE1U] = 0xFFFFU; /* Write mask low */
 obj->stat.ropd[0xEE2U] = 0x0007U; /* Partition size */
 obj->stat.ropd[0xEE3U] = 0x01FEU; /* Bg. display list */
 obj->stat.ropd[0xEE4U] = 0x01FDU; /* Layer 0 disp. list */
 obj->stat.ropd[0xEE5U] = 0x01FAU; /* Layer 1 disp. list */
 obj->stat.ropd[0xEE6U] = 0x01FCU; /* Layer 2 disp. list */
 obj->stat.ropd[0xEE7U] = 0x01FBU; /* Layer 3 disp. list */

 /* Reset cycle counters & display list */
 for (i = 0xD50U; i < 0xD60U; i++){
  obj->stat.ropd[i] = 0;
 }

 /* Init where necessary */
 obj->stat.ropd[0xD50U] = 0x0190U; /* Start at 400 decimal (VBlank) */

 /* Clear first 32 pages of VRAM */
 for (i = 0U * 2048U; i < 32U * 2048U; i++){
  obj->stat.vram[i] = 0;
 }
}



/* Initializes library specific portions of the emulation instance, also used
** when loading state. */
void rrpge_m_ires_initl(rrpge_object_t* obj)
{
 /* Note: breakpoints are not affected! */

 /* Network packets: none */
 obj->rebr = 0;
 obj->rebw = 0;
 obj->reir = 0;
 obj->reiw = 0;

 /* Forward renderer */
 obj->frln = obj->stat.ropd[0xD50U]; /* At current line */
 obj->frld = 0;

 /* Rendering enabled */
 obj->rena = 0x3U;                   /* Enabled, and requests enabling */

 /* Kernel tasks not started */
 obj->tsfl = 0;

 /* No halt causes */
 obj->hlt = 0;

 /* No audio events waiting for servicing */
 obj->aco = 0;

 /* Kernel internal task cycles: set up to start with a reasonable amount of
 ** free time first. */
 obj->kfc = 400U * 64U;              /* 64 lines */
}



/* Initializes starting resources for an RRPGE emulator object. This includes
** areas of the ROPD and data memory areas. It does not depend on the
** application loaded or to be loaded. Keeps user area of ROPD intact. */
void rrpge_m_ires_init(rrpge_object_t* obj)
{
 auint i;
 auint j;
 uint8 r;

 /* Reset ROPD areas (excluding user area) */
 for (i = 0xC00U; i < 0xE80U; i++){
  obj->stat.ropd[i] = 0;
 }
 for (i = 0xEAAU; i < 0x1000U; i++){
  obj->stat.ropd[i] = 0;
 }

 /* Reset memories */
 for (i =  0U * 4096U; i < 224U * 4096U; i++){
  obj->stat.dram[i] = 0;
 }
 for (i =  0U * 4096U; i <   8U * 4096U; i++){
  obj->stat.sram[i] = 0;
 }
 for (i = 32U * 2048U; i < 128U * 2048U; i++){
  obj->stat.vram[i] = 0;
 }

 /* Init video */
 rrpge_m_ires_initg(obj);


 /* Populate Data RAM page 0 */

 /* Fill in first half of it with silence */
 for (i = 0U; i < 0x800U; i++){
  obj->stat.dram[i] = 0x8080U;
 }

 /* 0x800 - 0x87F: 50% square wave */
 for (i = 0x1000U; i < 0x1080U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, 0xFFU);
 }
 for (i = 0x1080U; i < 0x1100U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, 0x00U);
 }

 /* 0x880 - 0x8FF: Sine */
 for (i = 0x1100U; i < 0x1140U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, rrpge_m_ires_sinq8[i - 0x1100U]);
 }
 for (i = 0x1140U; i < 0x1180U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, rrpge_m_ires_sinq8[0x117FU - i]);
 }
 for (i = 0x1180U; i < 0x11C0U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, 0xFFU - rrpge_m_ires_sinq8[i - 0x1180U]);
 }
 for (i = 0x11C0U; i < 0x1200U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, 0xFFU - rrpge_m_ires_sinq8[0x11FFU - i]);
 }

 /* 0x900 - 0x97F: Triangle */
 for (i = 0x1200U; i < 0x1240U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, ((i - 0x1200U) << 1) + 0x80U);
 }
 for (i = 0x1240U; i < 0x12C0U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, ((0x12BFU - i) << 1) + 0x01U);
 }
 for (i = 0x12C0U; i < 0x1300U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, ((i - 0x12C0U) << 1));
 }

 /* 0x900 - 0x9FF: Spikes */
 for (i = 0x1300U; i < 0x1340U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, 0x80U + 0xFFU - rrpge_m_ires_sinq8[0x133FU - i]);
 }
 for (i = 0x1340U; i < 0x1380U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, 0x80U + 0xFFU - rrpge_m_ires_sinq8[i - 0x1340U]);
 }
 for (i = 0x1380U; i < 0x13C0U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, rrpge_m_ires_sinq8[0x13BFU - i] - 0x80U);
 }
 for (i = 0x13C0U; i < 0x1400U; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, rrpge_m_ires_sinq8[i - 0x13C0U] - 0x80U);
 }

 /* 0xA00 - 0xA7F: Sawtooth, incremental */
 for (i = 0x1400U; i < 0x14FFU; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, i - 0x1400U);
 }

 /* 0xA80 - 0xAFF: Sawtooth, decremental */
 for (i = 0x1500U; i < 0x15FFU; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, 0x15FFU - i);
 }

 /* 0xB00 - 0xB7F: Noise 1 */
 r = 0U;
 for (i = 0x1600U; i < 0x16FFU; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, r);
  r = (uint8)(((r >> 7) + (r + r) + 0xBBU) ^ 0x7FU);
 }

 /* 0xB80 - 0xBFF: Noise 2 */
 r = 0U;
 for (i = 0x1700U; i < 0x17FFU; i++){
  rrpge_m_ires_setb(&(obj->stat.dram[0]), i, r);
  r = (uint8)(((r >> 7) + (r + r) + 0xA3U) ^ 0xB3U);
 }

 /* 0xC00 - 0xCFF: Frequency table, whole parts (not initializing all, rest is
 ** just left being zero from the initial zeroing). */
 for (j = 0U; j < 16U; j++){
  for (i = 0U; i < 12U; i++){
   obj->stat.dram[i + (0xCF4U - (j * 12U))] = (uint16)(rrpge_m_ires_ft[i] >> (16 + j));
  }
 }

 /* 0xD00 - 0xDFF: Frequency table, fractional parts */
 for (j = 0U; j < 21U; j++){
  for (i = 0U; i < 12U; i++){
   obj->stat.dram[i + (0xDF4U - (j * 12U))] = (uint16)(rrpge_m_ires_ft[i] >> j);
  }
 }
 for (i = 8U; i < 12U; i++){
  obj->stat.dram[i + (0xDF4U - (21U * 12U))] = (uint16)(rrpge_m_ires_ft[i] >> 21);
 }


 /* Populate Video RAM pages (initial display lists) */

 /* Background display list at 127:0x800 */
 for (i = 0U; i < 400U; i++){
  obj->stat.vram[127U * 2048U + 0x400U + i] = 0x60100000U;
 }

 /* Layer display lists. Odd lines are zero, not initializing those (they are
 ** left zeroed). */
 obj->stat.vram[127U * 2048U + 0x200U] = 0x00000100U;
 obj->stat.vram[126U * 2048U + 0x400U] = 0x00000100U;
 obj->stat.vram[127U * 2048U + 0x000U] = 0x00000100U;
 obj->stat.vram[126U * 2048U + 0x600U] = 0x00000100U;
 for (i = 2; i < 400; i+=2){
  obj->stat.vram[127U * 2048U + 0x200U + i] = 0x00500000U;
  obj->stat.vram[126U * 2048U + 0x400U + i] = 0x00500000U;
  obj->stat.vram[127U * 2048U + 0x000U + i] = 0x00500000U;
  obj->stat.vram[126U * 2048U + 0x600U + i] = 0x00500000U;
 }


 /* Set up audio mixer */
 obj->stat.ropd[0xED3U] = 0x0100U; /* Amplitudo: max (multiplier off) */
 obj->stat.ropd[0xEDAU] = 0x000CU; /* Freq. table whole pointer */
 obj->stat.ropd[0xEDBU] = 0x000DU; /* Freq. table fraction pointer */
 obj->stat.ropd[0xEDCU] = 0x6667U; /* Partitioning */


 /* Populate ROPD areas (only the CPU R/W address space, rest are zero) */
 for (i = 0U; i < 32U;  i++){
  obj->stat.ropd[0xD00U + i] = rrpge_m_ires_cpurw[i];
 }

 /* Populate ROPD constant areas and color palette */

 /* Color palette */
 for (i = 0U; i < 256U; i++){
  obj->stat.ropd[0xC00U + i] = rrpge_m_ires_pal[i];
 }

 /* 0xD40 - 0xD7F: Color palette low 64 colors */
 for (i = 0U; i < 64U;  i++){
  obj->ropc[0x000U + i] = rrpge_m_ires_pal[i];
 }

 /* 0xD80 - 0xDFF: Audio sine, copy from data page 0. */
 for (i = 0U; i < 128U; i++){
  obj->ropc[0x040U + i] = obj->stat.dram[0x880U + i];
 }

 /* 0xC00 - 0xFFF: HQ 512 element sine */
 obj->ropc[0x0C0U] = 0x0000U;
 obj->ropc[0x140U] = 0x4000U;
 obj->ropc[0x1C0U] = 0x0000U;
 obj->ropc[0x240U] = 0xC000U;
 for (i = 1U; i < 128U; i++){
  obj->ropc[0x0C0U + i] = rrpge_m_ires_sinq16[i];
 }
 for (i = 1U; i < 128U; i++){
  obj->ropc[0x140U + i] = rrpge_m_ires_sinq16[128U - i];
 }
 for (i = 1U; i < 128U; i++){
  obj->ropc[0x1C0U + i] = 0x0000U - rrpge_m_ires_sinq16[i];
 }
 for (i = 1U; i < 128U; i++){
  obj->ropc[0x240U + i] = 0x0000U - rrpge_m_ires_sinq16[128U - i];
 }


 /* Prepare emulation library specific flags for a proper start */

 rrpge_m_ires_initl(obj);
}
