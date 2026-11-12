/*
 * validaciones.c
 * Lectura segura de datos.
 *
 * Idea: en vez de usar scanf directamente (que deja basura en el buffer si el
 * usuario escribe letras y puede generar bucles infinitos), leemos SIEMPRE la
 * linea completa con fgets y despues sacamos el numero con sscanf.
 * Asi controlamos: letras, entradas vacias, caracteres de mas (ej: "12abc"),
 * numeros demasiado grandes y valores fuera de rango.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validaciones.h"

/*
 * Lee una linea del teclado y le saca el '\n' del final.
 * Devuelve 1 si la linea entro completa y 0 si era demasiado larga.
 */
int leer_linea(char linea[], int tam)
{
    int i = 0;
    int c;

    if (fgets(linea, tam, stdin) == NULL) {
        /* se termino la entrada (por ejemplo Ctrl+Z): salimos sin quedar en un bucle */
        printf("\nNo hay mas datos de entrada. Fin del programa.\n");
        exit(0);
    }

    while (linea[i] != '\0' && linea[i] != '\n') {
        i++;
    }

    if (linea[i] == '\n') {
        linea[i] = '\0';
        return 1;
    }

    /* no llego el '\n': lo que sobra quedo en el buffer y hay que descartarlo */
    c = getchar();
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
    return 0;
}

/*
 * Pide un entero hasta que el usuario ingrese uno valido entre min y max.
 */
int leer_entero(char mensaje[], int min, int max)
{
    char linea[MAX_LINEA];
    char sobra;
    int valor = 0;
    int leidos;
    int valido = 0;

    do {
        printf("%s", mensaje);

        if (leer_linea(linea, MAX_LINEA) == 0) {
            printf("  Error: la entrada es demasiado larga.\n");
        } else {
            leidos = sscanf(linea, "%d %c", &valor, &sobra);

            if (leidos == EOF) {
                printf("  Error: no ingresaste nada.\n");
            } else if (leidos == 0) {
                printf("  Error: tenes que ingresar un numero entero.\n");
            } else if (leidos == 2) {
                printf("  Error: hay caracteres de mas despues del numero.\n");
            } else if (strlen(linea) > 9) {
                printf("  Error: el numero es demasiado grande.\n");
            } else if (valor < min || valor > max) {
                printf("  Error: el valor debe estar entre %d y %d.\n", min, max);
            } else {
                valido = 1;
            }
        }
    } while (valido == 0);

    return valor;
}

/*
 * Pide un numero real hasta que el usuario ingrese uno valido entre min y max.
 */
double leer_real(char mensaje[], double min, double max)
{
    char linea[MAX_LINEA];
    char sobra;
    double valor = 0;
    int leidos;
    int valido = 0;

    do {
        printf("%s", mensaje);

        if (leer_linea(linea, MAX_LINEA) == 0) {
            printf("  Error: la entrada es demasiado larga.\n");
        } else {
            leidos = sscanf(linea, "%lf %c", &valor, &sobra);

            if (leidos == EOF) {
                printf("  Error: no ingresaste nada.\n");
            } else if (leidos == 0) {
                printf("  Error: tenes que ingresar un numero.\n");
            } else if (leidos == 2) {
                if (sobra == ',') {
                    printf("  Error: usa punto (.) para los decimales, no coma.\n");
                } else {
                    printf("  Error: hay caracteres de mas despues del numero.\n");
                }
            } else if (!(valor >= min && valor <= max)) {
                /* escrito asi tambien rechaza valores raros como "nan" o "inf" */
                printf("  Error: el valor debe estar entre %.2f y %.2f.\n", min, max);
            } else {
                valido = 1;
            }
        }
    } while (valido == 0);

    return valor;
}

/* Espera a que el usuario presione Enter */
void pausar(void)
{
    char linea[MAX_LINEA];

    printf("\nPresiona Enter para continuar...");
    leer_linea(linea, MAX_LINEA);
}
