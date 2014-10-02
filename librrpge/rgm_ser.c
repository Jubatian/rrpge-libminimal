/**
**  \file
**  \brief     Serialization functions
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.10.02
*/


#include "rgm_ser.h"


/* Byte -> Word page conversion - Implementation of RRPGE library function. */
void rrpge_conv_b2w(rrpge_uint8 const* src, rrpge_uint16* dst, rrpge_uint32 bct)
{
 if ((bct & 1U) != 0U){
  bct -= 1U;
  dst[bct >> 1] = (uint16)(src[bct]) << 8;
 }
 while (bct != 0U){
  bct -= 2U;
  dst[bct >> 1] = ((uint16)(src[bct + 1U])) |
                  ((uint16)(src[bct     ]) << 8);
 }
}



/* Word -> Byte page conversion - Implementation of RRPGE library function. */
void rrpge_conv_w2b(rrpge_uint16 const* src, rrpge_uint8* dst, rrpge_uint32 bct)
{
 if ((bct & 1U) != 0U){
  bct -= 1U;
  dst[bct] = (uint8)(src[bct >> 1]) >> 8;
 }
 while (bct != 0U){
  bct -= 2U;
  dst[bct + 1U] = (uint8)(src[bct >> 1]);
  dst[bct     ] = (uint8)(src[bct >> 1] >> 8);
 }
}



/* State serialization - Implementation of RRPGE library function */
void rrpge_state2raw(rrpge_state_t const* src, rrpge_uint8* dst)
{
 auint  i;
 uint32 t;

 /* Order is: State (including app. header); CPU Data; PRAM */

 rrpge_conv_w2b(&(src->stat[0]), dst, sizeof(src->stat));
 dst += sizeof(src->stat);
 rrpge_conv_w2b(&(src->dram[0]), dst, sizeof(src->dram));
 dst += sizeof(src->dram);

 for (i = 0; i < (sizeof(src->pram) / sizeof(src->pram[0])); i++){
  t = src->pram[i];
  dst[(i << 2) + 0U] = (uint8)(t >> 24);
  dst[(i << 2) + 1U] = (uint8)(t >> 16);
  dst[(i << 2) + 2U] = (uint8)(t >>  8);
  dst[(i << 2) + 3U] = (uint8)(t      );
 }
}



/* State deserialization - Implementation of RRPGE library function */
void rrpge_raw2state(rrpge_uint8 const* src, rrpge_state_t* dst)
{
 auint i;

 /* Order is: State (including app. header); CPU Data; PRAM */

 rrpge_conv_b2w(src, &(dst->stat[0]), sizeof(dst->stat));
 src += sizeof(dst->stat);
 rrpge_conv_b2w(src, &(dst->dram[0]), sizeof(dst->dram));
 src += sizeof(dst->dram);

 for (i = 0; i < (sizeof(dst->pram) / sizeof(dst->pram[0])); i++){
  dst->pram[i] = ((auint)(src[(i << 2) + 0U]) << 24) |
                 ((auint)(src[(i << 2) + 1U]) << 16) |
                 ((auint)(src[(i << 2) + 2U]) <<  8) |
                 ((auint)(src[(i << 2) + 3U]));
 }
}
