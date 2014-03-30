#
# Author    Sandor Zsuga (Jubatian)
# Copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
#           License) extended as RRPGEv1 (version 1 of the RRPGE License): see
#           LICENSE.GPLv3 and LICENSE.RRPGEv1 in the project root.
#

OBJECTS+= $(OBD)screen.o   $(OBD)audio.o    $(OBD)filels.o

$(OBD)screen.o: host/screen.c host/screen.h
	$(CC) -c host/screen.c -o $(OBD)screen.o $(CFSPD)

$(OBD)audio.o: host/audio.c host/audio.h
	$(CC) -c host/audio.c -o $(OBD)audio.o $(CFSPD)

$(OBD)filels.o: host/filels.c host/filels.h
	$(CC) -c host/filels.c -o $(OBD)filels.o $(CFSPD)
