/**
**  \file
**  \brief     Pseudorandom number generator
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#include "rgm_prng.h"


#define NUM1 0x778BU
#define NUM2 0x4A8BU


/* Random number generator's state */
static auint rrpge_m_prng_s = 0U;


/* Returns a 16bit pseudorandom number */
auint rrpge_m_prng(void)
{
 rrpge_m_prng_s = ( ( ((rrpge_m_prng_s >> 15) & 1U) +
                      rrpge_m_prng_s + rrpge_m_prng_s + NUM1
                    ) ^ NUM2) & 0xFFFFU;
 return rrpge_m_prng_s;
}
