/**
**  \file
**  \brief     Mouse interfacing
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.10.04
*/



#include "imouse.h"



/* Touch sensitive area definition */
typedef struct{
 auint x;
 auint y;
 auint w;
 auint h;
}imouse_touch_t;


/* Touch sensitive areas */
static imouse_touch_t imouse_touch[16];

/* Allocation state (nonzero: allocated, zero: not allocated) */
static auint imouse_als = 0U;

/* Last X & Y coordinates */
static auint imouse_x = 0U;
static auint imouse_y = 0U;

/* Last button states */
static auint imouse_b = 0U;



/*
** Service routine for inputcom_newsrc. Can only return a device source for a
** single mouse device.
*/
auint imouse_newsrc(auint typ)
{
 if (typ != RRPGE_INPT_MOUSE){ return INPUTCOM_NONE; }
 if (imouse_als){ return INPUTCOM_NONE; }
 imouse_als = 1U;
 return 0U;
}



/*
** Service routine for inputcom_isexist. Always returns one (exists) for it's
** mouse device.
*/
auint imouse_isexist(auint id)
{
 if (!imouse_als){ return 0U; }
 if (id == 0U){ return 1U; }
 return 0U;
}



/*
** Service routine for inputcom_freesrc.
*/
void  imouse_freesrc(auint id)
{
 imouse_als = 0U;
}



/*
** Service routine for inputcom_di. Returns mouse buttons and touch sensitive
** area hits.
*/
auint imouse_di(auint id, auint ing)
{
 auint i;
 auint r;

 if (ing == 0U){       /* Return touch */
  if ((imouse_b & 0x0010U) == 0U){ return 0U; } /* No button press */

  r = 0U;              /* Collect areas in which the cursor is */
  for (i = 0U; i < 16U; i++){
   if ( (imouse_x >= (imouse_touch[i].x)) &&
        (imouse_x <  (imouse_touch[i].x + imouse_touch[i].w)) &&
        (imouse_y >= (imouse_touch[i].y)) &&
        (imouse_y <  (imouse_touch[i].y + imouse_touch[i].h)) ){
    r |= (auint)(1U) << i;
   }
  }
  return r;

 }else if (ing == 1U){ /* Return buttons */
  return imouse_b;

 }else{                /* Other input groups are empty */
  return 0U;
 }
}



/*
** Service routine for inputcom_ai. Returns mouse cursor coordinates.
*/
auint imouse_ai(auint di, auint inp)
{
 if (inp == 0U){ return imouse_x; }
 if (inp == 1U){ return imouse_y; }
 return 0U;
}



/*
** Refreshes mouse coordinates. The coordinates must range from 0-639 (X), and
** 0-399 (Y).
*/
void  imouse_setcoords(auint x, auint y)
{
 if (x >= 640U){ imouse_x = 639U; }
 else          { imouse_x = x;    }
 if (y >= 400U){ imouse_y = 399U; }
 else          { imouse_y = y;    }
}



/*
** Refreshes mouse button states. The buttons are as follows:
**
** bit 0: Scroll up
** bit 1: Scroll right
** bit 2: Scroll down
** bit 3: Scroll left
** bit 4: Primary (left) button
** bit 5: Secondary (right) button
** bit 6: Middle button
**
** The bset parameter is a set mask ('1' bits are set), and the bclr parameter
** is a clear mask ('1' bits are cleared).
*/
void  imouse_setbuttons(auint bset, auint bclr)
{
 imouse_b = (imouse_b | bset) & (~bclr);
}
