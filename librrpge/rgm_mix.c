/**
**  \file
**  \brief     Mixer emulation main
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.15
*/


#include "rgm_mix.h"
#include "rgm_mixo.h"
#include "rgm_stat.h"



/* State: Mixer registers (0x090 - 0x09F), reader */
RRPGE_M_FASTCALL static auint rrpge_m_mix_stat_regs_read(rrpge_object_t* hnd, auint adr, auint rmw)
{
 switch (adr){
  case 0x05U: return (hnd->mix.dbk) & 0x000FU; break;
  case 0x06U: return (hnd->mix.dof) & 0x8FFFU; break;
  case 0x07U: return (hnd->mix.dct) & 0xF01FU; break;
  case 0x08U: return (hnd->mix.scf) & 0xFFFFU; break;
  case 0x09U: return (hnd->mix.spr) & 0xFFFFU; break;
  case 0x0AU: return (hnd->mix.sfa) & 0xFFFFU; break;
  case 0x0BU: return (hnd->mix.sfr) & 0xFFFFU; break;
  case 0x0CU: return (hnd->mix.ama) & 0xFFFFU; break;
  case 0x0DU: return (hnd->mix.ami) & 0xFFFFU; break;
  case 0x0EU: return (hnd->mix.soh) & 0x01FFU; break;
  case 0x0FU: return (hnd->mix.sol) & 0xFFFFU; break;
  default:    return 0U;                       break;
 }
}


/* State: Mixer registers (0x090 - 0x09F), writer */
RRPGE_M_FASTCALL static void rrpge_m_mix_stat_regs_write(rrpge_object_t* hnd, auint adr, auint val)
{
 switch (adr){
  case 0x05U: hnd->mix.dbk = val & 0x000FU; break;
  case 0x06U: hnd->mix.dof = val & 0x8FFFU; break;
  case 0x07U: hnd->mix.dct = val & 0xF01FU; break;
  case 0x08U: hnd->mix.scf = val & 0xFFFFU; break;
  case 0x09U: hnd->mix.spr = val & 0xFFFFU; break;
  case 0x0AU: hnd->mix.sfa = val & 0xFFFFU; break;
  case 0x0BU: hnd->mix.sfr = val & 0xFFFFU; break;
  case 0x0CU: hnd->mix.ama = val & 0xFFFFU; break;
  case 0x0DU: hnd->mix.ami = val & 0xFFFFU; break;
  case 0x0EU: hnd->mix.soh = val & 0x01FFU; break;
  case 0x0FU: hnd->mix.sol = val & 0xFFFFU; break;
  default:                                  break;
 }
}



/* Initializes mixer emulation adding the appropriate handlers to the state
** manager. */
void rrpge_m_mix_init(void)
{
 rrpge_m_stat_add_rw_handler(&rrpge_m_mix_stat_regs_read, &rrpge_m_mix_stat_regs_write,
                             RRPGE_STA_MIXER, 16U);
 rrpge_m_mixo_init();
}



/* Execute mixer operation, whatever is in the current set of mixer
** registers. Returns number of cycles the operation takes. */
auint rrpge_m_mix_op(rrpge_object_t* hnd)
{
 return rrpge_m_mixo(hnd);
}
