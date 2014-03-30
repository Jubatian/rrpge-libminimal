#
# Author    Sandor Zsuga (Jubatian)
# Copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
#           License) extended as RRPGEv1 (version 1 of the RRPGE License): see
#           LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
#

OBJECTS+= $(OBD)render.o

$(OBD)render.o: iface/render.c iface/render.h
	$(CC) -c iface/render.c -o $(OBD)render.o $(CFSPD)
