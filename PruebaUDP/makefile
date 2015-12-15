#####################################
# Fichero: makefile
# Autores: 
# Héctor Gonzalo Andrés DNI 71038384D
# Néstor 
#####################################

all: cliente servidor

cliente: client.o
	cc -o cliente client.o
servidor: server.o
	cc -o servidor server.o

cliente.o: client.c mylib.h
	cc -c client.c
servidor.o: server.c mylib.h
	cc -c server.c