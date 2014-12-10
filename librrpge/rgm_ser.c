/**
**  \file
**  \brief     Serialization functions
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2014.12.10
*/


#include "rgm_ser.h"


/* Byte -> Word conversion - Implementation of RRPGE library function. */
void rrpge_conv_b2w(rrpge_uint8 const* src, rrpge_uint16* dst, rrpge_iuint bct)
{
 if ((bct & 1U) != 0U){
  bct -= 1U;
  dst[bct >> 1] = (src[bct] & 0xFFU) << 8;
 }
 while (bct != 0U){
  bct -= 2U;
  dst[bct >> 1] = ((src[bct + 1U] & 0xFFU)) |
                  ((src[bct     ] & 0xFFU) << 8);
 }
}



/* Word -> Byte conversion - Implementation of RRPGE library function. */
void rrpge_conv_w2b(rrpge_uint16 const* src, rrpge_uint8* dst, rrpge_iuint bct)
{
 if ((bct & 1U) != 0U){
  bct -= 1U;
  dst[bct] = ((src[bct >> 1]) >> 8) & 0xFFU;
 }
 while (bct != 0U){
  bct -= 2U;
  dst[bct + 1U] = (src[bct >> 1])      & 0xFFU;
  dst[bct     ] = (src[bct >> 1] >> 8) & 0xFFU;
 }
}



/* State serialization - Implementation of RRPGE library function */
void rrpge_state2raw(rrpge_state_t const* src, rrpge_uint8* dst)
{
 auint i;
 auint t;

 /* Order is: State (including app. header); CPU Data; PRAM */

 rrpge_conv_w2b(&(src->stat[0]), dst, sizeof(src->stat));
 dst += sizeof(src->stat);
 rrpge_conv_w2b(&(src->dram[0]), dst, sizeof(src->dram));
 dst += sizeof(src->dram);

 for (i = 0; i < (sizeof(src->pram) / sizeof(src->pram[0])); i++){
  t = src->pram[i];
  dst[(i << 2) + 0U] = (t >> 24) & 0xFFU;
  dst[(i << 2) + 1U] = (t >> 16) & 0xFFU;
  dst[(i << 2) + 2U] = (t >>  8) & 0xFFU;
  dst[(i << 2) + 3U] = (t      ) & 0xFFU;
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
  dst->pram[i] = ((src[(i << 2) + 0U] & 0xFFU) << 24) |
                 ((src[(i << 2) + 1U] & 0xFFU) << 16) |
                 ((src[(i << 2) + 2U] & 0xFFU) <<  8) |
                 ((src[(i << 2) + 3U] & 0xFFU));
 }
}
