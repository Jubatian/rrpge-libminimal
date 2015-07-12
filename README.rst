
RRPGE Minimal library & Host prototype
==============================================================================

.. image:: https://cdn.rawgit.com/Jubatian/rrpge-spec/00.013.002/logo_txt.svg
   :align: center
   :width: 100%

:Author:    Sandor Zsuga (Jubatian)
:License:   2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
            License) extended as RRPGEvt (temporary version of the RRPGE
            License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
            root.




Introduction
------------------------------------------------------------------------------


This is a prototype RRPGE library crafted together with a simple host. The
RRPGE Minimal library will be derived from this work, while the host side may
later contribute for a simple SDL based host.

The library itself is fast, however the host currently does not support
frameskipping (this feature is also untested in the library) and operates at
32 bit depth, so crippling performance on older systems.

The basic features: graphics and audio should work reasonably well and
according to the RRPGE specification meeting the minimal timing requirements.
The program is largely untested though, and contains several locations of
prototype patches.


Related projects
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- RRPGE home: http://www.rrpge.org
- RRPGE Specification: https://www.github.com/Jubatian/rrpge-spec
- RRPGE Assembler: https://www.github.com/Jubatian/rrpge-asm
- RRPGE User Library: https://www.github.com/Jubatian/rrpge-userlib
- Example programs: https://www.github.com/Jubatian/rrpge-examples


Temporary license notes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Currently the project is developed under a temporary GPL compatible license.
The intention for later is to add some permissive exceptions to this license,
allowing for creating derivative works (most importantly, applications) under
other licenses than GPL.

For more information, see http://www.rrpge.org/community/index.php?topic=30.0




Invocation
------------------------------------------------------------------------------


The program requires one parameter: an RRPGE application binary (an ".rpa"
file). It runs the passed application until it terminates or produces an error
(points where the RRPGE specification requires the termination of the
application).
