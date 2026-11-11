/*
 * main.c
 * Simulador de Misiones Artemis - Escenario 2: Orion en orbita LEO
 * "Centro de Control Orbital Orion"
 * Grupo Lab C N 14 - Algoritmos y Estructuras de Datos - UTN FRRe
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

    inicializar_mision(&mision);   /* arranca con los valores del enunciado */

    printf("=================================================\n");
    printf("   CENTRO DE CONTROL ORBITAL ORION - ARTEMIS\n");
    printf("   Orbita terrestre baja (LEO) - MCU y Gravitacion\n");
    printf("=================================================\n");
    printf("Sos el responsable del control de la mision. Orion ya esta\n");
    printf("en orbita y tenes que mantenerla estable con el propulsor lateral.\n");

    do {
        mostrar_menu();
        opcion = leer_entero("Elegi una opcion: ", 1, 6);

        switch (opcion) {
            case 1:
                configurar_mision(&mision);
                break;
            case 2:
                mostrar_analisis_orbital(&mision);
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
                printf("\nFin de la simulacion. Hasta la proxima mision!\n");
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
    printf("2. Analisis orbital (incisos a, b, c y d)\n");
    printf("3. Supervision por turnos (maniobras)\n");
    printf("4. Historial de maniobras\n");
    printf("5. Informe final\n");
    printf("6. Salir\n");
}
