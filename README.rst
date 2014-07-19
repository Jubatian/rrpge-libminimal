
RRPGE Minimal library & Host prototype
==============================================================================

:Author:    Sandor Zsuga (Jubatian)
:Copyright: 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
            License) extended as RRPGEv2 (version 2 of the RRPGE License): see
            LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.




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




Invocation
------------------------------------------------------------------------------


The program requires one parameter: an RRPGE application binary (an ".rpa"
file). It runs the passed application until it terminates or produces an error
(points where the RRPGE specification requires the termination of the
application).