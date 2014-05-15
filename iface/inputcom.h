/**
**  \file
**  \brief     Common input device handler
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.15
**
**
**  Common handler for input devices interfacing the RRPGE library. It
**  supports multiple sources for inputs, so making it possible to create
**  handler modules for each device individually.
*/


#ifndef INPUTCOM_H
#define INPUTCOM_H


#include "../host/types.h"
#include "../librrpge/rrpge.h"


/* No more input sources available */
#define INPUTCOM_NONE (~(auint)(0U))


/*
** Used to allocate new input sources. Returns next input source ID for the
** module if any. If there are no more input sources, it has to return
** INPUTCOM_NONE. The handler will use the returned ID as object handle in
** further requests. The passed parameter is the RRPGE type needing service.
** The module must only respond with a valid input source ID if it is able to
** service this passed type.
*/
typedef auint inputcom_newsrc_t(auint typ);



/*
** Used to check if the input source is still available. If an input source is
** removed, the module should keep responding with inactivity to requests
** covering it until the source is freed. If the input source is not available
** any more, zero needs to be returned, otherwise nonzero.
*/
typedef auint inputcom_isexist_t(auint id);



/*
** Frees an input source. The input handler may use this to deallocate
** resources used for servicing that source.
*/
typedef void  inputcom_freesrc_t(auint id);



/*
** Returns a digital input group's state for a given input source. The low 16
** bits of the return value has to be populated with the appropriate input
** states. The first parameter of the function is the input source ID, the
** second is the input group.
*/
typedef auint inputcom_di_t(auint id, auint ing);



/*
** Returns an analog input's state for a given device. The return has to be
** populated with a 2's complement value (which will be saturated to the
** -0x8000 - 0x7FFF range). The first parameter of the function is the input
** source ID, the second is the input.
*/
typedef auint inputcom_ai_t(auint id, auint inp);



/*
** Accepts the coordinate set for a touch sensitive area. Note that touch
** sensitive areas are independent of the input sources, the source may use
** them if necessary (such as if it is a mouse) to provide the appropriate
** inputs. If the area is cleared (not defined), either width or height is set
** to zero.
*/
typedef void  inputcom_settouch_t(auint ari, auint x, auint y, auint w, auint h);



/*
** A module's callback package to send to this handler. Callbacks may be set
** NULL if the module need not use them except for newsrc and isexist.
*/
typedef struct{
 inputcom_newsrc_t*   newsrc;
 inputcom_isexist_t*  isexist;
 inputcom_freesrc_t*  freesrc;
 inputcom_di_t*       di;
 inputcom_ai_t*       ai;
 inputcom_settouch_t* settouch;
}inputcom_cb_t;



/*
** Register handler module. Note that the modules are probed in the order they
** are registered, so the first registered module will get a chance to service
** a type first.
*/
void inputcom_register(inputcom_cb_t const* cb);


/*
** Resets input system state according to the given application. If the
** application excepts inputs (restored from a state), the appropriate input
** sources are also opened.
*/
void inputcom_reset(rrpge_object_t* hnd);


/*
** Get device properties callback service routine.
*/
rrpge_uint32 inputcom_getprops(rrpge_object_t* hnd, const void* par);

/*
** Drop device callback service routine.
*/
void         inputcom_dropdev(rrpge_object_t* hnd, const void* par);

/*
** Get digital input description symbols callback service routine.
*/
rrpge_uint32 inputcom_getdidesc(rrpge_object_t* hnd, const void* par);

/*
** Get digital inputs callback service routine.
*/
rrpge_uint32 inputcom_getdi(rrpge_object_t* hnd, const void* par);

/*
** Get analog inputs callback service routine.
*/
rrpge_uint32 inputcom_getai(rrpge_object_t* hnd, const void* par);

/*
** Pop text FIFO callback service routine.
*/
rrpge_uint32 inputcom_popchar(rrpge_object_t* hnd, const void* par);

/*
** Set touch sensitive area callback service routine.
*/
void         inputcom_settouch(rrpge_object_t* hnd, const void* par);


#endif
