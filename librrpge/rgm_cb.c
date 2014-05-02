/**
**  \file
**  \brief     Callback related components, default callbacks.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.02
*/


#include "rgm_cb.h"
#include "rrpge.h"



/* Default callbacks. These all produce the minimal result, most commonly
** reporting unsupported feature. In the case of kernel tasks this inlcudes an
** immediate termination using rrpge_taskend() with the appropriate result. */

/* Line renderer */
static void rrpge_m_cb_line(rrpge_object_t* hnd, rrpge_uint32 ln, rrpge_uint32 const* buf)
{
}

/* Task: Load binary data */
static void rrpge_m_cb_loadbin(rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);
 rrpge_m_info.hlt |= RRPGE_HLT_FAULT; /* This callback should really really be provided... */
}

/* Task: Load nonvolatile save */
static void rrpge_m_cb_loadnv(rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);  /* No pages loaded */
}

/* Task: Save nonvolatile save */
static void rrpge_m_cb_savenv(rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8001U);  /* Failed save */
}

/* Task: List nonvolatile saves */
static void rrpge_m_cb_listnv(rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);  /* No saves found */
}

/* Function: Load arbitrary file */
static rrpge_uint32 rrpge_m_cb_loadfile(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* Not supported */
}

/* Function: Save arbitrary file */
static rrpge_uint32 rrpge_m_cb_savefile(rrpge_object_t* hnd, const void* par)
{
 return 1;                            /* Not supported */
}

/* Subroutine: Set palette entry */
static void rrpge_m_cb_setcolor(rrpge_object_t* hnd, const void* par)
{
}

/* Function: Digital joystick: Read device availability */
static rrpge_uint32 rrpge_m_cb_idig_ava(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* None available */
}

/* Function: Digital joystick: Read controls */
static rrpge_uint32 rrpge_m_cb_idig_ctr(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* Nothing pressed */
}

/* Function: Analog joystick: Read device availability */
static rrpge_uint32 rrpge_m_cb_iana_ava(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* None available */
}

/* Function: Analog joystick: Read controls */
static rrpge_uint32 rrpge_m_cb_iana_ctr(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* Nothing pressed */
}

/* Function: Analog joystick: Read position */
static rrpge_uint32 rrpge_m_cb_iana_pos(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* Centered */
}

/* Function: Mice: Read device availability */
static rrpge_uint32 rrpge_m_cb_imou_ava(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* None available */
}

/* Function: Mice: Read controls */
static rrpge_uint32 rrpge_m_cb_imou_ctr(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* Nothing pressed */
}

/* Function: Mice: Read position */
static rrpge_uint32 rrpge_m_cb_imou_pos(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* In the upper left corner */
}

/* Function: Mice: Read cursor requirement */
static rrpge_uint32 rrpge_m_cb_imou_cur(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* Not required (nonexistent mouse) */
}

/* Function: Text: Read device availability */
static rrpge_uint32 rrpge_m_cb_itxt_ava(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* None available */
}

/* Function: Text: Read FIFO */
static rrpge_uint32 rrpge_m_cb_itxt_rfi(rrpge_object_t* hnd, const void* par)
{
 return 0;                            /* No input */
}

/* Task: Read UTF-8 representation of user. */
static void rrpge_m_cb_getuutf(rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);  /* Does nothing (empty strings) and returns. */
}

/* Task: Send out network packet. */
static void rrpge_m_cb_sendto(rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);  /* Packet went up in smoke, fine with that. */
}

/* Task: List accessible users. */
static void rrpge_m_cb_listuser(rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);  /* No users found. */
}



/* Check ID for validity. Returns nonzero if valid, 0 otherwise. */
static auint rrpge_m_cbid_isvalid(auint id)
{
 if (id == RRPGE_CB_LOADBIN) { return 1; }
 if (id == RRPGE_CB_LOADNV)  { return 1; }
 if (id == RRPGE_CB_SAVENV)  { return 1; }
 if (id == RRPGE_CB_LISTNV)  { return 1; }
 if (id == RRPGE_CB_GETUUTF) { return 1; }
 if (id == RRPGE_CB_SENDTO)  { return 1; }
 if (id == RRPGE_CB_LISTUSER){ return 1; }
 if (id == RRPGE_CB_SETCOLOR){ return 1; }
 if (id == RRPGE_CB_LOADFILE){ return 1; }
 if (id == RRPGE_CB_SAVEFILE){ return 1; }
 if (id == RRPGE_CB_IDIG_AVA){ return 1; }
 if (id == RRPGE_CB_IDIG_CTR){ return 1; }
 if (id == RRPGE_CB_IANA_AVA){ return 1; }
 if (id == RRPGE_CB_IANA_CTR){ return 1; }
 if (id == RRPGE_CB_IANA_POS){ return 1; }
 if (id == RRPGE_CB_IMOU_AVA){ return 1; }
 if (id == RRPGE_CB_IMOU_CTR){ return 1; }
 if (id == RRPGE_CB_IMOU_POS){ return 1; }
 if (id == RRPGE_CB_IMOU_CUR){ return 1; }
 if (id == RRPGE_CB_ITXT_AVA){ return 1; }
 if (id == RRPGE_CB_ITXT_RFI){ return 1; }
 return 0;
}



/* Processes an rrpge_cbpack_t structure into the given emulator object for
** servicing callbacks. Entries not supplied in the structure will get the
** default empty callbacks. */
void rrpge_m_cb_process(rrpge_object_t* obj, rrpge_cbpack_t const* cbp)
{
 auint i;

 /* First fill in the defaults */
 obj->cb_lin = &rrpge_m_cb_line;
 obj->cb_tsk[RRPGE_CB_LOADBIN]  = &rrpge_m_cb_loadbin;
 obj->cb_tsk[RRPGE_CB_LOADNV]   = &rrpge_m_cb_loadnv;
 obj->cb_tsk[RRPGE_CB_SAVENV]   = &rrpge_m_cb_savenv;
 obj->cb_tsk[RRPGE_CB_LISTNV]   = &rrpge_m_cb_listnv;
 obj->cb_tsk[RRPGE_CB_GETUUTF]  = &rrpge_m_cb_getuutf;
 obj->cb_tsk[RRPGE_CB_SENDTO]   = &rrpge_m_cb_sendto;
 obj->cb_tsk[RRPGE_CB_LISTUSER] = &rrpge_m_cb_listuser;
 obj->cb_sub[RRPGE_CB_SETCOLOR] = &rrpge_m_cb_setcolor;
 obj->cb_fun[RRPGE_CB_LOADFILE] = &rrpge_m_cb_loadfile;
 obj->cb_fun[RRPGE_CB_SAVEFILE] = &rrpge_m_cb_savefile;
 obj->cb_fun[RRPGE_CB_IDIG_AVA] = &rrpge_m_cb_idig_ava;
 obj->cb_fun[RRPGE_CB_IDIG_CTR] = &rrpge_m_cb_idig_ctr;
 obj->cb_fun[RRPGE_CB_IANA_AVA] = &rrpge_m_cb_iana_ava;
 obj->cb_fun[RRPGE_CB_IANA_CTR] = &rrpge_m_cb_iana_ctr;
 obj->cb_fun[RRPGE_CB_IANA_POS] = &rrpge_m_cb_iana_pos;
 obj->cb_fun[RRPGE_CB_IMOU_AVA] = &rrpge_m_cb_imou_ava;
 obj->cb_fun[RRPGE_CB_IMOU_CTR] = &rrpge_m_cb_imou_ctr;
 obj->cb_fun[RRPGE_CB_IMOU_POS] = &rrpge_m_cb_imou_pos;
 obj->cb_fun[RRPGE_CB_IMOU_CUR] = &rrpge_m_cb_imou_cur;
 obj->cb_fun[RRPGE_CB_ITXT_AVA] = &rrpge_m_cb_itxt_ava;
 obj->cb_fun[RRPGE_CB_ITXT_RFI] = &rrpge_m_cb_itxt_rfi;

 /* Use the input to fill in any defined function (if any) */
 if (cbp != RRPGE_M_NULL){

  /* Line callback: Only if not null */
  if (cbp->cb_line != RRPGE_M_NULL){ obj->cb_lin = cbp->cb_line; }

  /* Tasks */
  for (i=0; i<(cbp->tsk_n); i++){
   if (rrpge_m_cbid_isvalid(cbp->tsk_d[i].id)){
    obj->cb_tsk[cbp->tsk_d[i].id] = cbp->tsk_d[i].cb;
   }
  }

  /* Subroutines */
  for (i=0; i<(cbp->sub_n); i++){
   if (rrpge_m_cbid_isvalid(cbp->sub_d[i].id)){
    obj->cb_sub[cbp->sub_d[i].id] = cbp->sub_d[i].cb;
   }
  }

  /* Functions */
  for (i=0; i<(cbp->fun_n); i++){
   if (rrpge_m_cbid_isvalid(cbp->fun_d[i].id)){
    obj->cb_fun[cbp->fun_d[i].id] = cbp->fun_d[i].cb;
   }
  }

 }

}
