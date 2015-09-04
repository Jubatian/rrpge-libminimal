/**
**  \file
**  \brief     Device translations.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.04
**
**
**  Translates between device types where appropriate, mostly relating the
**  keyboard. Between keyboard and gamepad it relies on that no keyboard
**  button is normally mapped to the 7 gamepad button ID's, so a device may
**  simply carry both information, and it might be isolated when mapping to an
**  application device only (by taking buttons up to and including button 7
**  for gamepads, and from and including button 8 for keyboards).
**
**  Also calls the text input mapper for the keyboard (in a way the gamepad
**  inputs can propagate into text input as required by the specification).
*/


#include "rgm_info.h"
#include "rgm_devk.h"


/* Button forward from src to destination. */
static void rrpge_m_devx_bfwd(rrpge_object_t* hnd, auint id, auint src, auint dst)
{
 hnd->dev.ds[id].btnc[dst] |= hnd->dev.ds[id].btnc[src];
}


/* Processes device translations */
void  rrpge_m_devx_proc(rrpge_object_t* hnd)
{
 auint i;
 auint j;
 auint typ;

 for (i = 0U; i < RRPGE_M_DEVT_MAX_DEV; i++){

  if (hnd->dev.devo[i] != 0U){                /* Device exists */

   typ = hnd->dev.ds[i].typ & 0xFU;

   if (typ == RRPGE_DEV_PAD){                 /* Generate keyboard and text input events */

    for (j = 8U; j < RRPGE_M_DEVT_MAX_BTN; j++){ hnd->dev.ds[i].btnc[j] = 0U; }

    rrpge_m_devx_bfwd(hnd, i, 0x01U, 0x0AU);  /* Primary action => ENTER */
    rrpge_m_devx_bfwd(hnd, i, 0x02U, 0x95U);  /* Secondary action => FN */
    rrpge_m_devx_bfwd(hnd, i, 0x03U, 0x94U);  /* Menu => ESC */
    if (hnd->dev.ds[i].btnc[0x95] == 0U){     /* State of FN key */
     rrpge_m_devx_bfwd(hnd, i, 0x04U, 0x82U); /* UP => UP */
     rrpge_m_devx_bfwd(hnd, i, 0x05U, 0x83U); /* RIGHT => RIGHT */
     rrpge_m_devx_bfwd(hnd, i, 0x06U, 0x84U); /* DOWN => DOWN */
     rrpge_m_devx_bfwd(hnd, i, 0x07U, 0x85U); /* LEFT => LEFT */
    }else{
     rrpge_m_devx_bfwd(hnd, i, 0x04U, 0x86U); /* UP => Page Up */
     rrpge_m_devx_bfwd(hnd, i, 0x05U, 0x87U); /* RIGHT => End */
     rrpge_m_devx_bfwd(hnd, i, 0x06U, 0x88U); /* DOWN => Page Down */
     rrpge_m_devx_bfwd(hnd, i, 0x07U, 0x89U); /* LEFT => Home */
    }

    rrpge_m_devk_proc(hnd, i);                /* Make text input */

   }else if (typ == RRPGE_DEV_KEYB){          /* Generate gamepad and text input */

    for (j = 0U; j < 8U; j++){ hnd->dev.ds[i].btnc[j] = 0U; }

    rrpge_m_devx_bfwd(hnd, i, 0x92U, 0x01U);  /* CTRL => Primary action */
    rrpge_m_devx_bfwd(hnd, i, 0x20U, 0x01U);  /* SPACE => Primary action */
    rrpge_m_devx_bfwd(hnd, i, 0x93U, 0x02U);  /* ALT => Secondary action */
    rrpge_m_devx_bfwd(hnd, i, 0x94U, 0x03U);  /* ESC => Menu */
    rrpge_m_devx_bfwd(hnd, i, 0x82U, 0x04U);  /* UP => UP */
    rrpge_m_devx_bfwd(hnd, i, 0x83U, 0x05U);  /* RIGHT => RIGHT */
    rrpge_m_devx_bfwd(hnd, i, 0x84U, 0x06U);  /* DOWN => DOWN */
    rrpge_m_devx_bfwd(hnd, i, 0x85U, 0x07U);  /* LEFT => LEFT */
    rrpge_m_devx_bfwd(hnd, i, 0x86U, 0x04U);  /* Page Up => UP */
    rrpge_m_devx_bfwd(hnd, i, 0x87U, 0x05U);  /* End => RIGHT */
    rrpge_m_devx_bfwd(hnd, i, 0x88U, 0x06U);  /* Page Down => DOWN */
    rrpge_m_devx_bfwd(hnd, i, 0x89U, 0x07U);  /* Home => LEFT */

    if ((hnd->dev.ds[i].typ & RRPGE_DEV_KBTXT) == 0U){
     rrpge_m_devk_proc(hnd, i);               /* Make text input unless it has own */
    }

   }else{}                                    /* Other devices have no translation */

  }

 }
}
