/*
 * validaciones.h
 * Funciones para leer datos del teclado de forma segura.
 * Grupo D14 - Comision 1.4 - Algoritmos y Estructuras de Datos - UTN FRRe
 */

#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#define MAX_LINEA 100   /* largo maximo de una linea ingresada */

int leer_linea(char linea[], int tam);
int leer_entero(char mensaje[], int min, int max);
double leer_real(char mensaje[], double min, double max);
void pausar(void);

#endif
