/**
**  \file
**  \brief     Input device manager.
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.09.04
*/



#include "rgm_dev.h"
#include "rgm_devx.h"
#include "rgm_halt.h"
#include "rgm_stat.h"



/* State: Device bindings (0x0070 - 0x007F), reader */
RRPGE_M_FASTCALL static auint rrpge_m_dev_stat_read_devb(rrpge_object_t* hnd, auint adr, auint rmw)
{
 if (adr > 0xFU){ return 0U; }
 return (hnd->dev.devb[adr] & 0x001FU);
}



/* State: Device bindings (0x0070 - 0x007F), writer */
RRPGE_M_FASTCALL static void rrpge_m_dev_stat_write_devb(rrpge_object_t* hnd, auint adr, auint val)
{
 if (adr < 0x10U){
  hnd->dev.devb[adr] = val & 0x001FU;
 }
}



/* State: Next event high (peek; 0x006E), reader */
RRPGE_M_FASTCALL static auint rrpge_m_dev_stat_read_peekh(rrpge_object_t* hnd, auint adr, auint rmw)
{
 return ((hnd->dev.peek >> 16) & 0xFFFFU);
}



/* State: Next event high (peek; 0x006E), writer */
RRPGE_M_FASTCALL static void rrpge_m_dev_stat_write_peekh(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->dev.peek = (hnd->dev.peek & 0x0000FFFFU) | ((val & 0xFFFFU) << 16);
}



/* State: Next event low (peek; 0x006F), reader */
RRPGE_M_FASTCALL static auint rrpge_m_dev_stat_read_peekl(rrpge_object_t* hnd, auint adr, auint rmw)
{
 return ((hnd->dev.peek) & 0xFFFFU);
}



/* State: Next event low (peek; 0x006F), writer */
RRPGE_M_FASTCALL static void rrpge_m_dev_stat_write_peekl(rrpge_object_t* hnd, auint adr, auint val)
{
 hnd->dev.peek = (hnd->dev.peek & 0xFFFF0000U) | (val & 0xFFFFU);
}






/* Initializes Input device manager adding the appropriate handlers to the
** state manager. */
void  rrpge_m_dev_init(void)
{
 rrpge_m_stat_add_rw_handler(&rrpge_m_dev_stat_read_devb, &rrpge_m_dev_stat_write_devb,
                             RRPGE_STA_VARS + 0x30U, 16U);
 rrpge_m_stat_add_rw_handler(&rrpge_m_dev_stat_read_peekh, &rrpge_m_dev_stat_write_peekh,
                             RRPGE_STA_VARS + 0x2EU, 1U);
 rrpge_m_stat_add_rw_handler(&rrpge_m_dev_stat_read_peekl, &rrpge_m_dev_stat_write_peekl,
                             RRPGE_STA_VARS + 0x2FU, 1U);
}



/* Initializes Input device manager internal resources */
void  rrpge_m_dev_initres(rrpge_object_t* hnd)
{
 auint i;

 /* Input device occupation: clear (no input device added) */

 for (i = 0U; i < RRPGE_M_DEVT_MAX_DEV; i++){
  hnd->dev.devo[i] = 0U;
 }

 /* Internal event FIFO is empty */

 hnd->dev.ffrd = 0U;
 hnd->dev.ffwr = 0U;
}






/* Internal function to put stuff on the application event queue */
static void rrpge_m_dev_fifoadd(rrpge_object_t* hnd, auint evt)
{
 if ((((hnd->dev.ffwr) + 1U) & (RRPGE_M_DEVT_FIFOLEN - 1U)) == (hnd->dev.ffrd)){
  return; /* FIFO full, discard event */
 }
 hnd->dev.fifo[hnd->dev.ffwr] = evt;
 hnd->dev.ffwr = ((hnd->dev.ffwr) + 1U) & (RRPGE_M_DEVT_FIFOLEN - 1U);
}



/* Internal function to sum device types for physical and app. required
** devices. The sums for each type go in phc and apc, physical and app.
** requirements, respectively. */
static void rrpge_m_dev_sumtyp(rrpge_object_t* hnd, auint* phc, auint* apc)
{
 auint i;

 /* Count physical devices of each type */

 for (i = 0U; i < 16U; i++){
  phc[i] = 0U;
 }
 for (i = 0U; i < RRPGE_M_DEVT_MAX_DEV; i++){
  if (hnd->dev.devo[i] != 0U){
   phc[hnd->dev.ds[i].typ & 0xFU] ++;
  }
 }

 /* Count application required devices of each type */

 for (i = 0U; i < 16U; i++){
  apc[i] = 0U;
 }
 for (i = 0U; i < 16U; i++){
  if (hnd->dev.devb[i] != 0U){
   apc[hnd->dev.devb[i] & 0xFU] ++;
  }
 }
}



/* Processes events into the application side event fifo. This should be
** called once every display frame to conform with the specification, since
** this is where source events are combined. */
void  rrpge_m_dev_proc(rrpge_object_t* hnd)
{
 auint bls[16][RRPGE_M_DEVT_MAX_DEV + 1U]; /* Device binding list */
 auint phc[16];     /* Physical device count of each type */
 auint apc[16];     /* Application required device count of each type */
 auint isb[RRPGE_M_DEVT_MAX_DEV]; /* Was the device already bound? */
 auint cur;
 auint prv;
 auint evt;
 auint typ;
 auint cyb;
 auint cye;
 auint i;
 auint j;
 auint c;
 auint t;



 /* Process device translations where necessary */

 rrpge_m_devx_proc(hnd);



 /* Sum devices by type (physical; app. requirements) */

 rrpge_m_dev_sumtyp(hnd, &phc[0], &apc[0]);



 /* First major pass: Bind physical devices to current application device
 ** requirements. This determines which physical device will send events to
 ** which application required device. */

 for (i = 0U; i < RRPGE_M_DEVT_MAX_DEV; i++){
  isb[i] = 0U; /* Devices not bound yet */
 }

 for (i = 0U; i < 16U; i++){

  if (hnd->dev.devb[i] != 0U){ /* Application requirement exists */

   switch (hnd->dev.devb[i] & 0xFU){

    case RRPGE_DEV_POINT:

     /* Assume requiring only one pointing device, and add all pointing and
     ** touch physical devices to it as sources. */

     c = 0U;
     for (j = 0U; j < RRPGE_M_DEVT_MAX_DEV; j++){
      if (hnd->dev.devo[j] != 0U){
       if ( ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_POINT) ||
            ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_TOUCH) ){
        bls[i][c] = j;
        isb[j] = 1U;
        c ++;
       }
      }
     }
     bls[i][c] = RRPGE_M_DEVT_MAX_DEV;
     break;

    case RRPGE_DEV_PAD:

     /* The keyboard(s) normally combine with the first gamepad. However if
     ** more gamepads are needed than physically available, this relation is
     ** broken, and the keyboard(s) get to emulate the first gamepad. */

     c = 0U;
     if ( (apc[RRPGE_DEV_KEYB] == 0U) &&
          (apc[RRPGE_DEV_TEXT] == 0U) ){ /* Only use keyboards unless they are needed by the app. */
      for (j = 0U; j < RRPGE_M_DEVT_MAX_DEV; j++){
       if (hnd->dev.devo[j] != 0U){
        if ( ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_KEYB) &&
             (isb[j] == 0U) ){
         bls[i][c] = j;
         isb[j] = 1U;
         c ++;
        }
       }
      }
     }
     if ( (c == 0U) ||                   /* No keyboard */
          (phc[RRPGE_DEV_PAD] >= apc[RRPGE_DEV_PAD]) ){ /* There are enough physical pads */
      for (j = 0U; j < RRPGE_M_DEVT_MAX_DEV; j++){
       if (hnd->dev.devo[j] != 0U){
        if ( ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_PAD) &&
             (isb[j] == 0U) ){
         bls[i][c] = j;
         isb[j] = 1U;
         c ++;
         if (apc[RRPGE_DEV_PAD] > 1U){ break; } /* Bind only one physical gamepad */
        }
       }
      }
     }
     bls[i][c] = RRPGE_M_DEVT_MAX_DEV;
     break;

    case RRPGE_DEV_KEYB:

     /* Keyboards normally combine with gamepads, so unless gamepads are
     ** specifically needed, they are also bound here. */

     c = 0U;
     for (j = 0U; j < RRPGE_M_DEVT_MAX_DEV; j++){
      if (hnd->dev.devo[j] != 0U){
       if ( ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_KEYB) ||
            ( ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_PAD) &&
              (apc[RRPGE_DEV_PAD] == 0U) ) ){
        bls[i][c] = j;
        isb[j] = 1U;
        c ++;
       }
      }
     }
     bls[i][c] = RRPGE_M_DEVT_MAX_DEV;
     break;

    case RRPGE_DEV_TEXT:

     /* Assume requiring only one device of this type, and add all matching
     ** physical devices to it as sources. Keyboards and gamepads are also
     ** added (the RRPGE_DEV_KBTXT flag only affects how a keyboard is
     ** processed). */

     c = 0U;
     for (j = 0U; j < RRPGE_M_DEVT_MAX_DEV; j++){
      if (hnd->dev.devo[j] != 0U){
       if ( ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_TEXT) ||
            ( ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_KEYB) &&
              (apc[RRPGE_DEV_KEYB] == 0U) ) ||
            ( ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_PAD ) &&
              (apc[RRPGE_DEV_PAD ] == 0U) ) ){
        bls[i][c] = j;
        isb[j] = 1U;
        c ++;
       }
      }
     }
     bls[i][c] = RRPGE_M_DEVT_MAX_DEV;
     break;

    case RRPGE_DEV_STICK:

     /* If only a single stick is required, all sticks provide input for it,
     ** otherwise they are allocated one by one. */

     c = 0U;
     for (j = 0U; j < RRPGE_M_DEVT_MAX_DEV; j++){
      if (hnd->dev.devo[j] != 0U){
       if ( ((hnd->dev.ds[j].typ & 0xFU) == RRPGE_DEV_STICK) &&
            (isb[j] == 0U) ){
        bls[i][c] = j;
        isb[j] = 1U;
        c ++;
        if (apc[RRPGE_DEV_STICK] > 1U){ break; } /* Bind only one physical stick */
       }
      }
     }
     bls[i][c] = RRPGE_M_DEVT_MAX_DEV;
     break;

    case RRPGE_DEV_TOUCH:
    default:

     /* Assume requiring only one device of this type, and add all matching
     ** physical devices to it as sources. */

     c = 0U;
     for (j = 0U; j < RRPGE_M_DEVT_MAX_DEV; j++){
      if (hnd->dev.devo[j] != 0U){
       if ((hnd->dev.ds[j].typ & 0xFU) == (hnd->dev.devb[i] & 0xFU)){
        bls[i][c] = j;
        isb[j] = 1U;
        c ++;
       }
      }
     }
     bls[i][c] = RRPGE_M_DEVT_MAX_DEV;
     break;

   }

  }

 }



 /* Second major pass: For each device requested by the application, generate
 ** the appropriate stream of events into the FIFO. */

 for (i = 0U; i < 16U; i++){

  if (hnd->dev.devb[i] != 0U){ /* Application requirement exists */

   typ = hnd->dev.devb[i] & 0xFU;
   evt = (i << 28) | (typ << 20);

   /* Pointing and touch devices have different button input than the rest,
   ** providing X & Y locations with presses. */

   if ( (typ == RRPGE_DEV_POINT) ||
        (typ == RRPGE_DEV_TOUCH) ){

    for (j = 1U; j < RRPGE_M_DEVT_MAX_TOU; j++){ /* Buttons / touches */
     cur = 0U;
     prv = 0U;
     c = 0U;
     t = 0U; /* Will store the device whose X & Y coordinates should be used */
     while (bls[i][c] != RRPGE_M_DEVT_MAX_DEV){
      if (hnd->dev.ds[bls[i][c]].btnc[j] != 0U){ cur = 1U; t = c; }
      if (hnd->dev.ds[bls[i][c]].btnp[j] != 0U){ prv = 1U; }
      c ++;
     }
     if (cur != prv){ /* Button state changed: generate events */
      if (cur != 0U){ /* Press */
       rrpge_m_dev_fifoadd(hnd, evt | (0U << 24) | (0U << 16) | j | 0x08000000U);
       rrpge_m_dev_fifoadd(hnd, evt | (1U << 24) | (0U << 16) | (hnd->dev.ds[bls[i][t]].prcx[j] & 0xFFFFU));
       rrpge_m_dev_fifoadd(hnd, evt | (2U << 24) | (0U << 16) | (hnd->dev.ds[bls[i][t]].prcy[j] & 0xFFFFU));
      }else{          /* Release */
       rrpge_m_dev_fifoadd(hnd, evt | (0U << 24) | (1U << 16) | j | 0x08000000U);
      }
     }
    }

   }

   /* Keyboards, digital gamepads and joysticks have similar button inputs,
   ** so can be served together */

   if ( (typ == RRPGE_DEV_KEYB)  ||
        (typ == RRPGE_DEV_PAD)   ||
        (typ == RRPGE_DEV_STICK) ){

    if (typ == RRPGE_DEV_KEYB){ /* Button ranges differ */
     cyb = 8U;
     cye = RRPGE_M_DEVT_MAX_BTN;
    }else{
     cyb = 1U;
     cye = 8U;
    }

    for (j = cyb; j < cye; j++){ /* Buttons */
     cur = 0U;
     prv = 0U;
     c = 0U;
     while (bls[i][c] != RRPGE_M_DEVT_MAX_DEV){
      if (hnd->dev.ds[bls[i][c]].btnc[j] != 0U){ cur = 1U; }
      if (hnd->dev.ds[bls[i][c]].btnp[j] != 0U){ prv = 1U; }
      c ++;
     }
     if (cur != prv){ /* Button state changed: generate events */
      if (cur != 0U){ /* Press */
       rrpge_m_dev_fifoadd(hnd, evt | (0U << 24) | (0U << 16) | j | 0x08000000U);
      }else{          /* Release */
       rrpge_m_dev_fifoadd(hnd, evt | (0U << 24) | (1U << 16) | j | 0x08000000U);
      }
     }
    }

   }

   /* Analog style inputs are provided by pointing devices and joysticks */

   if ( (typ == RRPGE_DEV_POINT) ||
        (typ == RRPGE_DEV_TOUCH) ||
        (typ == RRPGE_DEV_STICK) ){

    for (j = 0U; j < 6U; j++){ /* Analog style inputs */
     cur = 0U;
     prv = 0U;
     c = 0U;
     while (bls[i][c] != RRPGE_M_DEVT_MAX_DEV){
      cur = hnd->dev.ds[bls[i][c]].ainc[j];
      prv = hnd->dev.ds[bls[i][c]].ainp[j];
      if (cur != prv){ break; }
      c ++;
     }
     if (cur != prv){ /* Changed (Jitter-free operation should be ensured by host) */
      rrpge_m_dev_fifoadd(hnd, evt | (0U << 24) | ((j + 2U) << 16) | (cur & 0xFFFFU) | 0x08000000U);
     }
    }

   }

   /* Unmanaged events are provided by text input (Control and Character
   ** inputs) */

   if ( (typ == RRPGE_DEV_TEXT) ){

    c = 0U;
    while (bls[i][c] != RRPGE_M_DEVT_MAX_DEV){
     for (j = 0U; j < hnd->dev.ds[bls[i][c]].evtn; j++){
      rrpge_m_dev_fifoadd(hnd, evt | (hnd->dev.ds[bls[i][c]].evtq[j] & 0x0F0FFFFFU));
     }
     c ++;
    }

   }

  }

 }



 /* Cleanup: clear the previous states in input sources since they are now
 ** serviced (gone into the FIFO). Also empty the unmanaged event queues. */

 for (i = 0U; i < RRPGE_M_DEVT_MAX_DEV; i++){

  for (j = 0U; j < RRPGE_M_DEVT_MAX_BTN; j++){
   hnd->dev.ds[i].btnp[j] = hnd->dev.ds[i].btnc[j];
  }
  for (j = 0U; j < 6U; j++){
   hnd->dev.ds[i].ainp[j] = hnd->dev.ds[i].ainc[j];
  }
  hnd->dev.ds[i].evtn = 0U;

 }
}







/* Registers a device from application side. Returns nonzero if this was
** possible (there is a physical device which can serve it */
auint rrpge_m_dev_req(rrpge_object_t* hnd, auint id, auint typ)
{
 auint phc[16];     /* Physical device count of each type */
 auint apc[16];     /* Application required device count of each type */
 auint ac;

 /* Drop device (if already bound) */

 rrpge_m_dev_drop(hnd, id);

 /* Sum devices by type (physical; app. requirements) */

 rrpge_m_dev_sumtyp(hnd, &phc[0], &apc[0]);

 /* Can the request succeed? Check by device type */

 typ &= 0xFU;
 switch (typ){

  case RRPGE_DEV_POINT:
   if ((apc[RRPGE_DEV_POINT] + apc[RRPGE_DEV_TOUCH]) != 0U){
    return 0U;                     /* At most one device may be used */
   }
   if ((phc[RRPGE_DEV_POINT] + phc[RRPGE_DEV_TOUCH]) == 0U){
    return 0U;                     /* No suitable physical device present */
   }
   break;

  case RRPGE_DEV_TOUCH:
   if ((apc[RRPGE_DEV_POINT] + apc[RRPGE_DEV_TOUCH]) != 0U){
    return 0U;                     /* At most one device may be used */
   }
   if ( phc[RRPGE_DEV_TOUCH] == 0U){
    return 0U;                     /* No suitable physical device present */
   }
   break;

  case RRPGE_DEV_PAD:
   ac = phc[RRPGE_DEV_PAD];
   if ((apc[RRPGE_DEV_KEYB]  == 0U) &&
       (apc[RRPGE_DEV_TEXT]  == 0U) &&
       (phc[RRPGE_DEV_KEYB]  != 0U)){
    ac ++;                         /* Add keyboard to available device count */
   }
   if ( apc[RRPGE_DEV_PAD]   >= ac){
    return 0U;                     /* Not enough suitable physical devices */
   }
   break;

  case RRPGE_DEV_STICK:
   if ( apc[RRPGE_DEV_STICK] >= phc[RRPGE_DEV_STICK]){
    return 0U;                     /* Not enough suitable physical devices */
   }
   break;

  case RRPGE_DEV_TEXT:
   if ( apc[RRPGE_DEV_TEXT]  != 0U){
    return 0U;                     /* At most one device may be used */
   }
   ac = phc[RRPGE_DEV_TEXT];
   if ((apc[RRPGE_DEV_KEYB]  == 0U) &&
       (apc[RRPGE_DEV_PAD]   == 0U) &&
       (phc[RRPGE_DEV_KEYB]  != 0U)){
    ac ++;                         /* Add keyboard to available device count */
   }
   if (ac == 0U){
    return 0U;                     /* No suitable physical device present */
   }
   break;

  case RRPGE_DEV_KEYB:
  default:
   if ( apc[typ] != 0U){
    return 0U;                     /* At most one device may be used */
   }
   if ( phc[typ] == 0U){
    return 0U;                     /* No suitable physical device present */
   }
   break;

 }

 /* OK, device can be serviced, so bind it */

 hnd->dev.devb[id & 0xFU] |= typ | 0x10U;

 return 1U;
}



/* Drops a device from application side. */
void  rrpge_m_dev_drop(rrpge_object_t* hnd, auint id)
{
 hnd->dev.devb[id & 0xFU] = 0U;
}



/* Peeks an event from the application side FIFO. This removes it from the
** FIFO, putting it into the state (0x006E - 0x006F; only unless already done,
** that is, the state is empty), so it is retained as described in the
** specification. */
auint rrpge_m_dev_peek(rrpge_object_t* hnd)
{
 if (hnd->dev.peek != 0U){ return hnd->dev.peek; } /* Peek already stored */
 if (hnd->dev.ffwr == hnd->dev.ffrd){ return 0U; } /* FIFO empty */
 hnd->dev.peek = hnd->dev.fifo[hnd->dev.ffrd];
 hnd->dev.ffrd = ((hnd->dev.ffrd) + 1U) & (RRPGE_M_DEVT_FIFOLEN - 1U);
 return hnd->dev.peek;
}



/* Pops off an event from the application side FIFO. Either clears the state
** or attempts getting a new event from the FIFO depending on the contents of
** the state. */
auint rrpge_m_dev_pop(rrpge_object_t* hnd)
{
 auint peek = rrpge_m_dev_peek(hnd);
 hnd->dev.peek = 0U;
 return peek;
}



/* Flushes event FIFO and state. */
void  rrpge_m_dev_flush(rrpge_object_t* hnd)
{
 hnd->dev.peek = 0U;
 hnd->dev.ffrd = hnd->dev.ffwr;
}



/* Register input device - implementation of RRPGE library function */
rrpge_iuint rrpge_dev_add(rrpge_object_t* hnd, rrpge_iuint typ)
{
 auint d;
 auint i;

 /* Find first empty slot in physical input device list */

 for (d = 0U; d < RRPGE_M_DEVT_MAX_DEV; d++){
  if (hnd->dev.devo[d] == 0U){ break; }
 }
 if (d == RRPGE_M_DEVT_MAX_DEV){ return d; } /* Don't register, return dummy ID */

 /* Add the input device, starting with a clean state */

 hnd->dev.devo[d] = 1U;

 for (i = 0U; i < RRPGE_M_DEVT_MAX_BTN; i++){
  hnd->dev.ds[d].btnc[i] = 0U;
  hnd->dev.ds[d].btnp[i] = 0U;
  hnd->dev.ds[d].btnt[i] = 0U;
 }
 for (i = 0U; i < 6U; i++){
  hnd->dev.ds[d].ainc[i] = 0U;
  hnd->dev.ds[d].ainp[i] = 0U;
 }
 hnd->dev.ds[d].evtn = 0U;

 /* Set its type, and done */

 hnd->dev.ds[d].typ = typ;
 return d;
}



/* Remove input device - implementation of RRPGE library function */
void rrpge_dev_rem(rrpge_object_t* hnd, rrpge_iuint dev)
{
 if (dev >= RRPGE_M_DEVT_MAX_DEV){ return; }
 hnd->dev.devo[dev] = 0U;
}



/* Push an input event - implementation of RRPGE library function */
void rrpge_dev_push(rrpge_object_t* hnd, rrpge_iuint dev,
                    rrpge_iuint emt, rrpge_iuint cnt, rrpge_uint16 const* msg)
{
 auint i;
 auint t;

 /* Discard invalid or not even registered input devices */

 if (dev >= RRPGE_M_DEVT_MAX_DEV){ return; }
 if (hnd->dev.devo[dev] == 0U){ return; }

 /* At least one data member should be supplied, otherwise ignore. Also drop
 ** too long events. */

 if (cnt == 0U){ return; }
 if (cnt >  8U){ return; }

 /* Process events by message type */

 if       (emt == 0U){                 /* Button or touch press */

  if (msg[0] < RRPGE_M_DEVT_MAX_TOU){  /* Potential touch */
   if (cnt < 3U){                      /* No coordinates supplied */
    hnd->dev.ds[dev].prcx[msg[0]] = hnd->dev.ds[dev].ainc[0]; /* Simply use first analog inputs. */
    hnd->dev.ds[dev].prcy[msg[0]] = hnd->dev.ds[dev].ainc[1]; /* This is useful for a mouse not providing this info. */
   }else{                              /* Coordinates provided OK */
    hnd->dev.ds[dev].prcx[msg[0]] = msg[1];
    hnd->dev.ds[dev].prcy[msg[0]] = msg[2];
   }
  }
  if (msg[0] < RRPGE_M_DEVT_MAX_BTN){
   hnd->dev.ds[dev].btnc[msg[0]] = 1U; /* Pressed */
  }

 }else if (emt == 1U){                 /* Button or touch release */

  if (msg[0] < RRPGE_M_DEVT_MAX_BTN){
   hnd->dev.ds[dev].btnc[msg[0]] = 0U; /* Released */
  }

 }else if (emt <  8U){                 /* Analog style inputs */

  hnd->dev.ds[dev].ainc[emt - 2U] = msg[0];

 }else{                                /* Unmanaged event */

  for (i = 0U; i < cnt; i++){
   if (hnd->dev.ds[dev].evtn < RRPGE_M_DEVT_MAX_EVT){ /* Discard if queue is full */
    t  = msg[i];
    if (i == 0U){ t |= 0x08000000U; }  /* First data element marker */
    t |= i << 24U;                     /* Event data index */
    t |= (emt & 0xFU) << 16U;          /* Event message type */
    hnd->dev.ds[dev].evtq[hnd->dev.ds[dev].evtn] = t;
    hnd->dev.ds[dev].evtn ++;
   }
  }

 }

}
