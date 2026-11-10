/*
 * simulador.c
 * Calculos fisicos y logica del simulador "Centro de Control Orbital Orion".
 * Grupo Lab C N 14 - Algoritmos y Estructuras de Datos - UTN FRRe
 */

#include <stdio.h>
#include <math.h>
#include "simulador.h"
#include "validaciones.h"

/* Desvios laterales que pide el control de tierra en cada turno [m].
   El primero (25 m) es justo el caso del enunciado: 0,50 m/s^2 durante 10 s. */
static double objetivos[MAX_TURNOS] = {25.0, 40.0, 12.0, 90.0, 60.0};

/* ================== CONFIGURACION ================== */

/* Carga los valores de referencia del enunciado y deja el historial vacio */
void inicializar_mision(Mision *m)
{
    m->orbita.altitud = ALTITUD_REF_KM * 1000.0;   /* km -> m */
    m->orbita.masa = MASA_REF;
    calcular_orbita(&m->orbita);
    m->cant_maniobras = 0;
}

/* Permite elegir los datos de la mision. Si cambian los datos se reinicia el historial. */
void configurar_mision(Mision *m)
{
    int opcion;
    double altitud_km;

    printf("\n========== CONFIGURAR MISION ==========\n");
    printf("1. Usar los valores del enunciado (h = 192 km, m = 33000 kg)\n");
    printf("2. Ingresar altitud y masa\n");
    printf("3. Volver sin cambios\n");
    opcion = leer_entero("Elegi una opcion: ", 1, 3);

    if (opcion == 1) {
        inicializar_mision(m);
        printf("\nSe cargaron los valores del enunciado.\n");
    } else if (opcion == 2) {
        altitud_km = leer_real("Altitud orbital en km (160 a 2000): ", ALTITUD_MIN_KM, ALTITUD_MAX_KM);
        m->orbita.masa = leer_real("Masa de Orion en kg (1000 a 100000): ", MASA_MIN, MASA_MAX);
        m->orbita.altitud = altitud_km * 1000.0;   /* pasamos a metros (SI) */
        calcular_orbita(&m->orbita);
        m->cant_maniobras = 0;
        printf("\nMision configurada. El historial de maniobras se reinicio.\n");
    }

    if (opcion != 3) {
        mostrar_telemetria(m);
    }
}

/* ================== CALCULOS FISICOS ================== */

/*
 * Calcula todos los datos de la orbita circular a partir de la altitud y la masa.
 * Devuelve 1 si pudo calcular y 0 si los datos no sirven (evita dividir por cero).
 */
int calcular_orbita(EstadoOrbital *e)
{
    e->radio = RADIO_TIERRA + e->altitud;            /* r = RT + h */

    if (e->radio <= 0 || e->masa <= 0) {
        return 0;
    }

    e->velocidad = sqrt(G * MASA_TIERRA / e->radio);            /* v = raiz(G MT / r) */
    e->periodo = 2 * PI * e->radio / e->velocidad;              /* T = 2 pi r / v */
    e->acel_centripeta = e->velocidad * e->velocidad / e->radio; /* ac = v^2 / r */
    e->fuerza_gravitatoria = G * MASA_TIERRA * e->masa / (e->radio * e->radio); /* Fg */
    return 1;
}

/* Cambio de velocidad lateral: dv = a * t */
double calcular_delta_v(double aceleracion, double tiempo)
{
    return aceleracion * tiempo;
}

/* Desvio lateral (aceleracion constante partiendo de velocidad lateral 0): d = 1/2 a t^2 */
double calcular_desvio(double aceleracion, double tiempo)
{
    return 0.5 * aceleracion * tiempo * tiempo;
}

/* Error porcentual entre lo que se logro y lo que se pidio */
double calcular_error(double desvio, double objetivo)
{
    double diferencia;

    if (objetivo <= 0) {
        return 100.0;   /* no deberia pasar, pero evitamos dividir por cero */
    }

    diferencia = desvio - objetivo;
    if (diferencia < 0) {
        diferencia = -diferencia;
    }
    return diferencia / objetivo * 100.0;
}

/* Clasifica la maniobra segun el error */
int clasificar_maniobra(double error)
{
    if (error <= TOLERANCIA_PRECISA) {
        return PRECISA;
    } else if (error <= TOLERANCIA_ACEPTABLE) {
        return ACEPTABLE;
    } else {
        return FALLIDA;
    }
}

/* Devuelve el texto del resultado para mostrarlo en pantalla */
static char *texto_resultado(int resultado)
{
    if (resultado == PRECISA) {
        return "PRECISA";
    } else if (resultado == ACEPTABLE) {
        return "ACEPTABLE";
    }
    return "FALLIDA";
}

/* ================== PANTALLAS ================== */

void mostrar_telemetria(Mision *m)
{
    EstadoOrbital *e = &m->orbita;   /* puntero para no escribir m->orbita cada vez */

    printf("\n---------- TELEMETRIA DE ORION ----------\n");
    printf("Altitud              : %.2f km\n", e->altitud / 1000.0);
    printf("Masa                 : %.2f kg\n", e->masa);
    printf("Radio orbital (r)    : %.2f m\n", e->radio);
    printf("Velocidad orbital    : %.2f m/s\n", e->velocidad);
    printf("Periodo              : %.2f s (%.2f min)\n", e->periodo, e->periodo / 60.0);
    printf("Acel. centripeta     : %.4f m/s^2\n", e->acel_centripeta);
    printf("Fuerza gravitatoria  : %.2f N\n", e->fuerza_gravitatoria);
    printf("Maniobras realizadas : %d de %d\n", m->cant_maniobras, MAX_TURNOS);
}

/* Muestra el procedimiento de los incisos a, b, c y d con los datos actuales */
void mostrar_analisis_orbital(Mision *m)
{
    EstadoOrbital *e = &m->orbita;
    double dv, d;

    printf("\n========== ANALISIS ORBITAL ==========\n");
    printf("Datos: G = %.3e N m^2/kg^2 | MT = %.3e kg | RT = %.0f m\n", G, MASA_TIERRA, RADIO_TIERRA);
    printf("       h = %.0f m | m = %.0f kg | r = RT + h = %.0f m\n", e->altitud, e->masa, e->radio);

    printf("\nA) VELOCIDAD ORBITAL Y FUERZA CENTRIPETA\n");
    printf("   La gravedad es la unica fuerza que actua y cumple el papel de fuerza centripeta:\n");
    printf("   Fg = G MT m / r^2     y     Fc = m v^2 / r\n");
    printf("   Igualando: G MT m / r^2 = m v^2 / r\n");
    printf("   Se cancela la masa m de Orion y queda: v^2 = G MT / r\n");
    printf("   v = raiz(G MT / r) = raiz(%.3e x %.3e / %.0f)\n", G, MASA_TIERRA, e->radio);
    printf("   v = %.2f m/s\n", e->velocidad);
    printf("   Fuerza gravitatoria (= centripeta): Fg = %.2f N\n", e->fuerza_gravitatoria);

    printf("\nB) PERIODO ORBITAL\n");
    printf("   T = 2 pi r / v = 2 x %.5f x %.0f / %.2f\n", PI, e->radio, e->velocidad);
    printf("   T = %.2f s = %.2f min\n", e->periodo, e->periodo / 60.0);

    printf("\nC) ACELERACION CENTRIPETA\n");
    printf("   ac = v^2 / r = %.2f^2 / %.0f = %.4f m/s^2\n", e->velocidad, e->radio, e->acel_centripeta);
    printf("   Comprobacion: ac = G MT / r^2 = %.4f m/s^2\n", G * MASA_TIERRA / (e->radio * e->radio));
    printf("   Nota: v y ac NO dependen de la masa de Orion; Fg si depende.\n");

    dv = calcular_delta_v(ACEL_REF, TIEMPO_REF);
    d = calcular_desvio(ACEL_REF, TIEMPO_REF);
    printf("\nD) PROPULSOR LATERAL (a = %.2f m/s^2 durante %.0f s)\n", ACEL_REF, TIEMPO_REF);
    printf("   Hipotesis: aceleracion constante, en direccion fija perpendicular a la\n");
    printf("   velocidad inicial, partiendo de velocidad lateral 0.\n");
    printf("   dv = a t = %.2f x %.0f = %.2f m/s\n", ACEL_REF, TIEMPO_REF, dv);
    printf("   d  = 1/2 a t^2 = 0.5 x %.2f x %.0f^2 = %.2f m\n", ACEL_REF, TIEMPO_REF, d);
    printf("   Es valida porque 10 s es apenas el %.2f %% del periodo: en ese tiempo la\n",
           TIEMPO_REF / e->periodo * 100.0);
    printf("   direccion de la velocidad casi no cambia. La trayectoria se desvia un poco y\n");
    printf("   la orbita deja de ser un circulo perfecto (el programa no simula la orbita nueva).\n");
}

/* Ciclo de turnos: en cada turno el control de tierra pide un desvio */
void jugar_turnos(Mision *m)
{
    Maniobra *man;
    int opcion;
    int salir = 0;

    if (m->cant_maniobras == MAX_TURNOS) {
        printf("\nLa mision ya termino. Mira el informe final o configura una mision nueva.\n");
        return;
    }

    printf("\n========== SUPERVISION ORBITAL ==========\n");
    printf("En cada turno el control de tierra pide desviar la nave cierta distancia.\n");
    printf("Vos elegis la aceleracion del propulsor y la duracion del pulso.\n");
    printf("Desvio: d = 1/2 a t^2 | Precisa: error <= %.0f %% | Aceptable: <= %.0f %%\n",
           TOLERANCIA_PRECISA, TOLERANCIA_ACEPTABLE);
    printf("Para ganar necesitas %d maniobras precisas o aceptables de %d.\n",
           MANIOBRAS_PARA_EXITO, MAX_TURNOS);

    while (m->cant_maniobras < MAX_TURNOS && salir == 0) {
        printf("\n---------- TURNO %d de %d ----------\n", m->cant_maniobras + 1, MAX_TURNOS);
        printf("CONTROL DE TIERRA: desviar a Orion %.2f m en direccion lateral.\n",
               objetivos[m->cant_maniobras]);
        printf("1. Ejecutar maniobra\n");
        printf("2. Ver telemetria\n");
        printf("3. Pausar supervision y volver al menu\n");
        opcion = leer_entero("Elegi una opcion: ", 1, 3);

        if (opcion == 1) {
            /* puntero al lugar del arreglo donde se guarda esta maniobra */
            man = &m->historial[m->cant_maniobras];
            man->turno = m->cant_maniobras + 1;
            man->objetivo = objetivos[m->cant_maniobras];
            man->aceleracion = leer_real("Aceleracion del propulsor (0.01 a 0.50 m/s^2): ", ACEL_MIN, ACEL_MAX);
            man->tiempo = leer_real("Duracion del pulso (1 a 30 s): ", TIEMPO_MIN, TIEMPO_MAX);
            man->delta_v = calcular_delta_v(man->aceleracion, man->tiempo);
            man->desvio = calcular_desvio(man->aceleracion, man->tiempo);
            man->error_porcentual = calcular_error(man->desvio, man->objetivo);
            man->resultado = clasificar_maniobra(man->error_porcentual);
            m->cant_maniobras++;

            printf("\n  Cambio de velocidad lateral: %.2f m/s (%.3f %% de la velocidad orbital)\n",
                   man->delta_v, man->delta_v / m->orbita.velocidad * 100.0);
            printf("  Desvio obtenido: %.2f m | Pedido: %.2f m | Error: %.2f %%\n",
                   man->desvio, man->objetivo, man->error_porcentual);
            printf("  Resultado: MANIOBRA %s\n", texto_resultado(man->resultado));
        } else if (opcion == 2) {
            mostrar_telemetria(m);
        } else {
            salir = 1;
            printf("\nSupervision en pausa. Podes continuar desde el menu (opcion 3).\n");
        }
    }

    if (m->cant_maniobras == MAX_TURNOS) {
        printf("\nSe completaron los %d turnos. Mira el informe final (opcion 5).\n", MAX_TURNOS);
    }
}

void mostrar_historial(Mision *m)
{
    int i;
    Maniobra *man;

    printf("\n========== HISTORIAL DE MANIOBRAS ==========\n");
    if (m->cant_maniobras == 0) {
        printf("Todavia no se realizo ninguna maniobra.\n");
        return;
    }

    printf("Turno | Objetivo(m) |  a(m/s^2) |  t(s) | dv(m/s) | Desvio(m) | Error(%%) | Resultado\n");
    for (i = 0; i < m->cant_maniobras; i++) {
        man = &m->historial[i];
        printf("%5d | %11.2f | %9.2f | %5.1f | %7.2f | %9.2f | %8.2f | %s\n",
               man->turno, man->objetivo, man->aceleracion, man->tiempo,
               man->delta_v, man->desvio, man->error_porcentual, texto_resultado(man->resultado));
    }
}

void mostrar_informe_final(Mision *m)
{
    int i;
    int precisas = 0, aceptables = 0, fallidas = 0;
    int puntaje;
    double suma_error = 0;

    /* recorremos el arreglo para contar resultados */
    for (i = 0; i < m->cant_maniobras; i++) {
        if (m->historial[i].resultado == PRECISA) {
            precisas++;
        } else if (m->historial[i].resultado == ACEPTABLE) {
            aceptables++;
        } else {
            fallidas++;
        }
        suma_error = suma_error + m->historial[i].error_porcentual;
    }
    puntaje = precisas * 2 + aceptables;   /* maximo: 5 turnos x 2 = 10 puntos */

    printf("\n================ INFORME FINAL ================\n");
    mostrar_telemetria(m);
    mostrar_historial(m);

    printf("\n---------- RESUMEN ----------\n");
    printf("Maniobras precisas   : %d\n", precisas);
    printf("Maniobras aceptables : %d\n", aceptables);
    printf("Maniobras fallidas   : %d\n", fallidas);
    if (m->cant_maniobras > 0) {
        printf("Error promedio       : %.2f %%\n", suma_error / m->cant_maniobras);
    }
    printf("Puntaje              : %d / %d\n", puntaje, MAX_TURNOS * 2);

    if (m->cant_maniobras < MAX_TURNOS) {
        printf("Estado de la mision  : EN CURSO (faltan %d turnos)\n", MAX_TURNOS - m->cant_maniobras);
    } else if (precisas + aceptables >= MANIOBRAS_PARA_EXITO) {
        printf("Estado de la mision  : EXITOSA - Orion mantuvo una orbita estable\n");
    } else {
        printf("Estado de la mision  : FALLIDA - demasiadas maniobras fuera de tolerancia\n");
    }

    if (m->cant_maniobras == MAX_TURNOS) {
        if (puntaje >= 9) {
            printf("Desempeno            : Excelente, control de mision de primer nivel\n");
        } else if (puntaje >= 6) {
            printf("Desempeno            : Bueno\n");
        } else if (puntaje >= 3) {
            printf("Desempeno            : Regular, hay que practicar mas\n");
        } else {
            printf("Desempeno            : Malo, volver al simulador\n");
        }
    }
}
