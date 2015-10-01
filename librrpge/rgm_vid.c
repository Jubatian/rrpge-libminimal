/**
**  \file
**  \brief     Graphics FIFO and display manager.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.10
*/



#include "rgm_vid.h"
#include "rgm_vidl.h"
#include "rgm_halt.h"
#include "rgm_stat.h"
#include "rgm_fifo.h"



/* Peripheral memory size in 32bit units.
** Must be a power of 2 and a multiple of 64K. */
#define  PRAMS  RRPGE_M_PRAMS



/* State: GDG registers (0x0010 - 0x001F), reader */
RRPGE_M_FASTCALL static auint rrpge_m_vid_stat_read_regs(rrpge_object_t* hnd, auint adr, auint rmw)
{
 switch (adr){
  case 0x00U:
  case 0x01U: return (hnd->vid.ckey[adr     ]) & 0xFFFFU; break;
  case 0x02U: return (hnd->vid.dscn)           & 0x77FFU; break;
  case 0x03U: return (hnd->vid.dlcl)           & 0xFFFFU; break;
  case 0x04U:
  case 0x05U: return (hnd->vid.smrr[adr & 1U]) & 0xFFFFU; break;
  case 0x06U: return (hnd->vid.dldf)           & 0xFFFFU; break;
  case 0x07U: return (hnd->vid.stat)           & 0x8000U; break;
  case 0x08U:
  case 0x09U:
  case 0x0AU:
  case 0x0BU:
  case 0x0CU:
  case 0x0DU:
  case 0x0EU:
  case 0x0FU: return (hnd->vid.sdef[adr & 7U]) & 0xFFFFU; break;
  default:    return 0U;                                  break;
 }
}



/* State: GDG registers (0x0010 - 0x001F), writer */
RRPGE_M_FASTCALL static void rrpge_m_vid_stat_write_regs(rrpge_object_t* hnd, auint adr, auint val)
{
 switch (adr){
  case 0x00U:
  case 0x01U: hnd->vid.ckey[adr     ] = val & 0xFFFFU; break;
  case 0x02U: hnd->vid.dscn           = val & 0x77FFU; break;
  case 0x03U: hnd->vid.dlcl           = val & 0xFFFFU; break;
  case 0x04U:
  case 0x05U: hnd->vid.smrr[adr & 1U] = val & 0xFFFFU; break;
  case 0x06U: hnd->vid.dldf           = val & 0xFFFFU;
              rrpge_m_fifo_gsus_set(hnd);                     /* Suspend Graphics FIFO */
              hnd->vid.stat          |= 0x8000U;       break; /* Frame incomplete flag */
  case 0x07U:                                          break;
  case 0x08U:
  case 0x09U:
  case 0x0AU:
  case 0x0BU:
  case 0x0CU:
  case 0x0DU:
  case 0x0EU:
  case 0x0FU: hnd->vid.sdef[adr & 7U] = val & 0xFFFFU; break;
  default:                                             break;
 }
}



/* State: GDG display list definition register getter */
RRPGE_M_FASTCALL static auint rrpge_m_vid_stat_get_dreg(rrpge_object_t* hnd, auint adr)
{
 return (hnd->vid.dldf & 0xFFFFU);
}



/* State: GDG display list definition register setter */
RRPGE_M_FASTCALL static void rrpge_m_vid_stat_set_dreg(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->vid.dldf = val & 0xFFFFU;
}



/* State: GDG status register getter */
RRPGE_M_FASTCALL static auint rrpge_m_vid_stat_get_sreg(rrpge_object_t* hnd, auint adr)
{
 return (hnd->vid.stat & 0x8000U);
}



/* State: GDG status register setter */
RRPGE_M_FASTCALL static void rrpge_m_vid_stat_set_sreg(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->vid.stat = val & 0x8000U;
}



/* State: Display list definition latch reader */
RRPGE_M_FASTCALL static auint rrpge_m_vid_stat_read_dlat(rrpge_object_t* hnd, auint adr, auint rmw)
{
 return (hnd->vid.dlat & 0xFFFFU);
}



/* State: Display list definition latch writer */
RRPGE_M_FASTCALL static void rrpge_m_vid_stat_write_dlat(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->vid.dlat = val & 0xFFFFU;
}



/* State: Video line count reader */
RRPGE_M_FASTCALL static auint rrpge_m_vid_stat_read_vln(rrpge_object_t* hnd, auint adr, auint rmw)
{
 return (hnd->vid.vln & 0x03FFU);
}



/* State: Video line count writer */
RRPGE_M_FASTCALL static void rrpge_m_vid_stat_write_vln(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->vid.vln = val & 0x03FFU;
}



/* State: Video remaining cycles / line reader */
RRPGE_M_FASTCALL static auint rrpge_m_vid_stat_read_vlc(rrpge_object_t* hnd, auint adr, auint rmw)
{
 return (hnd->vid.vlc & 0x01FFU);
}



/* State: Video remaining cycles / line writer */
RRPGE_M_FASTCALL static void rrpge_m_vid_stat_write_vlc(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->vid.vlc = val & 0x01FFU;
}



/* Initializes Video (GDG) emulation adding the appropriate handlers to the
** state manager. */
void  rrpge_m_vid_init(void)
{
 rrpge_m_stat_add_rw_handler(&rrpge_m_vid_stat_read_regs, &rrpge_m_vid_stat_write_regs,
                             RRPGE_STA_UPA_G, 16U);
 rrpge_m_stat_add_ac_handler(&rrpge_m_vid_stat_get_dreg,  &rrpge_m_vid_stat_set_dreg,
                             RRPGE_STA_UPA_G + 0x6U, 1U);
 rrpge_m_stat_add_ac_handler(&rrpge_m_vid_stat_get_sreg,  &rrpge_m_vid_stat_set_sreg,
                             RRPGE_STA_UPA_G + 0x7U, 1U);
 rrpge_m_stat_add_rw_handler(&rrpge_m_vid_stat_read_dlat, &rrpge_m_vid_stat_write_dlat,
                             RRPGE_STA_VARS + 0x15U, 1U);
 rrpge_m_stat_add_rw_handler(&rrpge_m_vid_stat_read_vln,  &rrpge_m_vid_stat_write_vln,
                             RRPGE_STA_VARS + 0x10U, 1U);
 rrpge_m_stat_add_rw_handler(&rrpge_m_vid_stat_read_vlc,  &rrpge_m_vid_stat_write_vlc,
                             RRPGE_STA_VARS + 0x11U, 1U);
}



/* Initializes Video (GDG) emulation internal resources */
void  rrpge_m_vid_initres(rrpge_object_t* hnd)
{
 /* Invalidate clipping buffers */

 hnd->vid.cini[0] = 0x10000U;
 hnd->vid.cini[1] = 0x10000U;
 hnd->vid.cini[2] = 0x10000U;

 /* Rendering enabled */

 hnd->vid.rena = 0x3U;
}



/* Based on the cycles needing emulation, process the video, and the Graphics
** Display Generator's Display List clear function. Also calls back the line
** renderer as needed. */
void  rrpge_m_vid_proc(rrpge_object_t* hnd, auint cy)
{
 auint a;
 auint o;
 auint i;
 auint j;
 auint c;
 auint s;
 auint t;

 /* Consume the provided number of cycles */

 hnd->vid.vlc -= cy;

 /* Until the remaining cycle count is negative or zero, process lines. */

 while ( ((hnd->vid.vlc & 0x80000000U) != 0U) ||
         (hnd->vid.vlc == 0U) ){

  /* A line worth of cycles will be consumed */

  hnd->vid.vlc += 400U;

  /* Render the current line */

  if (((hnd->vid.rena) & 0x2U) != 0U){ /* Rendering enabled */
   rrpge_m_vidl(hnd);
  }

  /* Increment counters */

  hnd->vid.vln ++;
  hnd->vid.vln &= 0xFFFFU;
  if ((hnd->vid.vln >= 400U) && ((hnd->vid.vln & 0x8000U) == 0U)){
   hnd->vid.vln = 0x10000U + 400U - RRPGE_M_VLN;
   hnd->vid.rena = (hnd->vid.rena & (~0x2U)) | /* Transfer requested render state */
                   ((hnd->vid.rena & (0x1U)) << 1);
   rrpge_m_halt_set(hnd, RRPGE_HLT_FRAME);
  }

  /* Before starting next frame (line counter is zero, so next iteration will
  ** result in rendering a line), if the status flag request so, process the
  ** display list clear, and update the latch, so completing the double
  ** buffering assistance. */

  if ( (hnd->vid.vln == 0U) &&
       ((hnd->vid.stat & 0x8000U) != 0U) ){

   /* Prepare for clear */

   i  = hnd->vid.dlat;                      /* Display list def. latch */
   a  = ((i & 0xF000U) << 4) & (PRAMS - 1U);   /* PRAM bank */
   o  = ((i & 0x0FFCU) << 4);               /* Start offset */
   j  = 1600U << (i & 3U);                  /* Display list size limit */
   i  = hnd->vid.dlcl;                      /* Display list clear controls */
   s  = (i >> 11) & 0x1FU;                  /* Initial skip */
   j -= s;
   o += s;
   s  = (i >> 6) & 0x1FU;                   /* Skip amount / streak */
   c  = (i     ) & 0x3FU;                   /* Clear amount / streak */

   /* Clear */

   if (c != 0U){
    do{
     for (t = 0U; t < c; t++){ /* Clear */
      hnd->st.pram[a + o] = 0U;
      o  = (o + 1U) & 0xFFFFU;
      j --;
      if (j == 0U){ break; }
     }
     if (j <= s){              /* Skip */
      j  = 0U;
     }else{
      j -= s;
      o  = (o +  s) & 0xFFFFU;
     }
    }while (j != 0U);
   }

   /* Update latch, and clear flags */

   hnd->vid.dlat = hnd->vid.dldf;
   hnd->vid.stat = 0U;         /* Clear flag */
   rrpge_m_fifo_gsus_clr(hnd); /* Cease suspending Graphics FIFO */

  }

 }

}



/* Requests current status from the Status register, that is, whether a frame
** is waiting for completion (the Graphics FIFO uses it to suspend) */
auint rrpge_m_vid_getstat(rrpge_object_t* hnd)
{
 return ((hnd->vid.stat & 0x8000U) != 0U);
}



/* Requests remaining cycles from the current Video line. Used for generating
** bursts of emulation passes (line-oriented emulation) */
auint rrpge_m_vid_getremcy(rrpge_object_t* hnd)
{
 return hnd->vid.vlc;
}



/* Toggle graphics rendering - implementation of RRPGE library function */
void rrpge_enarender(rrpge_object_t* hnd, rrpge_ibool tg)
{
 if (tg){ hnd->vid.rena = (hnd->vid.rena) |   1U;  }
 else   { hnd->vid.rena = (hnd->vid.rena) & (~1U); }
}
