#!/bin/bash
# MakeFile de TP1 de Sistemas Operativos
.SILENT:

SRC=src
BIN=bin

C_OBJ= $(SRC)/kernel.o $(SRC)/libc.o $(SRC)/stdio.o $(SRC)/shell.o $(SRC)/pcihdr.o $(SRC)/video.o $(SRC)/exceptions.o $(SRC)/paging.o $(SRC)/kheap.o $(SRC)/process.o $(SRC)/ordered_array.o $(SRC)/time.o $(SRC)/console.o


C_OBJ_SIN_P = $(C_OBJ) $(SRC)/scheduler.o

C_OBJ_CON_P = $(C_OBJ) $(SRC)/scheduler_prioridad.o

EJEC=kernel.bin

CFLAGS= -fno-builtin -fno-stack-protector
LDOPTS = -o
COPTS=-c

all: sinprioridad
	
sinprioridad: $(C_OBJ_SIN_P) 
	
	@-nasm -f aout $(SRC)/libasm.asm -o libasm.o
	@-echo "1. Nasm OK!"
	@-nasm -f aout $(SRC)/kstart.asm -o kstart.o
	@-echo "2. Nasm OK!"
	@-nasm -f aout $(SRC)/loader.asm -o kstart.o
	@-echo "3. Nasm OK!"	
	@-mv *.o $(SRC)

	@-ld -T $(SRC)/link.ld $(LDOPTS) $(EJEC) $(SRC)/kstart.o $(SRC)/libasm.o $(C_OBJ_SIN_P)
	@-echo "SIN PRIORIDAD ..."

	mv $(SRC)/*.o $(BIN)
	mv $(EJEC) $(BIN)

	mcopy $(BIN)/kernel.bin b:boot/ -o
	bochs -q
	
conprioridad: $(C_OBJ_CON_P)

	@-nasm -f aout $(SRC)/libasm.asm -o libasm.o
	@-echo "1. Nasm OK!"
	@-nasm -f aout $(SRC)/kstart.asm -o kstart.o
	@-echo "2. Nasm OK!"
	@-nasm -f aout $(SRC)/loader.asm -o kstart.o
	@-echo "3. Nasm OK!"	
	@-mv *.o $(SRC)

	@-ld -T $(SRC)/link.ld $(LDOPTS) $(EJEC) $(SRC)/kstart.o $(SRC)/libasm.o $(C_OBJ_CON_P)

	mv $(SRC)/*.o $(BIN)
	mv $(EJEC) $(BIN)

	mcopy $(BIN)/kernel.bin b:boot/ -o
	bochs -q


.c.o:
	echo compilando $<
	@-gcc $(COPTS) $< $(CFLAGS)
	mv *.o $(SRC)

clear:
	echo
	echo Limpiando directorio
	echo

	#Borra objetos
	rm -f $(SRC)/*.o 
	rm -f $(BIN)/*.o 

	#Borra archivos temporales
	rm -f $(SRC)/*~ 
	
again: 
	make clear
	make all
	
