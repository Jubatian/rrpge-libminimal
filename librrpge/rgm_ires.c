/**
**  \file
**  \brief     Initialization resources & preparation
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#include "rgm_ires.h"
#include "rgm_ulib.h"


/* State: Nonzero elements in the VARS area (address, data high, data low) */
#define STANZ_CT 35U
static const uint8  rrpge_m_ires_stanz[3U * STANZ_CT] = {
 0x48U, 0x66U, 0x66U, /* XM register */
 0x55U, 0x07U, 0xF8U, /* GDG display list definition latch */
 0x95U, 0x05U, 0x58U, /* Mixer partitioning */
 0x99U, 0x01U, 0x00U, /* Mixer amplitudo */
 0xA0U, 0xFFU, 0xFFU, /* Accelerator write mask high */
 0xA1U, 0xFFU, 0xFFU, /* Accelerator write mask low */
 0xC4U, 0xFFU, 0x80U, /* Audio left buffer */
 0xC5U, 0xFFU, 0x80U, /* Audio right buffer */
 0xC6U, 0xFFU, 0xC0U, /* Audio buffer size mask */
 0xC7U, 0x00U, 0x01U, /* Audio divider */
 0xC8U, 0x1FU, 0xFCU, /* Mixer FIFO location & size */
 0xCCU, 0x4FU, 0xE0U, /* Graphics FIFO location & size */
 0xD0U, 0x01U, 0x02U, /* GDG mask / ckey 0 */
 0xD1U, 0x04U, 0x08U, /* GDG mask / ckey 1 */
 0xD2U, 0x10U, 0x20U, /* GDG mask / ckey 2 */
 0xD3U, 0x40U, 0x80U, /* GDG mask / ckey 3 */
 0xD4U, 0x50U, 0x00U, /* GDG shift mode region A */
 0xD5U, 0x50U, 0x00U, /* GDG shift mode region B */
 0xD7U, 0x37U, 0xF8U, /* GDG display list definition */
 0xD8U, 0x00U, 0x82U, /* GDG source A0 */
 0xD9U, 0x41U, 0x40U, /* GDG source A1 */
 0xDAU, 0x81U, 0x40U, /* GDG source A2 */
 0xDBU, 0xC1U, 0x40U, /* GDG source A3 */
 0xDCU, 0x02U, 0x60U, /* GDG source B0 */
 0xDDU, 0x82U, 0x60U, /* GDG source B1 */
 0xDEU, 0x03U, 0x60U, /* GDG source B2 */
 0xDFU, 0x83U, 0x60U, /* GDG source B3 */
 0xE3U, 0x00U, 0x01U, /* Pointer 0 increment */
 0xEBU, 0x00U, 0x04U, /* Pointer 1 increment */
 0xECU, 0x00U, 0x02U, /* Pointer 1 size */
 0xF3U, 0x00U, 0x08U, /* Pointer 2 increment */
 0xF4U, 0x00U, 0x03U, /* Pointer 2 size */
 0xFBU, 0x00U, 0x10U, /* Pointer 3 increment */
 0xFCU, 0x00U, 0x04U  /* Pointer 3 size */
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
 0x35EU, 0x85FU, 0x430U, 0xCC4U, 0x6AFU, 0x073U, 0x730U, 0xA83U,

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



/* Write a byte to 32 bit memory */
static void rrpge_m_ires_setb(uint32* d, auint o, auint v)
{
 auint oh = o >> 2;
 auint ol = ((o & 3U) ^ 3U) << 3;

 d[oh] = ((d[oh] & (~((auint)(0xFFU) << ol))) | ((v & 0xFFU) << ol)) & 0xFFFFFFFFU;
}



/* Initializes library specific portions of the emulation instance. */
static void rrpge_m_ires_initl(rrpge_object_t* obj)
{
 /* Note: breakpoints are not affected! */

 /* Network packets: none */
 obj->rebr = 0;
 obj->rebw = 0;
 obj->reir = 0;
 obj->reiw = 0;

 /* Rendering enabled */
 obj->rena = 0x3U;                   /* Enabled, and requests enabling */

 /* Kernel tasks not started */
 obj->tsfl = 0;

 /* No halt causes */
 obj->hlt = 0;

 /* No audio events waiting for servicing, double buffer empty */
 obj->aco = 0;
 obj->audp = 0;

 /* Kernel internal task cycles: set up to start with a reasonable amount of
 ** free time first. */
 obj->kfc = 400U * 64U;              /* 64 lines */
}



/* Initializes the CPU data memory into the 'dini' member of the object. This
** should be called before loading the application binary so data memory
** portions not loaded from the binary are adequately initialized */
void rrpge_m_ires_initdata(rrpge_object_t* obj)
{
 auint   i;
 auint   j;
 auint   t;
 auint   r;
 uint16* d = &(obj->dini[0]);

 /* Reset data memory initializer */

 for (i = 0U; i < (sizeof(obj->dini) / sizeof(obj->dini[0])); i++){
  d[i] = 0U;
 }

 /* Populate it */

 /* Color palette */
 for (i = 0U; i < 256U; i++){
  d[0xFB00U + i] = rrpge_m_ires_pal[i];
 }

 /* Musical logarithmic table */
 j = 12U;
 t = 0U;
 i = 256U;
 do{
  i--;
  j--;
  r = rrpge_m_ires_ft[j] >> t;
  d[0xFC00U + (i << 1)] = (r >> 16) & 0xFFFFU;
  d[0xFC01U + (i << 1)] = (r      ) & 0xFFFFU;
  if (j == 0U){
   t++;
   j = 12U;
  }
 }while (i != 0U);

 /* Large sine table */
 d[0xFE00U] = 0x0000U;
 d[0xFE80U] = 0x4000U;
 d[0xFF00U] = 0x0000U;
 d[0xFF80U] = 0xC000U;
 for (i = 1U; i < 128U; i++){
  d[0xFE00U + i] = rrpge_m_ires_sinq16[i];
 }
 for (i = 1U; i < 128U; i++){
  d[0xFE80U + i] = rrpge_m_ires_sinq16[128U - i];
 }
 for (i = 1U; i < 128U; i++){
  d[0xFF00U + i] = 0x0000U - rrpge_m_ires_sinq16[i];
 }
 for (i = 1U; i < 128U; i++){
  d[0xFF80U + i] = 0x0000U - rrpge_m_ires_sinq16[128U - i];
 }

 /* User Library initializers */
 for (i = 0U; i < RRPGE_M_ULIB_ICNT; i++){
  d[0xFA00U + (rrpge_m_ulib_idat[i * 3U] & 0xFFU)] =
      ((rrpge_m_ulib_idat[(i * 3U) + 1U] & 0xFFU) << 8) +
      ((rrpge_m_ulib_idat[(i * 3U) + 2U] & 0xFFU));
 }

}



/* Initializes the Code memory, filling it with zero and the User Library from
** 0xF000. This should be called before loading the Application's code. */
void rrpge_m_ires_initcode(rrpge_object_t* obj)
{
 auint   i;
 uint16* c = &(obj->crom[0]);

 /* Reset code memory */

 for (i = 0U; i < (sizeof(obj->crom) / sizeof(obj->crom[0])); i++){
  c[i] = 0U;
 }

 /* Fill in User Library */

 for (i = 0U; i < RRPGE_M_ULIB_SIZE; i++){
  c[0xF000U + i] = rrpge_m_ulib[i];
 }
}



/* Initializes state after the application header and descriptor were loaded.
** This should be used in initialization, so the complete header may be
** checked at once. */
void rrpge_m_ires_initstat(rrpge_object_t* obj)
{
 auint   i;
 uint16 *s = &(obj->st.stat[0]);

 /* Reset Application State */

 for (i = 0U; i < (sizeof(obj->st.stat) / sizeof(obj->st.stat[0])); i++){
  s[i] = 0U;
 }

 /* Populate nonzero members of Application State */

 /* Stuff in the VARS area */
 for (i = 0U; i < STANZ_CT; i++){
  s[rrpge_m_ires_stanz[i * 3U]] = ((rrpge_m_ires_stanz[(i * 3U) + 1U] & 0xFFU) << 8) +
                                  ((rrpge_m_ires_stanz[(i * 3U) + 2U] & 0xFFU));
 }

 /* Color palette */
 for (i = 0U; i < 256U; i++){
  s[RRPGE_STA_PAL + i] = rrpge_m_ires_pal[i];
 }


 /* Add application header and descriptor elements */

 for (i = 0U; i < 64U; i++){
  s[i] = obj->apph[i];
 }
 s[RRPGE_STA_VARS + 0x18U] = obj->appd[0x0U];
 s[RRPGE_STA_VARS + 0x19U] = obj->appd[0x1U];
 s[RRPGE_STA_VARS + 0x1AU] = obj->appd[0x8U];
 s[RRPGE_STA_VARS + 0x1BU] = obj->appd[0x9U];
 s[RRPGE_STA_VARS + 0x1CU] = obj->appd[0xAU];
 s[RRPGE_STA_VARS + 0x1DU] = obj->appd[0xBU];
}



/* Initializes starting resources for an RRPGE emulator object after an
** application was loaded. This should be used before starting emulation or
** when resetting it. Does not depend on state correctness, so a state check
** is not necessary before calling. */
void rrpge_m_ires_init(rrpge_object_t* obj)
{
 auint   i;
 auint   r;
 uint32 *p = &(obj->st.pram[0]);


 /* Reset memories */

 for (i = 0U; i < (sizeof(obj->dini   ) / sizeof(obj->dini[0]   )); i++){
  obj->st.dram[i] = obj->dini[i];
 }
 for (      ; i < (sizeof(obj->st.dram) / sizeof(obj->st.dram[0])); i++){
  obj->st.dram[i] = 0U;
 }

 for (i = 0U; i < RRPGE_M_PRAMS; i++){
  p[i] = 0U;
 }

 rrpge_m_ires_initstat(obj);


 /* Populate Peripheral RAM with initial data blocks */

 /* 0xFFE00 - 0xFFE3F: 50% square wave */
 for (i = 0U; i < 0x80U; i++){
  rrpge_m_ires_setb(p, (0xFFE00U << 2) + i, 0xFFU);
 }
 for (i = 0U; i < 0x80U; i++){
  rrpge_m_ires_setb(p, (0xFFE20U << 2) + i, 0x00U);
 }

 /* 0xFFE40 - 0xFFE7F: Sine */
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFE40U << 2) + i, rrpge_m_ires_sinq8[i]);
 }
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFE50U << 2) + i, rrpge_m_ires_sinq8[0x3FU - i]);
 }
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFE60U << 2) + i, 0xFFU - rrpge_m_ires_sinq8[i]);
 }
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFE70U << 2) + i, 0xFFU - rrpge_m_ires_sinq8[0x3FU - i]);
 }

 /* 0xFFE80 - 0xFFEBF: Triangle */
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFE80U << 2) + i, (i << 1) + 0x80U);
 }
 for (i = 0U; i < 0x80U; i++){
  rrpge_m_ires_setb(p, (0xFFE90U << 2) + i, ((0x7FU - i) << 1) + 0x01U);
 }
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFEB0U << 2) + i, (i << 1));
 }

 /* 0xFFEC0 - 0xFFEFF: Spikes */
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFEC0U << 2) + i, 0x80U + (0xFFU - rrpge_m_ires_sinq8[0x3FU - i]));
 }
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFED0U << 2) + i, 0x80U + (0xFFU - rrpge_m_ires_sinq8[i]));
 }
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFEE0U << 2) + i, rrpge_m_ires_sinq8[0x3FU - i] - 0x80U);
 }
 for (i = 0U; i < 0x40U; i++){
  rrpge_m_ires_setb(p, (0xFFEF0U << 2) + i, rrpge_m_ires_sinq8[i] - 0x80U);
 }

 /* 0xFFF00 - 0xFFF3F: Sawtooth, incremental */
 for (i = 0U; i < 0x100U; i++){
  rrpge_m_ires_setb(p, (0xFFF00U << 2) + i, i);
 }

 /* 0xFFF40 - 0xFFF7F: Sawtooth, decremental */
 for (i = 0U; i < 0x100U; i++){
  rrpge_m_ires_setb(p, (0xFFF40U << 2) + i, 0xFFU - i);
 }

 /* 0xFFF80 - 0xFFFBF: Noise 1 */
 r = 0U;
 for (i = 0U; i < 0x100U; i++){
  rrpge_m_ires_setb(p, (0xFFF80U << 2) + i, r);
  r = (((r >> 7) + (r + r) + 0xBBU) ^ 0x7FU) & 0xFFU;
 }

 /* 0xFFFC0 - 0xFFFFF: Noise 2 */
 r = 0U;
 for (i = 0U; i < 0x100U; i++){
  rrpge_m_ires_setb(p, (0xFFFC0U << 2) + i, r);
  r = (((r >> 7) + (r + r) + 0xA3U) ^ 0xB3U) & 0xFFU;
 }


 /* Populate Peripheral RAM with boot data blocks */

 /* 0xFF000 - 0xFF7FF: Display list (only first 1600 entries used) */
 for (i = 0U; i < 200U; i++){
  p[0xFF000U + (i * 8U) + 1U] = 0x0000C000U + (i * 0x50000U);
 }

 /* 0xFF800 - 0xFFBFF: Audio buffers (silence) */
 for (i = 0U; i < 1024U; i++){
  p[0xFF800U + i] = 0x80808080U;
 }


 /* Prepare emulation library specific flags for a proper start */

 rrpge_m_ires_initl(obj);
}
