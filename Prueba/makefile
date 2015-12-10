#####################################
# Fichero: makefile
# Autores: 
# Héctor Gonzalo Andrés DNI 71038384D
# Néstor 
#####################################

all: cliente servidor

cliente: cliente.o
	cc -o cliente cliente.o
servidor: servidor.o
	cc -o servidor servidor.o

cliente.o: cliente.c mylib.h
	cc -c cliente.c
servidor.o: servidor.c mylib.h
	cc -c servidor.c