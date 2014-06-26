#
# Author    Sandor Zsuga (Jubatian)
# Copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
#           License) extended as RRPGEv2 (version 2 of the RRPGE License): see
#           LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
#

OBJECTS+= $(OBD)rgm_aq.o   $(OBD)rgm_aud.o  $(OBD)rgm_cb.o   $(OBD)rgm_chk.o
OBJECTS+= $(OBD)rgm_cpua.o $(OBD)rgm_cpuo.o $(OBD)rgm_dma.o  $(OBD)rgm_grcy.o
OBJECTS+= $(OBD)rgm_grln.o $(OBD)rgm_grop.o $(OBD)rgm_info.o $(OBD)rgm_ires.o
OBJECTS+= $(OBD)rgm_krnm.o $(OBD)rgm_main.o $(OBD)rgm_mix.o  $(OBD)rgm_prng.o
OBJECTS+= $(OBD)rgm_run.o  $(OBD)rgm_ser.o  $(OBD)rgm_task.o $(OBD)rgm_vid.o

$(OBD)rgm_aq.o: librrpge/rgm_aq.c librrpge/rgm_aq.h
	$(CC) -c librrpge/rgm_aq.c -o $(OBD)rgm_aq.o $(CFSPD)
	$(CC) -S librrpge/rgm_aq.c -o $(OBD)rgm_aq.asm $(CFSPD)

$(OBD)rgm_aud.o: librrpge/rgm_aud.c librrpge/rgm_aud.h
	$(CC) -c librrpge/rgm_aud.c -o $(OBD)rgm_aud.o $(CFSPD)
	$(CC) -S librrpge/rgm_aud.c -o $(OBD)rgm_aud.asm $(CFSPD)

$(OBD)rgm_cb.o: librrpge/rgm_cb.c librrpge/rgm_cb.h
	$(CC) -c librrpge/rgm_cb.c -o $(OBD)rgm_cb.o $(CFSPD)
	$(CC) -S librrpge/rgm_cb.c -o $(OBD)rgm_cb.asm $(CFSPD)

$(OBD)rgm_chk.o: librrpge/rgm_chk.c librrpge/rgm_chk.h
	$(CC) -c librrpge/rgm_chk.c -o $(OBD)rgm_chk.o $(CFSPD)
	$(CC) -S librrpge/rgm_chk.c -o $(OBD)rgm_chk.asm $(CFSPD)

$(OBD)rgm_cpua.o: librrpge/rgm_cpua.c librrpge/rgm_cpua.h
	$(CC) -c librrpge/rgm_cpua.c -o $(OBD)rgm_cpua.o -fomit-frame-pointer $(CFSPD)
	$(CC) -S librrpge/rgm_cpua.c -o $(OBD)rgm_cpua.asm -fomit-frame-pointer $(CFSPD)

$(OBD)rgm_cpuo.o: librrpge/rgm_cpuo.c librrpge/rgm_cpuo.h
	$(CC) -c librrpge/rgm_cpuo.c -o $(OBD)rgm_cpuo.o -fomit-frame-pointer $(CFSPD)
	$(CC) -S librrpge/rgm_cpuo.c -o $(OBD)rgm_cpuo.asm -fomit-frame-pointer $(CFSPD)

$(OBD)rgm_dma.o: librrpge/rgm_dma.c librrpge/rgm_dma.h
	$(CC) -c librrpge/rgm_dma.c -o $(OBD)rgm_dma.o $(CFSPD)
	$(CC) -S librrpge/rgm_dma.c -o $(OBD)rgm_dma.asm $(CFSPD)

$(OBD)rgm_grcy.o: librrpge/rgm_grcy.c librrpge/rgm_grcy.h
	$(CC) -c librrpge/rgm_grcy.c -o $(OBD)rgm_grcy.o $(CFSPD)
	$(CC) -S librrpge/rgm_grcy.c -o $(OBD)rgm_grcy.asm $(CFSPD)

$(OBD)rgm_grln.o: librrpge/rgm_grln.c librrpge/rgm_grln.h
	$(CC) -c librrpge/rgm_grln.c -o $(OBD)rgm_grln.o $(CFSPD)
	$(CC) -S librrpge/rgm_grln.c -o $(OBD)rgm_grln.asm $(CFSPD)

$(OBD)rgm_grop.o: librrpge/rgm_grop.c librrpge/rgm_grop.h
	$(CC) -c librrpge/rgm_grop.c -o $(OBD)rgm_grop.o $(CFSPD)
	$(CC) -S librrpge/rgm_grop.c -o $(OBD)rgm_grop.asm $(CFSPD)

$(OBD)rgm_info.o: librrpge/rgm_info.c librrpge/rgm_info.h
	$(CC) -c librrpge/rgm_info.c -o $(OBD)rgm_info.o $(CFSPD)
	$(CC) -S librrpge/rgm_info.c -o $(OBD)rgm_info.asm $(CFSPD)

$(OBD)rgm_ires.o: librrpge/rgm_ires.c librrpge/rgm_ires.h
	$(CC) -c librrpge/rgm_ires.c -o $(OBD)rgm_ires.o $(CFSPD)
	$(CC) -S librrpge/rgm_ires.c -o $(OBD)rgm_ires.asm $(CFSPD)

$(OBD)rgm_krnm.o: librrpge/rgm_krnm.c librrpge/rgm_krnm.h
	$(CC) -c librrpge/rgm_krnm.c -o $(OBD)rgm_krnm.o $(CFSPD)
	$(CC) -S librrpge/rgm_krnm.c -o $(OBD)rgm_krnm.asm $(CFSPD)

$(OBD)rgm_main.o: librrpge/rgm_main.c librrpge/rgm_main.h
	$(CC) -c librrpge/rgm_main.c -o $(OBD)rgm_main.o $(CFSPD)
	$(CC) -S librrpge/rgm_main.c -o $(OBD)rgm_main.asm $(CFSPD)

$(OBD)rgm_mix.o: librrpge/rgm_mix.c librrpge/rgm_mix.h
	$(CC) -c librrpge/rgm_mix.c -o $(OBD)rgm_mix.o $(CFSPD)
	$(CC) -S librrpge/rgm_mix.c -o $(OBD)rgm_mix.asm $(CFSPD)

$(OBD)rgm_prng.o: librrpge/rgm_prng.c librrpge/rgm_prng.h
	$(CC) -c librrpge/rgm_prng.c -o $(OBD)rgm_prng.o $(CFSPD)
	$(CC) -S librrpge/rgm_prng.c -o $(OBD)rgm_prng.asm $(CFSPD)

$(OBD)rgm_run.o: librrpge/rgm_run.c librrpge/rgm_run.h
	$(CC) -c librrpge/rgm_run.c -o $(OBD)rgm_run.o $(CFSPD)
	$(CC) -S librrpge/rgm_run.c -o $(OBD)rgm_run.asm $(CFSPD)

$(OBD)rgm_ser.o: librrpge/rgm_ser.c librrpge/rgm_ser.h
	$(CC) -c librrpge/rgm_ser.c -o $(OBD)rgm_ser.o $(CFSPD)
	$(CC) -S librrpge/rgm_ser.c -o $(OBD)rgm_ser.asm $(CFSPD)

$(OBD)rgm_task.o: librrpge/rgm_task.c librrpge/rgm_task.h
	$(CC) -c librrpge/rgm_task.c -o $(OBD)rgm_task.o $(CFSPD)
	$(CC) -S librrpge/rgm_task.c -o $(OBD)rgm_task.asm $(CFSPD)

$(OBD)rgm_vid.o: librrpge/rgm_vid.c librrpge/rgm_vid.h
	$(CC) -c librrpge/rgm_vid.c -o $(OBD)rgm_vid.o $(CFSPD)
	$(CC) -S librrpge/rgm_vid.c -o $(OBD)rgm_vid.asm $(CFSPD)
