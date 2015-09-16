/**
**  \file
**  \brief     Audio output manager, rrpge_getaudio() implementation.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.15
*/


#include "rgm_aud.h"
#include "rgm_halt.h"
#include "rgm_stat.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* State: Audio registers (0x0000 - 0x0007), reader */
RRPGE_M_FASTCALL static auint rrpge_m_aud_stat_regs_read(rrpge_object_t* hnd, auint adr, auint rmw)
{
 switch (adr){
  case 0x1U: return (hnd->aud.c187) & 0xFFFFU; break;
  case 0x2U: return (hnd->aud.dsct) & 0xFFFFU; break;
  case 0x3U: return (hnd->aud.dbcl) & 0xFFFFU; break;
  case 0x4U: return (hnd->aud.dlof) & 0xFFFFU; break;
  case 0x5U: return (hnd->aud.drof) & 0xFFFFU; break;
  case 0x6U: return (hnd->aud.dsiz) & 0xFFFFU; break;
  case 0x7U: return (hnd->aud.ddiv) & 0xFFFFU; break;
  default:   return 0U;                        break;
 }
}



/* State: Audio registers (0x0000 - 0x0007), writer */
RRPGE_M_FASTCALL static void rrpge_m_aud_stat_regs_write(rrpge_object_t* hnd, auint adr, auint val)
{
 switch (adr){
  case 0x4U: hnd->aud.dlof = val & 0xFFFFU; break;
  case 0x5U: hnd->aud.drof = val & 0xFFFFU; break;
  case 0x6U: hnd->aud.dsiz = val & 0xFFFFU; break;
  case 0x7U: hnd->aud.ddiv = val & 0xFFFFU; break;
  default:                                  break;
 }
}



/* State: Low audio registers (0x0000 - 0x0003), getter */
RRPGE_M_FASTCALL static auint rrpge_m_aud_stat_regs_get(rrpge_object_t* hnd, auint adr)
{
 switch (adr){
  case 0x1U: return (hnd->aud.c187) & 0xFFFFU; break;
  case 0x2U: return (hnd->aud.dsct) & 0xFFFFU; break;
  case 0x3U: return (hnd->aud.dbcl) & 0xFFFFU; break;
  default:   return 0U;                        break;
 }
}



/* State: Low audio registers (0x0000 - 0x0003), setter */
RRPGE_M_FASTCALL static void rrpge_m_aud_stat_regs_set(rrpge_object_t* hnd, auint adr, auint val)
{
 switch (adr){
  case 0x1U: hnd->aud.c187 = val & 0xFFFFU; break;
  case 0x2U: hnd->aud.dsct = val & 0xFFFFU; break;
  case 0x3U: hnd->aud.dbcl = val & 0xFFFFU; break;
  default:                                  break;
 }
}



/* State: Cycles until next 48KHz audio base clock tick reader */
RRPGE_M_FASTCALL static auint rrpge_m_aud_stat_read_rclk(rrpge_object_t* hnd, auint adr, auint rmw)
{
 return (hnd->aud.rclk & 0xFFFFU);
}



/* State: Cycles until next 48KHz audio base clock tick writer */
RRPGE_M_FASTCALL static void rrpge_m_aud_stat_write_rclk(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->aud.rclk = val & 0xFFFFU;
}



/* State: 48KHz audio base clock reader */
RRPGE_M_FASTCALL static auint rrpge_m_aud_stat_read_c48k(rrpge_object_t* hnd, auint adr, auint rmw)
{
 return (hnd->aud.c48k & 0xFFFFU);
}



/* State: 48KHz audio base clock writer */
RRPGE_M_FASTCALL static void rrpge_m_aud_stat_write_c48k(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->aud.c48k = val & 0xFFFFU;
}



/* Initializes audio emulation adding the appropriate handlers to the state
** manager. */
void rrpge_m_aud_init(void)
{
 rrpge_m_stat_add_rw_handler(&rrpge_m_aud_stat_regs_read, &rrpge_m_aud_stat_regs_write,
                             RRPGE_STA_UPA_A, 8U);
 rrpge_m_stat_add_ac_handler(&rrpge_m_aud_stat_regs_get,  &rrpge_m_aud_stat_regs_set,
                             RRPGE_STA_UPA_A, 4U);
 rrpge_m_stat_add_rw_handler(&rrpge_m_aud_stat_read_rclk, &rrpge_m_aud_stat_write_rclk,
                             RRPGE_STA_VARS + 0x13U, 1U);
 rrpge_m_stat_add_rw_handler(&rrpge_m_aud_stat_read_c48k, &rrpge_m_aud_stat_write_c48k,
                             RRPGE_STA_VARS + 0x14U, 1U);
}



/* Initializes audio emulation internal resources. */
void rrpge_m_aud_initres(rrpge_object_t* hnd)
{
 hnd->aud.bufp = 0U;
 hnd->aud.evct = 0U;
 hnd->aud.mclk = RRPGE_M_OSC; /* Main clock frequency: Use default rate. */
 hnd->aud.mclf = 0U;
}



/* Based on the cycles needing emulation, processes audio into the internal
** audio buffer, flagging the 512 sample passed event as required. */
void rrpge_m_aud_proc(rrpge_object_t* hnd, auint cy)
{
 auint lof = ((hnd->aud.dlof) << 4);
 auint rof = ((hnd->aud.drof) << 4);
 auint msk = ((hnd->aud.dsiz) << 4) & (PRAMS - 1U);
 auint t;

 lof &= msk;        /* Part to fetch from the start offset */
 rof &= msk;
 msk ^= PRAMS - 1U; /* Invert mask (staying in appropriate range) to use on next read offset */

 /* Consume the provided number of cycles */

 hnd->aud.rclk -= cy;

 /* Until the remaining cycles to next audio event is negative or zero,
 ** process */

 while ( ((hnd->aud.rclk & 0x80000000U) != 0U) ||
         (hnd->aud.rclk == 0U) ){

  /* One audio tick will be processed */

  hnd->aud.rclk += (hnd->aud.mclk) / 48000U;
  hnd->aud.mclf += (hnd->aud.mclk) % 48000U;
  if (hnd->aud.mclf >= 48000U){
   hnd->aud.mclf -= 48000U;
   hnd->aud.rclk ++;
  }

  /* Load samples from data memory into the internal double buffer */

  t = hnd->aud.dsct & 0xFFFFU;
  hnd->aud.bufl[(hnd->aud.bufp) & 0x3FFU] =
    ( hnd->st.pram[lof | ((t >> 1) & msk)] >> (((t & 1U) ^ 1U) << 4) ) & 0xFFFFU;
  hnd->aud.bufr[(hnd->aud.bufp) & 0x3FFU] =
    ( hnd->st.pram[rof | ((t >> 1) & msk)] >> (((t & 1U) ^ 1U) << 4) ) & 0xFFFFU;

  /* Increment pointers */

  hnd->aud.bufp ++;
  hnd->aud.dbcl = (hnd->aud.dbcl + 1U) & 0xFFFFU;
  if (hnd->aud.dbcl == hnd->aud.ddiv){
   hnd->aud.dbcl = 0U;
   hnd->aud.dsct = (hnd->aud.dsct + 1U) & 0xFFFFU;
  }

  /* Increment 187.5Hz clock */

  hnd->aud.c48k = (hnd->aud.c48k + 1U) & 0xFFFFU;
  if ((hnd->aud.c48k & 0xFFU) == 0U){
   hnd->aud.c187 =
       ((hnd->aud.c187 + 0x1U) & 0xFF00U) +
       ((hnd->aud.c48k >> 8) & 0xFFU);
  }

  /* Generate an event every 512 output (48KHz) samples */

  if ((hnd->aud.bufp & 0x1FFU) == 0U){
   hnd->aud.evct ++;
   rrpge_m_halt_set(hnd, RRPGE_HLT_AUDIO);
  }

 }

}



/* Get audio events and streams - implementation of RRPGE library function */
rrpge_iuint rrpge_getaudio(rrpge_object_t* hnd, rrpge_uint16* lbuf, rrpge_uint16* rbuf)
{
 auint  r;
 auint  i;
 uint16 const* pl;
 uint16 const* pr;

 if (!rrpge_m_halt_isset(hnd, RRPGE_HLT_AUDIO)){ return 0; } /* No audio event present */

 r = hnd->aud.evct;
 hnd->aud.evct = 0U;
 rrpge_m_halt_clr(hnd, RRPGE_HLT_AUDIO);

 /* Fill in the 512 sample target buffers */

 i  = ((hnd->aud.bufp) & 0x200U) ^ 0x200U; /* Select not currently filled half */
 pl = &(hnd->aud.bufl[i]);
 pr = &(hnd->aud.bufr[i]);

 for (i = 0; i < 512U; i++){
  lbuf[i] = pl[i];
  rbuf[i] = pr[i];
 }

 /* OK, all done */

 return r;
}



/* Sets main clock frequency - implementation of RRPGE library function */
void rrpge_set_clock(rrpge_object_t* hnd, rrpge_iuint clk)
{
 if (clk < 1000000U){ clk = 1000000U; } /* Don't allow below 1 MHz */
 hnd->aud.mclk = clk;
}
