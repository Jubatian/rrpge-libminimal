#
# Author    Sandor Zsuga (Jubatian)
# Copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
#           License) extended as RRPGEvt (temporary version of the RRPGE
#           License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
#           root.
#

OBJECTS+= $(OBD)rgm_aq.o   $(OBD)rgm_aud.o  $(OBD)rgm_cb.o   $(OBD)rgm_chk.o
OBJECTS+= $(OBD)rgm_cpua.o $(OBD)rgm_cpuo.o $(OBD)rgm_db.o   $(OBD)rgm_fifo.o
OBJECTS+= $(OBD)rgm_grln.o $(OBD)rgm_grop.o $(OBD)rgm_info.o $(OBD)rgm_ires.o
OBJECTS+= $(OBD)rgm_krnm.o $(OBD)rgm_main.o $(OBD)rgm_mix.o  $(OBD)rgm_pram.o
OBJECTS+= $(OBD)rgm_prng.o $(OBD)rgm_run.o  $(OBD)rgm_ser.o  $(OBD)rgm_task.o
OBJECTS+= $(OBD)rgm_ulib.o $(OBD)rgm_vid.o

$(OBD)rgm_aq.o: librrpge/rgm_aq.c librrpge/*.h
	$(CC) -c librrpge/rgm_aq.c -o $(OBD)rgm_aq.o $(CFSPD)
	$(CC) -S librrpge/rgm_aq.c -o $(OBD)rgm_aq.asm $(CFSPD)

$(OBD)rgm_aud.o: librrpge/rgm_aud.c librrpge/*.h
	$(CC) -c librrpge/rgm_aud.c -o $(OBD)rgm_aud.o $(CFSPD)
	$(CC) -S librrpge/rgm_aud.c -o $(OBD)rgm_aud.asm $(CFSPD)

$(OBD)rgm_cb.o: librrpge/rgm_cb.c librrpge/*.h
	$(CC) -c librrpge/rgm_cb.c -o $(OBD)rgm_cb.o $(CFSPD)
	$(CC) -S librrpge/rgm_cb.c -o $(OBD)rgm_cb.asm $(CFSPD)

$(OBD)rgm_chk.o: librrpge/rgm_chk.c librrpge/*.h
	$(CC) -c librrpge/rgm_chk.c -o $(OBD)rgm_chk.o $(CFSIZ)
	$(CC) -S librrpge/rgm_chk.c -o $(OBD)rgm_chk.asm $(CFSIZ)

$(OBD)rgm_cpua.o: librrpge/rgm_cpua.c librrpge/*.h
	$(CC) -c librrpge/rgm_cpua.c -o $(OBD)rgm_cpua.o -fomit-frame-pointer $(CFSPD)
	$(CC) -S librrpge/rgm_cpua.c -o $(OBD)rgm_cpua.asm -fomit-frame-pointer $(CFSPD)

$(OBD)rgm_cpuo.o: librrpge/rgm_cpuo.c librrpge/*.h
	$(CC) -c librrpge/rgm_cpuo.c -o $(OBD)rgm_cpuo.o -fomit-frame-pointer $(CFSPD)
	$(CC) -S librrpge/rgm_cpuo.c -o $(OBD)rgm_cpuo.asm -fomit-frame-pointer $(CFSPD)

$(OBD)rgm_db.o: librrpge/rgm_db.c librrpge/*.h
	$(CC) -c librrpge/rgm_db.c -o $(OBD)rgm_db.o $(CFSPD)
	$(CC) -S librrpge/rgm_db.c -o $(OBD)rgm_db.asm $(CFSPD)

$(OBD)rgm_fifo.o: librrpge/rgm_fifo.c librrpge/*.h
	$(CC) -c librrpge/rgm_fifo.c -o $(OBD)rgm_fifo.o $(CFSPD)
	$(CC) -S librrpge/rgm_fifo.c -o $(OBD)rgm_fifo.asm $(CFSPD)

$(OBD)rgm_grln.o: librrpge/rgm_grln.c librrpge/*.h
	$(CC) -c librrpge/rgm_grln.c -o $(OBD)rgm_grln.o $(CFSPD)
	$(CC) -S librrpge/rgm_grln.c -o $(OBD)rgm_grln.asm $(CFSPD)

$(OBD)rgm_grop.o: librrpge/rgm_grop.c librrpge/*.h
	$(CC) -c librrpge/rgm_grop.c -o $(OBD)rgm_grop.o $(CFSPD)
	$(CC) -S librrpge/rgm_grop.c -o $(OBD)rgm_grop.asm $(CFSPD)

$(OBD)rgm_info.o: librrpge/rgm_info.c librrpge/*.h
	$(CC) -c librrpge/rgm_info.c -o $(OBD)rgm_info.o $(CFSPD)
	$(CC) -S librrpge/rgm_info.c -o $(OBD)rgm_info.asm $(CFSPD)

$(OBD)rgm_ires.o: librrpge/rgm_ires.c librrpge/*.h
	$(CC) -c librrpge/rgm_ires.c -o $(OBD)rgm_ires.o $(CFSIZ)
	$(CC) -S librrpge/rgm_ires.c -o $(OBD)rgm_ires.asm $(CFSIZ)

$(OBD)rgm_krnm.o: librrpge/rgm_krnm.c librrpge/*.h
	$(CC) -c librrpge/rgm_krnm.c -o $(OBD)rgm_krnm.o $(CFSPD)
	$(CC) -S librrpge/rgm_krnm.c -o $(OBD)rgm_krnm.asm $(CFSPD)

$(OBD)rgm_main.o: librrpge/rgm_main.c librrpge/*.h
	$(CC) -c librrpge/rgm_main.c -o $(OBD)rgm_main.o $(CFSPD)
	$(CC) -S librrpge/rgm_main.c -o $(OBD)rgm_main.asm $(CFSPD)

$(OBD)rgm_mix.o: librrpge/rgm_mix.c librrpge/*.h
	$(CC) -c librrpge/rgm_mix.c -o $(OBD)rgm_mix.o $(CFSPD)
	$(CC) -S librrpge/rgm_mix.c -o $(OBD)rgm_mix.asm $(CFSPD)

$(OBD)rgm_pram.o: librrpge/rgm_pram.c librrpge/*.h
	$(CC) -c librrpge/rgm_pram.c -o $(OBD)rgm_pram.o $(CFSPD)
	$(CC) -S librrpge/rgm_pram.c -o $(OBD)rgm_pram.asm $(CFSPD)

$(OBD)rgm_prng.o: librrpge/rgm_prng.c librrpge/*.h
	$(CC) -c librrpge/rgm_prng.c -o $(OBD)rgm_prng.o $(CFSPD)
	$(CC) -S librrpge/rgm_prng.c -o $(OBD)rgm_prng.asm $(CFSPD)

$(OBD)rgm_run.o: librrpge/rgm_run.c librrpge/*.h
	$(CC) -c librrpge/rgm_run.c -o $(OBD)rgm_run.o $(CFSPD)
	$(CC) -S librrpge/rgm_run.c -o $(OBD)rgm_run.asm $(CFSPD)

$(OBD)rgm_ser.o: librrpge/rgm_ser.c librrpge/*.h
	$(CC) -c librrpge/rgm_ser.c -o $(OBD)rgm_ser.o $(CFSPD)
	$(CC) -S librrpge/rgm_ser.c -o $(OBD)rgm_ser.asm $(CFSPD)

$(OBD)rgm_task.o: librrpge/rgm_task.c librrpge/*.h
	$(CC) -c librrpge/rgm_task.c -o $(OBD)rgm_task.o $(CFSPD)
	$(CC) -S librrpge/rgm_task.c -o $(OBD)rgm_task.asm $(CFSPD)

$(OBD)rgm_ulib.o: librrpge/rgm_ulib.c librrpge/*.h
	$(CC) -c librrpge/rgm_ulib.c -o $(OBD)rgm_ulib.o $(CFSIZ)
	$(CC) -S librrpge/rgm_ulib.c -o $(OBD)rgm_ulib.asm $(CFSIZ)

$(OBD)rgm_vid.o: librrpge/rgm_vid.c librrpge/*.h
	$(CC) -c librrpge/rgm_vid.c -o $(OBD)rgm_vid.o $(CFSPD)
	$(CC) -S librrpge/rgm_vid.c -o $(OBD)rgm_vid.asm $(CFSPD)
