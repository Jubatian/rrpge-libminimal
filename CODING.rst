
RRPGE Minimal library & Host prototype, Coding style and conventions
==============================================================================

:Author:    Sandor Zsuga (Jubatian)
:Copyright: 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
            License) extended as RRPGEvt (temporary version of the RRPGE
            License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
            root.




Introduction
------------------------------------------------------------------------------


This file describes some cruical coding style elements and conventions
utilized for the development of this library, primarily those which may not be
apparent from the code or it's comments alone.




Freestanding
------------------------------------------------------------------------------


The code in "librrpge" consists the freestanding RRPGE emulator library. This
is supposed to be compile-able with any compiler capable to process code
following the C89 standard.

Standard C library functions must also be avoided, so making the library
capable to support environments where the standard C library does not exists.
Only the type definitions from <stdint.h> may be used to define own types, so
in a C89 environment they can be substituted.




RRPGE library interface
------------------------------------------------------------------------------


The headers in "librrpge" with an "rrpge_" prefix are the interface of the
RRPGE library. This component is still in development. The RRPGE library is
meant to be accessed only through these, later, this part should be
standardised to provide a common interface for probable different
implementations.




Types
------------------------------------------------------------------------------


Compiler "int" width
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The compiler's "int" width is assumed to be at least 32 bits. This most
importantly assumes the followings to be true:

- Integer promotion promotes towards an at least 32 bit wide type.
- Constants are at least 32 bit wide.

There is no point in supporting narrower "int" widths as it is unlikely
feasible emulation is possible on such architectures.


The "auint" type
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The "auint" type must be declared to be at least as wide as the compiler's
native "int" width, ensuring no promotions (into signed integer) happen on a
variable of type "auint". The type may be wider than the compiler's native
"int" width.


Fixed width types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Fixed width types (8, 16, 32 bits) should only be used for working with arrays
in memory.

The types must be assumed to hold at least the indicated amount of bits. They
may hold more.

When moving values into a fixed with type, if necessary, an appropriate mask
should ensure the value is not out of range for the type. Normally compilers
supporting the given type proper may discard this mask, so it has no effect on
performance or compiled binary size.

When reading a fixed with type, if necessary, the range should be ensured by
an appropriate mask. Normally compilers should be capable to discard this mask
if the type has the appropriate fixed width.

These rules ensure that even when a suitable fixed width type is not
available, the emulator may still operate properly using wider types.
