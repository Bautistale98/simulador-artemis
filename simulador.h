/*
 * simulador.h
 * Simulador de Misiones Artemis - Escenario 4:
 * Reingreso atmosferico, frenado aerodinamico y amerizaje de Orion
 * (Trabajo, Energia e Impacto)
 * Grupo D14 - Comision 1.4 - Algoritmos y Estructuras de Datos - UTN FRRe
 *
 * Contiene las constantes, las estructuras y los prototipos del simulador.
 */

#ifndef SIMULADOR_H
#define SIMULADOR_H

/* ---------- Constante fisica (unidades SI) ---------- */
#define GRAVEDAD  9.8                /* aceleracion de la gravedad [m/s^2] */

/* ---------- Valores de referencia del enunciado ---------- */
#define MASA_REF            10400.0  /* masa del modulo de tripulacion [kg] */
#define V_REINGRESO_REF     11000.0  /* velocidad de reingreso [m/s] */
#define ALT_REINGRESO_REF   120.0    /* altitud de reingreso [km] */
#define V_PARACAIDAS_REF    25.0     /* velocidad al abrir los paracaidas [m/s] */
#define ALT_PARACAIDAS_REF  2.5      /* altitud de apertura de paracaidas [km] */
#define V_IMPACTO_REF       8.0      /* velocidad al tocar el agua [m/s] */
#define HUNDIMIENTO_REF     1.5      /* distancia que se hunde en el agua [m] */
#define TIEMPO_FRENADO_REF  400.0    /* duracion de la fase de mayor frenado [s] */

/* ---------- Ventana de seguridad del angulo de reingreso (consigna) ---------- */
#define ANGULO_MIN  5.5              /* menos: rebote atmosferico [grados] */
#define ANGULO_MAX  7.0              /* mas: desaceleracion excesiva [grados] */

/* ---------- Rangos validos para las entradas (definidos por el grupo) ---------- */
#define MASA_MIN        1000.0
#define MASA_MAX        20000.0
#define V_REINGRESO_MIN 1000.0
#define V_REINGRESO_MAX 12000.0
#define ALT_REINGRESO_MIN 50.0       /* km */
#define ALT_REINGRESO_MAX 200.0      /* km */
#define V_PARACAIDAS_MIN 10.0
#define V_PARACAIDAS_MAX 100.0
#define ALT_PARACAIDAS_MIN 1.0       /* km */
#define ALT_PARACAIDAS_MAX 5.0       /* km */
#define V_IMPACTO_MIN   1.0
#define V_IMPACTO_MAX   15.0
#define HUNDIMIENTO_MIN 0.5
#define HUNDIMIENTO_MAX 3.0
#define TIEMPO_MIN      60.0
#define TIEMPO_MAX      600.0
#define ANGULO_ING_MIN  1.0          /* angulo que se puede ingresar en el juego */
#define ANGULO_ING_MAX  15.0

/* ---------- Reglas del juego (definidas por el grupo) ---------- */
#define MAX_TURNOS        5
#define LIMITE_G          4.0        /* desaceleracion maxima segura para la tripulacion [g] */
#define ANGULO_IDEAL_MIN  6.0        /* centro de la ventana: reingreso perfecto */
#define ANGULO_IDEAL_MAX  6.5
#define REINGRESOS_PARA_EXITO 3

/* Resultado de un reingreso */
#define PERFECTO          1
#define EXITOSO           2
#define REBOTE            3          /* angulo menor a 5,5 grados */
#define ANGULO_EXCESIVO   4          /* angulo mayor a 7 grados */
#define FRENADO_EXCESIVO  5          /* mas de LIMITE_G */

/* ---------- Estructuras ---------- */

/* Datos de entrada del reingreso (los del enunciado o los del usuario) */
typedef struct {
    double masa;              /* [kg] */
    double v_reingreso;       /* [m/s] */
    double alt_reingreso;     /* [m] */
    double v_paracaidas;      /* [m/s] */
    double alt_paracaidas;    /* [m] */
    double v_impacto;         /* [m/s] */
    double hundimiento;       /* [m] */
    double tiempo_frenado;    /* [s] */
} DatosReingreso;

/* Resultados de los calculos de los incisos a, b y c */
typedef struct {
    double ec_inicial;        /* energia cinetica al reingresar [J] */
    double ec_final;          /* energia cinetica al abrir paracaidas [J] */
    double trabajo_peso;      /* trabajo del peso en la bajada [J] */
    double trabajo_roz;       /* trabajo de la fuerza de rozamiento del aire [J] */
    double desaceleracion;    /* desaceleracion media [m/s^2] */
    double desaceleracion_g;  /* desaceleracion media en multiplos de g */
    double fuerza_agua;       /* fuerza promedio del agua [N] */
    double fuerza_en_pesos;   /* fuerza del agua en multiplos del peso */
} Resultados;

/* Datos de un reingreso del juego */
typedef struct {
    int turno;
    double v_reingreso;       /* velocidad que da el control de mision [m/s] */
    double angulo;            /* angulo elegido [grados] */
    double tiempo;            /* duracion del frenado elegida [s] */
    double desaceleracion_g;  /* resultado [g] */
    double trabajo_roz;       /* energia que disipo el escudo termico [J] */
    int resultado;            /* PERFECTO, EXITOSO, REBOTE, ... */
} Reingreso;

/* Toda la informacion de la mision */
typedef struct {
    DatosReingreso datos;
    Resultados res;
    Reingreso historial[MAX_TURNOS];  /* arreglo con los reingresos jugados */
    int cant_reingresos;
} Mision;

/* ---------- Prototipos ---------- */

/* Configuracion */
void inicializar_mision(Mision *m);
void configurar_mision(Mision *m);

/* Calculos fisicos */
int calcular_resultados(DatosReingreso *d, Resultados *r);
double calcular_trabajo_rozamiento(DatosReingreso *d, double v_reingreso);
double calcular_desaceleracion_g(double v_inicial, double v_final, double tiempo);
int evaluar_reingreso(double angulo, double desaceleracion_g);

/* Pantallas */
void mostrar_datos(Mision *m);
void mostrar_analisis(Mision *m);
void jugar_turnos(Mision *m);
void mostrar_historial(Mision *m);
void mostrar_informe_final(Mision *m);

#endif
