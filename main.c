/*
 * main.c
 * Simulador de Misiones Artemis - Escenario 4: Reingreso atmosferico,
 * frenado aerodinamico y amerizaje de Orion
 * "Control de Reingreso Orion"
 * Grupo D14 - Comision 1.4 - Algoritmos y Estructuras de Datos - UTN FRRe
 *
 * Solo tiene el menu principal y la navegacion. Los calculos estan en
 * simulador.c y la lectura de datos en validaciones.c.
 */

#include <stdio.h>
#include "simulador.h"
#include "validaciones.h"

void mostrar_menu(void);

int main(void)
{
    Mision mision;
    int opcion;

    inicializar_mision(&mision);   /* arranca con los datos del enunciado */

    printf("=================================================\n");
    printf("     CONTROL DE REINGRESO ORION - ARTEMIS\n");
    printf("  Reingreso atmosferico, frenado y amerizaje\n");
    printf("=================================================\n");
    printf("Orion vuelve de la Luna a 11 km/s. Sos el responsable del\n");
    printf("control de mision y tenes que traer a la tripulacion a salvo.\n");

    do {
        mostrar_menu();
        opcion = leer_entero("Elegi una opcion: ", 1, 6);

        switch (opcion) {
            case 1:
                configurar_mision(&mision);
                break;
            case 2:
                mostrar_analisis(&mision);
                break;
            case 3:
                jugar_turnos(&mision);
                break;
            case 4:
                mostrar_historial(&mision);
                break;
            case 5:
                mostrar_informe_final(&mision);
                break;
            case 6:
                printf("\nFin de la simulacion. Bienvenidos a casa, tripulacion!\n");
                break;
        }

        if (opcion != 6) {
            pausar();
        }
    } while (opcion != 6);

    return 0;
}

void mostrar_menu(void)
{
    printf("\n============== MENU PRINCIPAL ==============\n");
    printf("1. Configurar mision\n");
    printf("2. Analisis del reingreso (incisos a, b, c y d)\n");
    printf("3. Control de reingresos (juego)\n");
    printf("4. Historial de reingresos\n");
    printf("5. Informe final\n");
    printf("6. Salir\n");
}
