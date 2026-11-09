CC = gcc
CFLAGS = -Wall -Wextra
OBJ = main.c simulador.c validaciones.c

simulador: $(OBJ) simulador.h validaciones.h
	$(CC) $(CFLAGS) $(OBJ) -o simulador -lm
