#
# Author    Sandor Zsuga (Jubatian)
# Copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
#           License) extended as RRPGEvt (temporary version of the RRPGE
#           License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
#           root.
#

OBJECTS+= $(OBD)screen.o   $(OBD)audio.o    $(OBD)filels.o

$(OBD)screen.o: host/screen.c host/screen.h
	$(CC) -c host/screen.c -o $(OBD)screen.o $(CFSIZ)

$(OBD)audio.o: host/audio.c host/audio.h
	$(CC) -c host/audio.c -o $(OBD)audio.o $(CFSIZ)

$(OBD)filels.o: host/filels.c host/filels.h
	$(CC) -c host/filels.c -o $(OBD)filels.o $(CFSIZ)
