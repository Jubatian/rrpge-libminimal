#
# Author    Sandor Zsuga (Jubatian)
# Copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
#           License) extended as RRPGEvt (temporary version of the RRPGE
#           License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
#           root.
#

OBJECTS+= $(OBD)render.o   $(OBD)inputcom.o $(OBD)input.o    $(OBD)imouse.o

$(OBD)render.o: iface/render.c iface/*.h host/*.h librrpge/rrpge*.h
	$(CC) -c iface/render.c -o $(OBD)render.o $(CFSPD)

$(OBD)inputcom.o: iface/inputcom.c iface/*.h host/*.h librrpge/rrpge*.h
	$(CC) -c iface/inputcom.c -o $(OBD)inputcom.o $(CFSIZ)

$(OBD)input.o: iface/input.c iface/*.h host/*.h librrpge/rrpge*.h
	$(CC) -c iface/input.c -o $(OBD)input.o $(CFSIZ)

$(OBD)imouse.o: iface/imouse.c iface/*.h host/*.h librrpge/rrpge*.h
	$(CC) -c iface/imouse.c -o $(OBD)imouse.o $(CFSIZ)
