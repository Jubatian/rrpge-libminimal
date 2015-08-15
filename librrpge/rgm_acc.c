/**
**  \file
**  \brief     CPU emulation main
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.15
*/


#include "rgm_acc.h"
#include "rgm_acco.h"
#include "rgm_stat.h"



/* State: Accelerator registers (0x0A0 - 0x0BF), reader */
RRPGE_M_FASTCALL static auint rrpge_m_acc_stat_regs_read(rrpge_object_t* hnd, auint adr, auint rmw)
{
 switch (adr){
  case 0x00U: return (hnd->acc.rwm >> 16) & 0xFFFFU; break;
  case 0x01U: return (hnd->acc.rwm      ) & 0xFFFFU; break;
  case 0x02U: return (hnd->acc.dbp      ) & 0xFFFFU; break;
  case 0x03U: return (hnd->acc.dps      ) & 0xFFFFU; break;
  case 0x04U: return (hnd->acc.dad >> 16) & 0xFFFFU; break;
  case 0x05U: return (hnd->acc.dad      ) & 0xFFFFU; break;
  case 0x06U: return (hnd->acc.cad >> 16) & 0xFFFFU; break;
  case 0x07U: return (hnd->acc.cad      ) & 0xFFFFU; break;
  case 0x08U: return (hnd->acc.yad >> 16) & 0xFFFFU; break;
  case 0x09U: return (hnd->acc.yad      ) & 0xFFFFU; break;
  case 0x0AU: return (hnd->acc.xad >> 16) & 0xFFFFU; break;
  case 0x0BU: return (hnd->acc.xad      ) & 0xFFFFU; break;
  case 0x0CU: return (hnd->acc.yin >> 16) & 0xFFFFU; break;
  case 0x0DU: return (hnd->acc.yin      ) & 0xFFFFU; break;
  case 0x0EU: return (hnd->acc.xin >> 16) & 0xFFFFU; break;
  case 0x0FU: return (hnd->acc.xin      ) & 0xFFFFU; break;
  case 0x10U: return (hnd->acc.yof >> 16) & 0xFFFFU; break;
  case 0x11U: return (hnd->acc.yof      ) & 0xFFFFU; break;
  case 0x12U: return (hnd->acc.sbn      ) & 0xFFFFU; break;
  case 0x13U: return (hnd->acc.spt      ) & 0xFFFFU; break;
  case 0x14U: return (hnd->acc.sps      ) & 0xFFFFU; break;
  case 0x15U: return (hnd->acc.bfl      ) & 0xFFFFU; break;
  case 0x16U: return (hnd->acc.sms      ) & 0xFFFFU; break;
  case 0x17U: return (hnd->acc.rct      ) & 0xFFFFU; break;
  case 0x18U: return (hnd->acc.cct >> 16) & 0xFFFFU; break;
  case 0x19U: return (hnd->acc.cct      ) & 0xFFFFU; break;
  case 0x1AU: return (hnd->acc.xof >> 16) & 0xFFFFU; break;
  case 0x1BU: return (hnd->acc.xof      ) & 0xFFFFU; break;
  case 0x1CU: return (hnd->acc.dof >> 16) & 0xFFFFU; break;
  case 0x1DU: return (hnd->acc.dof      ) & 0xFFFFU; break;
  case 0x1EU: return (hnd->acc.rpm      ) & 0xFFFFU; break;
  case 0x1FU: return (hnd->acc.pat      ) & 0xFFFFU; break;
  default:    return 0U;                             break;
 }
}


/* State: Accelerator registers (0x0A0 - 0x0BF), writer */
RRPGE_M_FASTCALL static void rrpge_m_acc_stat_regs_write(rrpge_object_t* hnd, auint adr, auint val)
{
 switch (adr){
  case 0x00U: hnd->acc.rwm = (hnd->acc.rwm & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x01U: hnd->acc.rwm = (hnd->acc.rwm & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x02U: hnd->acc.dbp = val & 0xFFFFU;                                      break;
  case 0x03U: hnd->acc.dps = val & 0xFFFFU;                                      break;
  case 0x04U: hnd->acc.dad = (hnd->acc.dad & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x05U: hnd->acc.dad = (hnd->acc.dad & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x06U: hnd->acc.cad = (hnd->acc.cad & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x07U: hnd->acc.cad = (hnd->acc.cad & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x08U: hnd->acc.yad = (hnd->acc.yad & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x09U: hnd->acc.yad = (hnd->acc.yad & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x0AU: hnd->acc.xad = (hnd->acc.xad & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x0BU: hnd->acc.xad = (hnd->acc.xad & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x0CU: hnd->acc.yin = (hnd->acc.yin & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x0DU: hnd->acc.yin = (hnd->acc.yin & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x0EU: hnd->acc.xin = (hnd->acc.xin & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x0FU: hnd->acc.xin = (hnd->acc.xin & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x10U: hnd->acc.yof = (hnd->acc.yof & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x11U: hnd->acc.yof = (hnd->acc.yof & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x12U: hnd->acc.sbn = val & 0xFFFFU;                                      break;
  case 0x13U: hnd->acc.spt = val & 0xFFFFU;                                      break;
  case 0x14U: hnd->acc.sps = val & 0xFFFFU;                                      break;
  case 0x15U: hnd->acc.bfl = val & 0xFFFFU;                                      break;
  case 0x16U: hnd->acc.sms = val & 0xFFFFU;                                      break;
  case 0x17U: hnd->acc.rct = val & 0xFFFFU;                                      break;
  case 0x18U: hnd->acc.cct = (hnd->acc.cct & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x19U: hnd->acc.cct = (hnd->acc.cct & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x1AU: hnd->acc.xof = (hnd->acc.xof & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x1BU: hnd->acc.xof = (hnd->acc.xof & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x1CU: hnd->acc.dof = (hnd->acc.dof & 0xFFFFU) | ((val & 0xFFFFU) << 16); break;
  case 0x1DU: hnd->acc.dof = (hnd->acc.dof & 0xFFFF0000U) | (val & 0xFFFFU);     break;
  case 0x1EU: hnd->acc.rpm = val & 0xFFFFU;                                      break;
  case 0x1FU: hnd->acc.pat = val & 0xFFFFU;                                      break;
  default:                                                                       break;
 }
}


/* State: Reindex table (0x300 - 0x3FF), reader */
RRPGE_M_FASTCALL static auint rrpge_m_acc_stat_rtab_read(rrpge_object_t* hnd, auint adr, auint rmw)
{
 adr &= 0xFFU;
 return (((auint)(hnd->acc.grb[(adr << 1)     ])) << 8) |
        (((auint)(hnd->acc.grb[(adr << 1) + 1U]))     );
}


/* State: Reindex table (0x300 - 0x3FF), writer */
RRPGE_M_FASTCALL static void rrpge_m_acc_stat_rtab_write(rrpge_object_t* hnd, auint adr, auint val)
{
 adr &= 0xFFU;
 hnd->acc.grb[(adr << 1)     ] = (val >> 8) & 0xFFU;
 hnd->acc.grb[(adr << 1) + 1U] = (val     ) & 0xFFU;
}



/* Initializes accelerator emulation adding the appropriate handlers to the
** state manager. */
void rrpge_m_acc_init(void)
{
 rrpge_m_stat_add_rw_handler(&rrpge_m_acc_stat_regs_read, &rrpge_m_acc_stat_regs_write,
                             RRPGE_STA_ACC, 32U);
 rrpge_m_stat_add_rw_handler(&rrpge_m_acc_stat_rtab_read, &rrpge_m_acc_stat_rtab_write,
                             RRPGE_STA_REIND, 256U);
}



/* Execute accelerator operation, whatever is in the current set of
** accelerator registers. Returns number of cycles the operation takes. */
auint rrpge_m_acc_op(rrpge_object_t* hnd)
{
 return rrpge_m_acco(hnd);
}
