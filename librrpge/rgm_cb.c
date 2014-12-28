/**
**  \file
**  \brief     Callback related components, default callbacks.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.28
*/


#include "rgm_cb.h"
#include "rrpge.h"



/* Default callbacks. These all produce the minimal result, most commonly
** reporting unsupported feature. In the case of kernel tasks this inlcudes an
** immediate termination using rrpge_taskend() with the appropriate result. */

/* Line renderer */
static void rrpge_m_cb_line(rrpge_object_t* hnd, rrpge_iuint ln, rrpge_uint32 const* buf)
{
}

/* Task: Load binary data */
static void rrpge_m_cb_loadbin(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);
 rrpge_m_info.hlt |= RRPGE_HLT_FAULT; /* This callback is mandatory */
}

/* Task: Load page from file */
static void rrpge_m_cb_load(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, RRPGE_SFI_UNSUPP); /* Load failed */
}

/* Task: Save page into file */
static void rrpge_m_cb_save(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, RRPGE_SFI_UNSUPP); /* Save failed */
}

/* Task: Find next file */
static void rrpge_m_cb_next(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par)
{
 rrpge_cbp_next_t const* p = (rrpge_cbp_next_t const*)(par);
 if ((p->ncw) != 0U){ p->nam[0] = 0U; }     /* No files can be found */
 rrpge_taskend(hnd, tsh, 0x8000U);          /* Normal termination */
}

/* Task: Move a file */
static void rrpge_m_cb_move(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, RRPGE_SFI_UNSUPP); /* Move failed */
}

/* Subroutine: Set palette entry */
static void rrpge_m_cb_setpal(rrpge_object_t* hnd, const void* par)
{
 /* This callback is normally mandatory, but the host may decide to not show
 ** graphics at all (audio playback only for music applications), so no fault
 ** if not implemented. */
}

/* Subroutine: Change video mode */
static void rrpge_m_cb_mode(rrpge_object_t* hnd, const void* par)
{
 /* This callback is normally mandatory, but the host may decide to not show
 ** graphics at all (audio playback only for music applications), so no fault
 ** if not implemented. */
}

/* Subroutine: Set stereoscopic 3D */
static void rrpge_m_cb_setst3d(rrpge_object_t* hnd, const void* par)
{
}

/* Function: Get device properties */
static rrpge_iuint rrpge_m_cb_getprops(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Subroutine: Drop device */
static void rrpge_m_cb_dropdev(rrpge_object_t* hnd, const void* par)
{
}

/* Function: Get digital input descriptor */
static rrpge_iuint rrpge_m_cb_getdidesc(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Function: Get analog input descriptor */
static rrpge_iuint rrpge_m_cb_getaidesc(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Function: Get device name */
static rrpge_iuint rrpge_m_cb_getname(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Function: Get digital inputs */
static rrpge_iuint rrpge_m_cb_getdi(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Function: Get analog inputs */
static rrpge_iuint rrpge_m_cb_getai(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Function: Pop text FIFO */
static rrpge_iuint rrpge_m_cb_popchar(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Subroutine: Get local users */
static void rrpge_m_cb_getlocal(rrpge_object_t* hnd, const void* par)
{
}

/* Task: Read UTF-8 representation of User ID. */
static void rrpge_m_cb_getutf(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);    /* Does nothing (empty strings) and returns. */
}

/* Function: Get user preferred language */
static rrpge_iuint rrpge_m_cb_getlang(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Function: Get user preferred colors */
static rrpge_iuint rrpge_m_cb_getcolors(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Function: Get user stereoscopic 3D preference */
static rrpge_iuint rrpge_m_cb_getst3d(rrpge_object_t* hnd, const void* par)
{
 return 0;
}

/* Task: Send out network packet. */
static void rrpge_m_cb_send(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);    /* Packet went up in smoke, fine with that. */
}

/* Task: List accessible users. */
static void rrpge_m_cb_listusers(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par)
{
 rrpge_taskend(hnd, tsh, 0x8000U);    /* No users found. */
}

/* Function: Return area activity. This is public (RRPGE library interface) */
rrpge_iuint rrpge_cb_checkarea(rrpge_object_t* hnd, const void* par)
{
 rrpge_cbp_checkarea_t const* p = par;
 rrpge_cbp_getdi_t sdi;
 rrpge_cbp_getai_t sai;
 auint x;
 auint y;
 auint b;
 auint t;

 t = (hnd->st.stat[RRPGE_STA_VARS + 0x30U + ((p->dev) & 0xFU)]) & 0xF800U;

 if ( (t != ((RRPGE_INPT_MOUSE << 12) | 0x0800U)) &&
      (t != ((RRPGE_INPT_TOUCH << 12) | 0x0800U)) ){ return 0; }

 sdi.dev = p->dev;
 sdi.ing = 0U;
 b = hnd->cb_fun[RRPGE_CB_GETDI](hnd, &sdi);

 if (t == ((RRPGE_INPT_TOUCH << 12) | 0x0800U)){
  if ((b & 0x1010U) == 0U){ return 0; } /* Neither touch, neither hover */
 }else{
  b |= b >> 1;                          /* Mouse: Also active for right button press */
 }
 b = (b >> 4) & 1U;                     /* Button / Press activity */

 sai.dev = p->dev;
 sai.inp = 0U;
 x = hnd->cb_fun[RRPGE_CB_GETAI](hnd, &sai);
 sai.inp = 1U;
 y = hnd->cb_fun[RRPGE_CB_GETAI](hnd, &sai);

 if ( (x >= (p->x)) && (x < ((p->x) + (p->w))) &&
      (y >= (p->y)) && (y < ((p->y) + (p->h))) ){ return (2U + b); }

 return 0;
}



/* Check ID for validity. Returns nonzero if valid, 0 otherwise. */
static auint rrpge_m_cbid_isvalid(auint id)
{
 if ( (id == RRPGE_CB_LOADBIN)   ||
      (id == RRPGE_CB_LOAD)      ||
      (id == RRPGE_CB_SAVE)      ||
      (id == RRPGE_CB_NEXT)      ||
      (id == RRPGE_CB_MOVE)      ||
      (id == RRPGE_CB_GETUTF)    ||
      (id == RRPGE_CB_SEND)      ||
      (id == RRPGE_CB_LISTUSERS) ||
      (id == RRPGE_CB_SETPAL)    ||
      (id == RRPGE_CB_MODE)      ||
      (id == RRPGE_CB_SETST3D)   ||
      (id == RRPGE_CB_DROPDEV)   ||
      (id == RRPGE_CB_CHECKAREA) ||
      (id == RRPGE_CB_GETLOCAL)  ||
      (id == RRPGE_CB_GETPROPS)  ||
      (id == RRPGE_CB_GETDIDESC) ||
      (id == RRPGE_CB_GETAIDESC) ||
      (id == RRPGE_CB_GETNAME)   ||
      (id == RRPGE_CB_GETDI)     ||
      (id == RRPGE_CB_GETAI)     ||
      (id == RRPGE_CB_POPCHAR)   ||
      (id == RRPGE_CB_GETLANG)   ||
      (id == RRPGE_CB_GETCOLORS) ||
      (id == RRPGE_CB_GETST3D)   ){ return 1; }
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
 obj->cb_tsk[RRPGE_CB_LOADBIN]   = &rrpge_m_cb_loadbin;
 obj->cb_tsk[RRPGE_CB_LOAD]      = &rrpge_m_cb_load;
 obj->cb_tsk[RRPGE_CB_SAVE]      = &rrpge_m_cb_save;
 obj->cb_tsk[RRPGE_CB_NEXT]      = &rrpge_m_cb_next;
 obj->cb_tsk[RRPGE_CB_MOVE]      = &rrpge_m_cb_move;
 obj->cb_tsk[RRPGE_CB_GETUTF]    = &rrpge_m_cb_getutf;
 obj->cb_tsk[RRPGE_CB_SEND]      = &rrpge_m_cb_send;
 obj->cb_tsk[RRPGE_CB_LISTUSERS] = &rrpge_m_cb_listusers;
 obj->cb_sub[RRPGE_CB_SETPAL]    = &rrpge_m_cb_setpal;
 obj->cb_sub[RRPGE_CB_MODE]      = &rrpge_m_cb_mode;
 obj->cb_sub[RRPGE_CB_SETST3D]   = &rrpge_m_cb_setst3d;
 obj->cb_sub[RRPGE_CB_DROPDEV]   = &rrpge_m_cb_dropdev;
 obj->cb_sub[RRPGE_CB_GETLOCAL]  = &rrpge_m_cb_getlocal;
 obj->cb_fun[RRPGE_CB_GETPROPS]  = &rrpge_m_cb_getprops;
 obj->cb_fun[RRPGE_CB_GETDIDESC] = &rrpge_m_cb_getdidesc;
 obj->cb_fun[RRPGE_CB_GETAIDESC] = &rrpge_m_cb_getaidesc;
 obj->cb_fun[RRPGE_CB_GETNAME]   = &rrpge_m_cb_getname;
 obj->cb_fun[RRPGE_CB_GETDI]     = &rrpge_m_cb_getdi;
 obj->cb_fun[RRPGE_CB_GETAI]     = &rrpge_m_cb_getai;
 obj->cb_fun[RRPGE_CB_POPCHAR]   = &rrpge_m_cb_popchar;
 obj->cb_fun[RRPGE_CB_CHECKAREA] = &rrpge_cb_checkarea;
 obj->cb_fun[RRPGE_CB_GETLANG]   = &rrpge_m_cb_getlang;
 obj->cb_fun[RRPGE_CB_GETCOLORS] = &rrpge_m_cb_getcolors;
 obj->cb_fun[RRPGE_CB_GETST3D]   = &rrpge_m_cb_getst3d;

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
