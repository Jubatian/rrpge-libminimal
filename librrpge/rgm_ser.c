/**
**  \file
**  \brief     Serialization functions
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.08
*/


#include "rgm_ser.h"


/* Byte -> Word page conversion - Implementation of RRPGE library function. */
void rrpge_convpg_b2w(rrpge_uint8 const* src, rrpge_uint16* dst)
{
 auint i = 4096U;

 do{
  i--;
  dst[i] = (uint16)(src[(i << 1) + 1U]) + ((uint16)(src[i << 1]) << 8);
  i--;
  dst[i] = (uint16)(src[(i << 1) + 1U]) + ((uint16)(src[i << 1]) << 8);
 }while (i);
}



/* Word -> Byte page conversion - Implementation of RRPGE library function. */
void rrpge_convpg_w2b(rrpge_uint16 const* src, rrpge_uint8* dst)
{
 auint i = 4096U;

 do{
  i--;
  dst[(i << 1) + 1U] = (uint8)(src[i]);
  dst[(i << 1)     ] = (uint8)(src[i] >> 8);
  i--;
  dst[(i << 1) + 1U] = (uint8)(src[i]);
  dst[(i << 1)     ] = (uint8)(src[i] >> 8);
 }while (i);
}



/* State serialization - Implementation of RRPGE library function */
void rrpge_state2raw(rrpge_state_t const* src, rrpge_uint8* dst)
{
 auint  i;
 uint32 t;

 /* Order is: ROPD; Stack; Data; Video */

 rrpge_convpg_w2b(&(src->ropd[0]), dst);

 for (i = 0; i < 8U; i++){
  rrpge_convpg_w2b(&(src->sram[i * 4096U]), dst + ((i +   1U) * 4096U));
 }

 for (i = 0; i < 224U; i++){
  rrpge_convpg_w2b(&(src->dram[i * 4096U]), dst + ((i +   9U) * 4096U));
 }

 for (i = 0; i< 128U * 4096U; i++){
  t = src->vram[i];
  dst[233U * 4096U + (i << 2) + 0U] = (uint8)(t >> 24);
  dst[233U * 4096U + (i << 2) + 1U] = (uint8)(t >> 16);
  dst[233U * 4096U + (i << 2) + 2U] = (uint8)(t >>  8);
  dst[233U * 4096U + (i << 2) + 3U] = (uint8)(t      );
 }

}
