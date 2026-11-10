/*
 * simulador.h
 * Simulador de Misiones Artemis - Escenario 2: Orion en orbita LEO (MCU y Gravitacion)
 * Grupo Lab C N 14 - Algoritmos y Estructuras de Datos - UTN FRRe
 *
 * Contiene las constantes, las estructuras y los prototipos del simulador.
 */

#ifndef SIMULADOR_H
#define SIMULADOR_H

/* ---------- Constantes fisicas (datos de la catedra, unidades SI) ---------- */
#define G             6.674e-11      /* constante gravitacional [N m^2 / kg^2] */
#define MASA_TIERRA   5.972e24       /* masa de la Tierra [kg] */
#define RADIO_TIERRA  6371000.0      /* radio terrestre medio [m] */
#define PI            3.14159265358979

/* ---------- Valores de referencia del enunciado ---------- */
#define ALTITUD_REF_KM  192.0        /* altitud orbital inicial [km] */
#define MASA_REF        33000.0      /* masa de Orion [kg] */
#define ACEL_REF        0.50         /* aceleracion del propulsor lateral [m/s^2] */
#define TIEMPO_REF      10.0         /* duracion del pulso lateral [s] */

/* ---------- Rangos validos para las entradas (definidos por el grupo) ---------- */
#define ALTITUD_MIN_KM  160.0
#define ALTITUD_MAX_KM  2000.0
#define MASA_MIN        1000.0
#define MASA_MAX        100000.0
#define ACEL_MIN        0.01
#define ACEL_MAX        0.50         /* el propulsor no puede superar 0,50 m/s^2 */
#define TIEMPO_MIN      1.0
#define TIEMPO_MAX      30.0         /* pulsos cortos: la aproximacion sigue siendo valida */

/* ---------- Reglas del juego ---------- */
#define MAX_TURNOS          5
#define TOLERANCIA_PRECISA   10.0    /* error maximo [%] para una maniobra precisa */
#define TOLERANCIA_ACEPTABLE 25.0    /* error maximo [%] para una maniobra aceptable */
#define MANIOBRAS_PARA_EXITO 3       /* maniobras buenas necesarias para ganar */

/* Resultado de una maniobra */
#define PRECISA    1
#define ACEPTABLE  2
#define FALLIDA    3

/* ---------- Estructuras ---------- */

/* Estado de la orbita de Orion */
typedef struct {
    double altitud;              /* [m] */
    double masa;                 /* [kg] */
    double radio;                /* r = RT + h [m] */
    double velocidad;            /* [m/s] */
    double periodo;              /* [s] */
    double acel_centripeta;      /* [m/s^2] */
    double fuerza_gravitatoria;  /* [N] */
} EstadoOrbital;

/* Datos de una maniobra con el propulsor lateral */
typedef struct {
    int turno;
    double objetivo;             /* desvio pedido por el control de tierra [m] */
    double aceleracion;          /* [m/s^2] */
    double tiempo;               /* [s] */
    double delta_v;              /* cambio de velocidad lateral [m/s] */
    double desvio;               /* desvio lateral obtenido [m] */
    double error_porcentual;     /* diferencia con el objetivo [%] */
    int resultado;               /* PRECISA, ACEPTABLE o FALLIDA */
} Maniobra;

/* Toda la informacion de la mision */
typedef struct {
    EstadoOrbital orbita;
    Maniobra historial[MAX_TURNOS];   /* arreglo con las maniobras realizadas */
    int cant_maniobras;
} Mision;

/* ---------- Prototipos ---------- */

/* Configuracion */
void inicializar_mision(Mision *m);
void configurar_mision(Mision *m);

/* Calculos fisicos */
int calcular_orbita(EstadoOrbital *e);
double calcular_delta_v(double aceleracion, double tiempo);
double calcular_desvio(double aceleracion, double tiempo);
double calcular_error(double desvio, double objetivo);
int clasificar_maniobra(double error);

/* Pantallas */
void mostrar_telemetria(Mision *m);
void mostrar_analisis_orbital(Mision *m);
void jugar_turnos(Mision *m);
void mostrar_historial(Mision *m);
void mostrar_informe_final(Mision *m);

#endif
