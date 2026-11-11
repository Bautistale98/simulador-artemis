/*
 * simulador.c
 * Calculos fisicos y logica del simulador "Control de Reingreso Orion".
 * Grupo D14 - Comision 1.4 - Algoritmos y Estructuras de Datos - UTN FRRe
 */

#include <stdio.h>
#include "simulador.h"
#include "validaciones.h"

/* Velocidad de reingreso que informa el control de mision en cada turno [m/s].
   El primero es el caso del enunciado (11 km/s). */
static double velocidades[MAX_TURNOS] = {11000.0, 10800.0, 11200.0, 10500.0, 11500.0};

/* ================== CONFIGURACION ================== */

/* Carga los datos del enunciado y deja el historial vacio */
void inicializar_mision(Mision *m)
{
    m->datos.masa = MASA_REF;
    m->datos.v_reingreso = V_REINGRESO_REF;
    m->datos.alt_reingreso = ALT_REINGRESO_REF * 1000.0;     /* km -> m */
    m->datos.v_paracaidas = V_PARACAIDAS_REF;
    m->datos.alt_paracaidas = ALT_PARACAIDAS_REF * 1000.0;   /* km -> m */
    m->datos.v_impacto = V_IMPACTO_REF;
    m->datos.hundimiento = HUNDIMIENTO_REF;
    m->datos.tiempo_frenado = TIEMPO_FRENADO_REF;
    calcular_resultados(&m->datos, &m->res);
    m->cant_reingresos = 0;
}

/* Permite cambiar los datos. Si cambian, se reinicia el historial del juego. */
void configurar_mision(Mision *m)
{
    int opcion;
    DatosReingreso *d = &m->datos;

    printf("\n========== CONFIGURAR MISION ==========\n");
    printf("1. Usar los datos del enunciado\n");
    printf("2. Ingresar datos propios\n");
    printf("3. Volver sin cambios\n");
    opcion = leer_entero("Elegi una opcion: ", 1, 3);

    if (opcion == 1) {
        inicializar_mision(m);
        printf("\nSe cargaron los datos del enunciado.\n");
    } else if (opcion == 2) {
        d->masa = leer_real("Masa de la capsula en kg (1000 a 20000): ", MASA_MIN, MASA_MAX);
        d->v_reingreso = leer_real("Velocidad de reingreso en m/s (1000 a 12000): ", V_REINGRESO_MIN, V_REINGRESO_MAX);
        d->alt_reingreso = leer_real("Altitud de reingreso en km (50 a 200): ", ALT_REINGRESO_MIN, ALT_REINGRESO_MAX) * 1000.0;
        d->v_paracaidas = leer_real("Velocidad al abrir paracaidas en m/s (10 a 100): ", V_PARACAIDAS_MIN, V_PARACAIDAS_MAX);
        d->alt_paracaidas = leer_real("Altitud de apertura de paracaidas en km (1 a 5): ", ALT_PARACAIDAS_MIN, ALT_PARACAIDAS_MAX) * 1000.0;
        d->v_impacto = leer_real("Velocidad al tocar el agua en m/s (1 a 15): ", V_IMPACTO_MIN, V_IMPACTO_MAX);
        d->hundimiento = leer_real("Distancia que se hunde en el agua en m (0.5 a 3): ", HUNDIMIENTO_MIN, HUNDIMIENTO_MAX);
        d->tiempo_frenado = leer_real("Duracion de la fase de frenado en s (60 a 600): ", TIEMPO_MIN, TIEMPO_MAX);
        calcular_resultados(d, &m->res);
        m->cant_reingresos = 0;
        printf("\nMision configurada. El historial de reingresos se reinicio.\n");
    }

    if (opcion != 3) {
        mostrar_datos(m);
    }
}

/* ================== CALCULOS FISICOS ================== */

/*
 * Trabajo del rozamiento del aire entre el reingreso y la apertura de paracaidas.
 * Teorema trabajo-energia: W_total = Ec_final - Ec_inicial
 *   W_roz + W_peso = Ec_final - Ec_inicial  ->  W_roz = (Ec_final - Ec_inicial) - W_peso
 */
double calcular_trabajo_rozamiento(DatosReingreso *d, double v_reingreso)
{
    double ec_inicial, ec_final, trabajo_peso;

    ec_inicial = 0.5 * d->masa * v_reingreso * v_reingreso;
    ec_final = 0.5 * d->masa * d->v_paracaidas * d->v_paracaidas;
    trabajo_peso = d->masa * GRAVEDAD * (d->alt_reingreso - d->alt_paracaidas);
    return (ec_final - ec_inicial) - trabajo_peso;
}

/* Desaceleracion media en multiplos de g: a = (vi - vf) / t */
double calcular_desaceleracion_g(double v_inicial, double v_final, double tiempo)
{
    if (tiempo <= 0) {
        return 0;   /* evitamos dividir por cero */
    }
    return (v_inicial - v_final) / tiempo / GRAVEDAD;
}

/*
 * Calcula los incisos a, b y c con los datos cargados.
 * Devuelve 1 si pudo calcular y 0 si los datos no sirven.
 */
int calcular_resultados(DatosReingreso *d, Resultados *r)
{
    if (d->masa <= 0 || d->tiempo_frenado <= 0 || d->hundimiento <= 0) {
        return 0;
    }

    /* a) trabajo del rozamiento */
    r->ec_inicial = 0.5 * d->masa * d->v_reingreso * d->v_reingreso;
    r->ec_final = 0.5 * d->masa * d->v_paracaidas * d->v_paracaidas;
    r->trabajo_peso = d->masa * GRAVEDAD * (d->alt_reingreso - d->alt_paracaidas);
    r->trabajo_roz = calcular_trabajo_rozamiento(d, d->v_reingreso);

    /* b) desaceleracion media */
    r->desaceleracion = (d->v_reingreso - d->v_paracaidas) / d->tiempo_frenado;
    r->desaceleracion_g = r->desaceleracion / GRAVEDAD;

    /* c) fuerza del agua: el agua frena la capsula en la distancia d.
          W_agua + W_peso = 0 - Ec  ->  F d = 1/2 m v^2 + m g d
          F = m (v^2 / (2 d) + g) */
    r->fuerza_agua = d->masa * (d->v_impacto * d->v_impacto / (2 * d->hundimiento) + GRAVEDAD);
    r->fuerza_en_pesos = r->fuerza_agua / (d->masa * GRAVEDAD);
    return 1;
}

/* Evalua un reingreso del juego segun el angulo y la desaceleracion */
int evaluar_reingreso(double angulo, double desaceleracion_g)
{
    if (angulo < ANGULO_MIN) {
        return REBOTE;
    } else if (angulo > ANGULO_MAX) {
        return ANGULO_EXCESIVO;
    } else if (desaceleracion_g > LIMITE_G) {
        return FRENADO_EXCESIVO;
    } else if (angulo >= ANGULO_IDEAL_MIN && angulo <= ANGULO_IDEAL_MAX) {
        return PERFECTO;
    }
    return EXITOSO;
}

/* Texto del resultado para mostrar en pantalla */
static char *texto_resultado(int resultado)
{
    if (resultado == PERFECTO) {
        return "PERFECTO";
    } else if (resultado == EXITOSO) {
        return "EXITOSO";
    } else if (resultado == REBOTE) {
        return "FALLIDO - rebote";
    } else if (resultado == ANGULO_EXCESIVO) {
        return "FALLIDO - angulo";
    }
    return "FALLIDO - frenado";
}

/* Puntos que da cada resultado */
static int puntos_resultado(int resultado)
{
    if (resultado == PERFECTO) {
        return 2;
    } else if (resultado == EXITOSO) {
        return 1;
    }
    return 0;
}

/* ================== PANTALLAS ================== */

void mostrar_datos(Mision *m)
{
    DatosReingreso *d = &m->datos;   /* puntero para no escribir m->datos cada vez */

    printf("\n---------- DATOS DE LA MISION ----------\n");
    printf("Masa de la capsula        : %.2f kg\n", d->masa);
    printf("Velocidad de reingreso    : %.2f m/s (%.2f km/h)\n", d->v_reingreso, d->v_reingreso * 3.6);
    printf("Altitud de reingreso      : %.2f km\n", d->alt_reingreso / 1000.0);
    printf("Velocidad con paracaidas  : %.2f m/s\n", d->v_paracaidas);
    printf("Altitud de paracaidas     : %.2f km\n", d->alt_paracaidas / 1000.0);
    printf("Velocidad de amerizaje    : %.2f m/s\n", d->v_impacto);
    printf("Hundimiento en el agua    : %.2f m\n", d->hundimiento);
    printf("Duracion del frenado      : %.2f s\n", d->tiempo_frenado);
    printf("Reingresos jugados        : %d de %d\n", m->cant_reingresos, MAX_TURNOS);
}

/* Muestra el procedimiento de los incisos a, b, c y d con los datos actuales */
void mostrar_analisis(Mision *m)
{
    DatosReingreso *d = &m->datos;
    Resultados *r = &m->res;

    printf("\n========== ANALISIS DEL REINGRESO ==========\n");
    printf("Datos: m = %.0f kg | vi = %.0f m/s | hi = %.0f m | vf = %.0f m/s | hf = %.0f m | g = %.1f m/s^2\n",
           d->masa, d->v_reingreso, d->alt_reingreso, d->v_paracaidas, d->alt_paracaidas, GRAVEDAD);

    printf("\nA) TRABAJO DEL ROZAMIENTO ATMOSFERICO (de %.0f m a %.0f m)\n", d->alt_reingreso, d->alt_paracaidas);
    printf("   Teorema del trabajo y la energia: W_roz + W_peso = Ec_final - Ec_inicial\n");
    printf("   Ec_inicial = 1/2 m vi^2 = 0.5 x %.0f x %.0f^2 = %.4e J\n", d->masa, d->v_reingreso, r->ec_inicial);
    printf("   Ec_final   = 1/2 m vf^2 = 0.5 x %.0f x %.0f^2 = %.4e J\n", d->masa, d->v_paracaidas, r->ec_final);
    printf("   W_peso     = m g (hi - hf) = %.0f x %.1f x %.0f = %.4e J\n",
           d->masa, GRAVEDAD, d->alt_reingreso - d->alt_paracaidas, r->trabajo_peso);
    printf("   W_roz = (Ec_final - Ec_inicial) - W_peso = %.4e J\n", r->trabajo_roz);
    printf("   Es negativo porque el aire frena la capsula: esa energia la disipa el escudo termico.\n");

    printf("\nB) DESACELERACION MEDIA (fase de %.0f s)\n", d->tiempo_frenado);
    printf("   a = (vi - vf) / t = (%.0f - %.0f) / %.0f = %.2f m/s^2\n",
           d->v_reingreso, d->v_paracaidas, d->tiempo_frenado, r->desaceleracion);
    printf("   En multiplos de g: a / g = %.2f / %.1f = %.2f g\n", r->desaceleracion, GRAVEDAD, r->desaceleracion_g);

    printf("\nC) FUERZA DEL AGUA EN EL AMERIZAJE (v = %.1f m/s, se hunde %.2f m)\n", d->v_impacto, d->hundimiento);
    printf("   El agua frena la capsula hasta detenerla: F d = 1/2 m v^2 + m g d\n");
    printf("   F = m (v^2 / (2 d) + g) = %.0f x (%.1f^2 / (2 x %.2f) + %.1f) = %.2f N\n",
           d->masa, d->v_impacto, d->hundimiento, GRAVEDAD, r->fuerza_agua);
    printf("   En multiplos del peso: F / (m g) = %.2f veces el peso de la nave\n", r->fuerza_en_pesos);

    printf("\nD) ANGULO DE REINGRESO (ventana segura: %.1f a %.1f grados)\n", ANGULO_MIN, ANGULO_MAX);
    printf("   Si el angulo es mayor a %.1f: la capsula entra muy empinada, atraviesa aire\n", ANGULO_MAX);
    printf("   denso muy rapido y la desaceleracion y el calentamiento son excesivos.\n");
    printf("   Si el angulo es menor a %.1f: entra muy rasante, no frena lo suficiente y\n", ANGULO_MIN);
    printf("   rebota en la atmosfera, volviendo al espacio sin control.\n");
    printf("   Esto se pone a prueba en la opcion 3 (reingresos).\n");
}

/* Ciclo de turnos: en cada turno el jugador controla un reingreso */
void jugar_turnos(Mision *m)
{
    Reingreso *rein;
    int opcion;
    int salir = 0;

    if (m->cant_reingresos == MAX_TURNOS) {
        printf("\nLa mision ya termino. Mira el informe final o configura una mision nueva.\n");
        return;
    }

    printf("\n========== CONTROL DE REINGRESO ==========\n");
    printf("En cada turno el control de mision informa la velocidad de reingreso.\n");
    printf("Vos elegis el angulo de reingreso y la duracion del frenado.\n");
    printf("- Angulo seguro: entre %.1f y %.1f grados (ideal: %.1f a %.1f)\n",
           ANGULO_MIN, ANGULO_MAX, ANGULO_IDEAL_MIN, ANGULO_IDEAL_MAX);
    printf("- La tripulacion no puede superar %.1f g de desaceleracion: a = (vi - vf) / t\n", LIMITE_G);
    printf("Para ganar necesitas %d reingresos exitosos de %d.\n", REINGRESOS_PARA_EXITO, MAX_TURNOS);

    while (m->cant_reingresos < MAX_TURNOS && salir == 0) {
        printf("\n---------- REINGRESO %d de %d ----------\n", m->cant_reingresos + 1, MAX_TURNOS);
        printf("CONTROL DE MISION: la capsula llega a %.0f m/s. Hay que frenarla hasta %.0f m/s.\n",
               velocidades[m->cant_reingresos], m->datos.v_paracaidas);
        printf("1. Iniciar reingreso\n");
        printf("2. Ver datos de la mision\n");
        printf("3. Pausar y volver al menu\n");
        opcion = leer_entero("Elegi una opcion: ", 1, 3);

        if (opcion == 1) {
            /* puntero al lugar del arreglo donde se guarda este reingreso */
            rein = &m->historial[m->cant_reingresos];
            rein->turno = m->cant_reingresos + 1;
            rein->v_reingreso = velocidades[m->cant_reingresos];
            rein->angulo = leer_real("Angulo de reingreso en grados (1 a 15): ", ANGULO_ING_MIN, ANGULO_ING_MAX);
            rein->tiempo = leer_real("Duracion del frenado en s (60 a 600): ", TIEMPO_MIN, TIEMPO_MAX);
            rein->desaceleracion_g = calcular_desaceleracion_g(rein->v_reingreso, m->datos.v_paracaidas, rein->tiempo);
            rein->trabajo_roz = calcular_trabajo_rozamiento(&m->datos, rein->v_reingreso);
            rein->resultado = evaluar_reingreso(rein->angulo, rein->desaceleracion_g);
            m->cant_reingresos++;

            printf("\n  Desaceleracion media: %.2f g | Energia disipada por el escudo: %.3e J\n",
                   rein->desaceleracion_g, -rein->trabajo_roz);
            if (rein->resultado == REBOTE) {
                printf("  Angulo muy bajo: la capsula reboto en la atmosfera y volvio al espacio.\n");
            } else if (rein->resultado == ANGULO_EXCESIVO) {
                printf("  Angulo muy empinado: desaceleracion y calentamiento excesivos.\n");
            } else if (rein->resultado == FRENADO_EXCESIVO) {
                printf("  Frenado demasiado brusco: la tripulacion supero los %.1f g.\n", LIMITE_G);
            } else {
                printf("  Orion atraveso la atmosfera y abrio los paracaidas sin problemas.\n");
            }
            printf("  Resultado: REINGRESO %s\n", texto_resultado(rein->resultado));
        } else if (opcion == 2) {
            mostrar_datos(m);
        } else {
            salir = 1;
            printf("\nControl en pausa. Podes continuar desde el menu (opcion 3).\n");
        }
    }

    if (m->cant_reingresos == MAX_TURNOS) {
        printf("\nSe completaron los %d reingresos. Mira el informe final (opcion 5).\n", MAX_TURNOS);
    }
}

void mostrar_historial(Mision *m)
{
    int i;
    Reingreso *rein;

    printf("\n========== HISTORIAL DE REINGRESOS ==========\n");
    if (m->cant_reingresos == 0) {
        printf("Todavia no se jugo ningun reingreso.\n");
        return;
    }

    printf("Turno | V. reingreso(m/s) | Angulo | Frenado(s) | Desacel.(g) | Resultado\n");
    for (i = 0; i < m->cant_reingresos; i++) {
        rein = &m->historial[i];
        printf("%5d | %17.0f | %6.2f | %10.1f | %11.2f | %s\n",
               rein->turno, rein->v_reingreso, rein->angulo, rein->tiempo,
               rein->desaceleracion_g, texto_resultado(rein->resultado));
    }
}

void mostrar_informe_final(Mision *m)
{
    int i;
    int perfectos = 0, exitosos = 0, fallidos = 0;
    int puntaje = 0;
    double suma_g = 0;

    /* recorremos el arreglo para contar resultados */
    for (i = 0; i < m->cant_reingresos; i++) {
        if (m->historial[i].resultado == PERFECTO) {
            perfectos++;
        } else if (m->historial[i].resultado == EXITOSO) {
            exitosos++;
        } else {
            fallidos++;
        }
        puntaje = puntaje + puntos_resultado(m->historial[i].resultado);
        suma_g = suma_g + m->historial[i].desaceleracion_g;
    }

    printf("\n================ INFORME FINAL ================\n");
    mostrar_datos(m);
    printf("\nResultados del enunciado: W_roz = %.4e J | a = %.2f g | F_agua = %.2f N (%.2f pesos)\n",
           m->res.trabajo_roz, m->res.desaceleracion_g, m->res.fuerza_agua, m->res.fuerza_en_pesos);
    mostrar_historial(m);

    printf("\n---------- RESUMEN ----------\n");
    printf("Reingresos perfectos  : %d\n", perfectos);
    printf("Reingresos exitosos   : %d\n", exitosos);
    printf("Reingresos fallidos   : %d\n", fallidos);
    if (m->cant_reingresos > 0) {
        printf("Desaceleracion media  : %.2f g\n", suma_g / m->cant_reingresos);
    }
    printf("Puntaje               : %d / %d\n", puntaje, MAX_TURNOS * 2);

    if (m->cant_reingresos < MAX_TURNOS) {
        printf("Estado de la mision   : EN CURSO (faltan %d reingresos)\n", MAX_TURNOS - m->cant_reingresos);
    } else if (perfectos + exitosos >= REINGRESOS_PARA_EXITO) {
        printf("Estado de la mision   : EXITOSA - la tripulacion volvio a salvo\n");
    } else {
        printf("Estado de la mision   : FALLIDA - demasiados reingresos fuera de los limites\n");
    }

    if (m->cant_reingresos == MAX_TURNOS) {
        if (puntaje >= 9) {
            printf("Desempeno             : Excelente, control de mision de primer nivel\n");
        } else if (puntaje >= 6) {
            printf("Desempeno             : Bueno\n");
        } else if (puntaje >= 3) {
            printf("Desempeno             : Regular, hay que practicar mas\n");
        } else {
            printf("Desempeno             : Malo, volver al simulador\n");
        }
    }
}
