/**
**  \file
**  \brief     LibRRPGE standard header package - library parameters
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv1 (version 1 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
**  \date      2014.03.23
*/


#ifndef RRPGE_PR_H
#define RRPGE_PR_H


#include "rrpge_tp.h"



/**
**  \brief     Library description
**
**  Conatins the static elements of the library's description. Note that there
**  might be additional static elements in a library which may be provided as
**  parameters constrained to a single value (consult the RRPGE library
**  specification for those elements).
*/
typedef struct{
 rrpge_uint8 const*
             const* lics;  /**< List of supported application licenses,
                                terminated with an empty string. The list may
                                be empty (containing a single empty string)
                                indicating that the library does not constrain
                                by license. The strings are case-sensitive. */
 rrpge_uint8 const* lnam;  /**< Library name, may be used by hosts in a
                                library selection feature. */
 rrpge_uint8 const* laut;  /**< Library author */
 rrpge_uint8 const* llic;  /**< Library license information */
 rrpge_uint8 const* lwww;  /**< Library home (url) */
 rrpge_uint32       ssiz;  /**< Emulator instance size in bytes (uint8) */
 rrpge_uint32       reen;  /**< Nonzero if library supports reentrancy (is
                                thread safe), zero otherwise. If this is zero
                                the library should be used from a single
                                thread only (it is still capable to
                                simultaneously emulate multiple systems
                                though). */
 rrpge_uint16       spat;  /**< Supported RRPGE specification patch version */
 rrpge_uint16       lpat;  /**< Library patch version */
 rrpge_uint8        smaj;  /**< Supported RRPGE specification major version */
 rrpge_uint8        lmaj;  /**< Library major version */
 rrpge_uint8        smin;  /**< Supported RRPGE specification minor version */
 rrpge_uint8        lmin;  /**< Library minor version */
}rrpge_libdesc_t;



/**
**  \ref       rrpge_libpar_t_flag_disp
**  \name      Display format suggestions for rrpge_libpar_t
**
**  These may be used with the flags member of the rrpge_libpar_t type to
**  identify display format suggestions.
**
**  \{ */
/** Base 10 decimal */
#define RRPGE_PDISP_DECIMAL   (0x0U << 6)
/** Base 2 binary */
#define RRPGE_PDISP_BINARY    (0x1U << 6)
/** Base 16 hexadecimal */
#define RRPGE_PDISP_HEXA      (0x2U << 6)
/** Base 2 binary as flag set */
#define RRPGE_PDISP_FLAGS     (0x3U << 6)
/** \} */



/**
**  \ref       rrpge_libpar_t_flag_cons
**  \name      Constraint modes for rrpge_libpar_t
**
**  These may be used with the flags member of the rrpge_libpar_t type to
**  identify constraint modes.
**
**  \{ */
/** No constraints (all values are permitted) */
#define RRPGE_PCONS_NONE      (0x0U << 8)
/** Min - Max limit constraint using the min and max members */
#define RRPGE_PCONS_MINMAX    (0x1U << 8)
/** Allowed value list constraint using the max member for count and vlist
**  for the list of allowed values. */
#define RRPGE_PCONS_VLIST     (0x2U << 8)
/** Mask constraint using the max member for mask. Set bits in the mask are
**  allowed to be altered. */
#define RRPGE_PCONS_MASK      (0x3U << 8)
/** \} */



/**
**  \ref       rrpge_libpar_t_flags
**  \name      Masks for the flags member of rrpge_libpar_t
**
**  These may be used with the flags member of the rrpge_libpar_t type to
**  identify the individual flags.
**
**  \{ */
/** Number of fractional bits (so parameter may be interpreted as a fixed
**  point real) */
#define RRPGE_PMASK_FRAC      0x001FU
/** 2 complement signed representation if set. The highest bit (bit 31) of the
**  parameter will indicate the negativeness of the parameter value */
#define RRPGE_PMASK_SIGNED    0x0020U
/** Display format suggestion */
#define RRPGE_PMASK_DISP      0x00C0U
/** Constraint mode defining the limits of accepted values for the parameter
**  value */
#define RRPGE_PMASK_CONS      0x0300U
/** \} */



/**
**  \brief     Library parameter definition
**
**  Defines a library parameter and provides it's current value. Parameters
**  can only be 32bit unsigned integers by type, but semantically they may
**  have different meanings depending on the description of the parameter.
*/
typedef struct{
 rrpge_uint8  const* name;  /**< String identification of the parameter */
 rrpge_uint32        val;   /**< Parameter's current value */
 rrpge_uint32        flags; /**< Parameter value's descriptor flags */
 rrpge_uint8  const* unit;  /**< String postfix (unit type) of parameter. This
                                 might be empty if value is dimensionless. */
 rrpge_uint32        max;   /**< Maximal allowed value if limit was set */
 rrpge_uint32        min;   /**< Minimal allowed value if limit was set */
 rrpge_uint32 const* vlist; /**< Allowed value list */
 rrpge_uint8  const*
              const* fsetn; /**< 32 strings specifying flag names if flag set
                                 representation was suggested. Note that
                                 although the range of valid flags may be
                                 limited, this string set is always complete
                                 (if flag representation was suggested). */
}rrpge_libpar_t;





/**
**  \brief     Requests library description
**
**  Provides a structure containing the library's static description. For more
**  information check the documentation of rrpge_libdesc_t.
*/
rrpge_libdesc_t const* rrpge_getdescription(void);


/**
**  \brief     Requests library parameter
**
**  Returns the parameter identified by the passed index. The parameter
**  description contains the name, value and constraints of the parameter, see
**  rrpge_libpar_t for details. The return may be NULL, but only if the index
**  is out of range.
**
**  \param[in]   idx   Index of parameter to request.
**  \return            Parameter description and value.
*/
rrpge_libpar_t const* rrpge_getpar(rrpge_uint32 idx);


/**
**  \brief     Sets library parameter
**
**  Sets the parameter at the given index. The parameter is checked and
**  constrained by the library if necessary. Returns the value actually set
**  for the parameter. If constraining is necessary or the index is out of
**  range, the return is implementation defined, but if constraining it is
**  always one of the valid values for the library.
**
**  \param[in]   idx   Index of parameter to set.
**  \param[in]   val   New value for the parameter.
**  \return            Actual value set.
*/
rrpge_uint32 rrpge_setpar(rrpge_uint32 idx, rrpge_uint32 val);



#endif
