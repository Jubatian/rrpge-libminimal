/**
**  \file
**  \brief     Serialization functions
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
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



/* Extracts user data - Implementation of RRPGE library function. */
void rrpge_ropd2udata(rrpge_uint16 const* src, rrpge_udata_t* dst)
{
 auint i;

 for (i=0; i<8; i++){ dst->name[0][i] = src[0xE80U + i]; }
 for (i=0; i<8; i++){ dst->name[1][i] = src[0xE88U + i]; }
 for (i=0; i<8; i++){ dst->name[2][i] = src[0xE90U + i]; }
 for (i=0; i<8; i++){ dst->name[3][i] = src[0xE98U + i]; }
 for (i=0; i<4; i++){
  dst->lnex[i] = ((auint)(src[0xEA0U + (i<<1)]) << 16) +
                  (auint)(src[0xEA1U + (i<<1)]);
 }
 dst->clfg = src[0xEA8U];
 dst->clbg = src[0xEA9U];
}



/* Writes user data into ROPD - Implementation of RRPGE library function. */
void rrpge_udata2ropd(rrpge_udata_t const* src, rrpge_uint16* dst)
{
 auint i;

 for (i=0; i<8; i++){ dst[0xE80U + i] = src->name[0][i]; }
 for (i=0; i<8; i++){ dst[0xE88U + i] = src->name[1][i]; }
 for (i=0; i<8; i++){ dst[0xE90U + i] = src->name[2][i]; }
 for (i=0; i<8; i++){ dst[0xE98U + i] = src->name[3][i]; }
 for (i=0; i<4; i++){
  dst[0xEA0U + (i<<1)] = (uint16)((src->lnex[i]) >> 16);
  dst[0xEA1U + (i<<1)] = (uint16)( src->lnex[i]       );
 }
 dst[0xEA8U] = src->clfg;
 dst[0xEA9U] = src->clbg;
}
