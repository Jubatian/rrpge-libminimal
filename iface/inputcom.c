/**
**  \file
**  \brief     Common input device handler
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#include "inputcom.h"



/* Number of input handlers supported. Attempting to register more will be
** ignored. */
#define INPUTCOM_HCNT 8U


/* Input handlers */
static inputcom_cb_t inputcom_handlers[8];
/* Count of registered handlers */
static auint inputcom_handler_cnt = 0U;

/* Handler associated to each device, identified by the handler's index */
static auint inputcom_dev_hnd[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/* Type of each device */
static auint inputcom_dev_typ[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/* Input source within handler for each device. If it is INPUTCOM_NONE, it
** identifies that the device has no handler associated, so no input. */
static auint inputcom_dev_src[16] = {INPUTCOM_NONE, INPUTCOM_NONE, INPUTCOM_NONE, INPUTCOM_NONE,
                                     INPUTCOM_NONE, INPUTCOM_NONE, INPUTCOM_NONE, INPUTCOM_NONE,
                                     INPUTCOM_NONE, INPUTCOM_NONE, INPUTCOM_NONE, INPUTCOM_NONE,
                                     INPUTCOM_NONE, INPUTCOM_NONE, INPUTCOM_NONE, INPUTCOM_NONE};



/*
** Register handler module. Note that the modules are probed in the order they
** are registered, so the first registered module will get a chance to service
** a type first.
*/
void inputcom_register(inputcom_cb_t const* cb)
{
 if (inputcom_handler_cnt >= INPUTCOM_HCNT){ return; }
 inputcom_handlers[inputcom_handler_cnt].newsrc   = cb->newsrc;
 inputcom_handlers[inputcom_handler_cnt].isexist  = cb->isexist;
 inputcom_handlers[inputcom_handler_cnt].freesrc  = cb->freesrc;
 inputcom_handlers[inputcom_handler_cnt].di       = cb->di;
 inputcom_handlers[inputcom_handler_cnt].ai       = cb->ai;
 inputcom_handler_cnt++;
}



/*
** Resets input system state according to the given application. If the
** application excepts inputs (restored from a state), the appropriate input
** sources are also opened.
*/
void inputcom_reset(rrpge_object_t* hnd)
{
 auint i;
 auint j;
 auint t;

 /* If any controller is allocated, free them */

 for (i = 0U; i < 16U; i++){
  if (inputcom_dev_src[i] != INPUTCOM_NONE){ /* There is an input source associated */
   t = inputcom_dev_hnd[i];
   if (inputcom_handlers[t].freesrc){
    inputcom_handlers[t].freesrc(inputcom_dev_src[i]);
   }
   inputcom_dev_src[i] = INPUTCOM_NONE;
  }
 }

 /* Allocate devices if the application state requires it */

 for (i = 0U; i < 16U; i++){
  t = rrpge_getlastdev(hnd, i);
  if (t != 0U){
   inputcom_dev_typ[i] = t >> 12; /* If possible to allocate, this will be it's type */
   for (j = 0U; j < inputcom_handler_cnt; j++){
    if (inputcom_handlers[j].newsrc){
     inputcom_dev_src[i] = inputcom_handlers[j].newsrc(t >> 12);
    }
    if (inputcom_dev_src[i] != INPUTCOM_NONE){ break; }
   }
  }
 }
}



/*
** Get device properties callback service routine.
*/
rrpge_iuint inputcom_getprops(rrpge_object_t* hnd, const void* par)
{
 rrpge_cbp_getprops_t const* p = (rrpge_cbp_getprops_t const*)(par);
 auint t;
 auint j;
 auint i;

 /* If there is no device available at the index, try to get one */

 if (inputcom_dev_src[(p->dev) & 0xFU] == INPUTCOM_NONE){
  t = rrpge_getalloweddevs(hnd);
  for (j = 0U; j < inputcom_handler_cnt; j++){
   if (inputcom_handlers[j].newsrc){
    for (i = 0U; i < 16U; i++){
     if ((t & (1U << i)) != 0U){ /* Dev. type is allowed */
      inputcom_dev_typ[(p->dev) & 0xFU] = i;
      inputcom_dev_src[(p->dev) & 0xFU] = inputcom_handlers[j].newsrc(i);
      if (inputcom_dev_src[(p->dev) & 0xFU] != INPUTCOM_NONE){ break; }
     }
    }
    if (i < 16U){ break; } /* Device allocated before drying up the loop */
   }
  }
 }

 /* Now either there is a valid device allocated, or not, return */

 if (inputcom_dev_src[(p->dev) & 0xFU] == INPUTCOM_NONE){
  return 0U;
 }else{
  return (inputcom_dev_typ[(p->dev) & 0xFU] << 12) | 0x0800U;
 }
}



/*
** Drop device callback service routine.
*/
void        inputcom_dropdev(rrpge_object_t* hnd, const void* par)
{
 rrpge_cbp_dropdev_t const* p = (rrpge_cbp_dropdev_t const*)(par);
 auint t;

 /* If there is a device available at the index, free it */

 if (inputcom_dev_src[(p->dev) & 0xFU] != INPUTCOM_NONE){
  t = inputcom_dev_hnd[(p->dev) & 0xFU];
  if (inputcom_handlers[t].freesrc){
   inputcom_handlers[t].freesrc(inputcom_dev_src[(p->dev) & 0xFU]);
  }
  inputcom_dev_src[(p->dev) & 0xFU] = INPUTCOM_NONE;
 }
}



/*
** Get digital input description symbols callback service routine.
*/
rrpge_iuint inputcom_getdidesc(rrpge_object_t* hnd, const void* par)
{
 return 0xFFFFFFFFU; /* Not implemented yet. Return native control for now. */
}



/*
** Get digital inputs callback service routine.
*/
rrpge_iuint inputcom_getdi(rrpge_object_t* hnd, const void* par)
{
 rrpge_cbp_getdi_t const* p = (rrpge_cbp_getdi_t const*)(par);
 auint t;

 if (inputcom_dev_src[(p->dev) & 0xFU] == INPUTCOM_NONE){ return 0U; }
 t = inputcom_dev_hnd[(p->dev) & 0xFU];
 if (!(inputcom_handlers[t].di)){ return 0U; }
 return inputcom_handlers[t].di(inputcom_dev_src[(p->dev) & 0xFU], p->ing);
}



/*
** Get analog inputs callback service routine.
*/
rrpge_iuint inputcom_getai(rrpge_object_t* hnd, const void* par)
{
 rrpge_cbp_getai_t const* p = (rrpge_cbp_getai_t const*)(par);
 auint t;

 if (inputcom_dev_src[(p->dev) & 0xFU] == INPUTCOM_NONE){ return 0U; }
 t = inputcom_dev_hnd[(p->dev) & 0xFU];
 if (!(inputcom_handlers[t].ai)){ return 0U; }
 return (inputcom_handlers[t].ai(inputcom_dev_src[(p->dev) & 0xFU], p->inp) & 0xFFFFU);
}



/*
** Pop text FIFO callback service routine.
*/
rrpge_iuint inputcom_popchar(rrpge_object_t* hnd, const void* par)
{
 return 0U; /* Not implemented yet. Returns no character in the FIFO for now. */
}
