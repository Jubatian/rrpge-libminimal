############
# Makefile #
############
#
# Author    Sandor Zsuga (Jubatian)
# Copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
#           License) extended as RRPGEvt (temporary version of the RRPGE
#           License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
#           root.
#
#
#
#
# The main makefile of the program
#
#
# make all (or make): build the program
# make clean:         to clean up
#
#

include Make_defines.mk

CFLAGS+=

OBJECTS=$(OBD)main.o

all: $(OUT)
clean:
	$(SHRM) $(OBJECTS) $(OUT)
	$(SHRM) $(OBB)


# modules
include librrpge/make.mk
include host/make.mk
include iface/make.mk


$(OUT): $(OBB) $(OBJECTS)
	$(CC) -o $(OUT) $(OBJECTS) $(CFSIZ) $(LINK)

$(OBB):
	$(SHMKDIR) $(OBB)

$(OBD)main.o: main.c librrpge/rrpge*.h iface/*.h host/*.h
	$(CC) -c main.c -o $(OBD)main.o $(CFSIZ)

.PHONY: all clean
