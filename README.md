# Simulador de Misiones Artemis: Control de Reingreso Orion

## Integrantes
- Bautista Lago Escobar
- Lucia Marlen Cardozo
- Dahiana Cano

Comisión: 1.4. Grupo: D14.
Materia: Algoritmos y Estructuras de Datos — UTN FRRe.

## Escenario asignado
**Escenario 4: Reingreso Atmosférico, Frenado Aerodinámico y Amerizaje (Trabajo, Energía e Impacto).**

Después del sobrevuelo lunar, el módulo de tripulación Orion (10.400 kg) reingresa a la atmósfera a 11 km/s (unos 39.600 km/h) a 120 km de altura. El escudo térmico disipa la energía cinética mediante el trabajo de la fuerza de rozamiento del aire, hasta que a 2,5 km la cápsula baja a 25 m/s y abre los paracaídas. Finalmente, amariza en el Océano Pacífico a 8 m/s.

El programa resuelve los cuatro incisos de la consigna con los datos del enunciado o con valores que ingresa el usuario:

| Inciso | Qué calcula | Fórmula | Valor con los datos del enunciado |
|---|---|---|---|
| A | Trabajo del rozamiento | W_roz = (Ec_f − Ec_i) − W_peso, con W_peso = m·g·(h_i − h_f) | −6,4117 × 10¹¹ J |
| B | Desaceleración media en 400 s | a = (v_i − v_f) / Δt | 27,44 m/s² = 2,80 g |
| C | Fuerza del agua (se hunde 1,5 m) | F = m·(v² / (2·d) + g) | 323.786,67 N = 3,18 veces el peso |
| D | Ángulo de reingreso | Ventana segura: 5,5° ≤ γ ≤ 7,0° | γ > 7°: desaceleración excesiva; γ < 5,5°: rebote atmosférico |

**Hipótesis:** g = 9,8 m/s². En A se aplica el teorema del trabajo y la energía, considerando el trabajo del peso durante la bajada. En C se toma la fuerza del agua como constante y se incluye el peso de la cápsula; no se considera el empuje del agua.

## Objetivo y reglas del simulador
El usuario es el responsable del control de misión y tiene que traer a la tripulación a salvo.

- La misión tiene **5 reingresos**. En cada uno, el control de misión informa una velocidad de reingreso distinta.
- El jugador elige el **ángulo de reingreso** (1° a 15°) y la **duración del frenado** (60 a 600 s).
- El programa calcula la desaceleración media en g y la energía que disipa el escudo térmico, y evalúa:
  - **Ángulo menor a 5,5°:** fallido por rebote atmosférico.
  - **Ángulo mayor a 7,0°:** fallido por desaceleración y calentamiento excesivos.
  - **Desaceleración mayor a 4 g:** fallido por frenado demasiado brusco (límite de seguridad definido por el grupo).
  - **Ángulo entre 6,0° y 6,5°:** perfecto (2 puntos). **Resto de la ventana:** exitoso (1 punto).
- **La misión es exitosa** si al menos 3 de los 5 reingresos son perfectos o exitosos. El puntaje máximo es 10.
- Cambiar la configuración reinicia el historial.

## Funcionamiento
Menú principal:
1. **Configurar misión:** usar los datos del enunciado o ingresar datos propios.
2. **Análisis del reingreso:** procedimiento de los incisos A, B, C y D con la sustitución de valores.
3. **Control de reingresos:** el juego. Se puede pausar y continuar después.
4. **Historial de reingresos:** tabla con todos los reingresos jugados.
5. **Informe final:** datos, resultados del enunciado, historial, resumen, puntaje y resultado de la misión.
6. **Salir.**

Todas las entradas se validan: letras, entradas vacías, caracteres de más (por ejemplo `12abc`), coma en lugar de punto decimal, números demasiado grandes y valores fuera de rango. Ante un error, el programa muestra un mensaje y vuelve a pedir el dato.

## Arquitectura
| Archivo | Responsabilidad |
|---|---|
| `main.c` | Función `main`, menú principal y navegación. |
| `simulador.h` | Constantes (`#define`), estructuras (`typedef struct`) y prototipos. |
| `simulador.c` | Cálculos físicos, ciclo de reingresos, historial e informe final. |
| `validaciones.h` / `validaciones.c` | Lectura segura de enteros y reales: se lee la línea completa con `fgets` y se convierte con `sscanf`, como en *Intro a C* (Marzal y Gracia). |
| `Makefile` | Compilación del proyecto. |

**Estructuras utilizadas**
- `DatosReingreso`: masa, velocidades, altitudes, hundimiento y duración del frenado.
- `Resultados`: energías, trabajos, desaceleración y fuerza del agua.
- `Reingreso`: datos y resultado de un turno del juego.
- `Mision`: los datos, los resultados, un **arreglo** `historial[5]` de reingresos y la cantidad jugada.

Las funciones que modifican la misión reciben un **puntero** (`Mision *m`, `DatosReingreso *d`, `Resultados *r`), así trabajan sobre el dato original y no sobre una copia. Las velocidades de cada turno se guardan en un arreglo.

## Diagrama general
```
                 +------------------+
                 |      INICIO      |
                 | carga los datos  |
                 | del enunciado    |
                 +--------+---------+
                          |
                          v
            +-------------+--------------+
            |       MENU PRINCIPAL       |<---------------+
            +-------------+--------------+                |
                          |                               |
   +-------+--------+-----+------+----------+--------+    |
   |       |        |            |          |        |    |
   v       v        v            v          v        v    |
 Config. Analisis Reingresos Historial  Informe   Salir   |
   |    (a,b,c,d)   |            |          |        |    |
   |       |        v            |          |        v    |
   |       |   +----+--------+   |          |      FIN    |
   |       |   | velocidad    |  |          |             |
   |       |   | lee angulo   |  |          |             |
   |       |   | y tiempo     |  |          |             |
   |       |   | calcula g    |  |          |             |
   |       |   | evalua       |  |          |             |
   |       |   | guarda en    |  |          |             |
   |       |   | historial[]  |  |          |             |
   |       |   +----+---------+  |          |             |
   |       |        | (5 turnos o pausa)    |             |
   +-------+--------+------------+----------+-------------+
```

## Compilación y ejecución
Con el Makefile:
```
mingw32-make      (en Windows con MinGW)
make              (en Linux)
```
O directamente con gcc:
```
gcc -Wall -Wextra main.c simulador.c validaciones.c -o simulador -lm
```
Ejecución:
```
./simulador
```

## Pruebas realizadas
- Datos del enunciado: los resultados coinciden con el cálculo manual (W_roz ≈ −6,41 × 10¹¹ J, a ≈ 2,80 g, F ≈ 323.787 N ≈ 3,18 pesos).
- Datos propios (m = 10.400 kg, v_i = 9.000 m/s, h_i = 80 km, v_f = 30 m/s, h_f = 2 km, v = 6 m/s, d = 1,2 m, Δt = 300 s): W_roz = −4,2915 × 10¹¹ J, comprobado a mano.
- Juego: los cinco resultados posibles (perfecto, exitoso, rebote, ángulo excesivo y frenado excesivo), pausa y continuación, historial, informe final y una misión completa con 10/10.
- Validaciones: letras, vacío, `2abc`, opciones inexistentes, fuera de rango, `0,5`, `nan`, números enormes y líneas de 150 caracteres.
- Compilación con `-Wall -Wextra` sin advertencias.

## Registro de uso de IA
| Fecha | Herramienta | Tarea | Consulta | Cómo se verificó / qué cambiamos |
|---|---|---|---|---|
| 09/11/2026 | Claude | Armado del Makefile (compilar varios .c con gcc) y de la estructura base de archivos del repositorio | Consulta sobre cómo compilar un proyecto con varios archivos .c y .h | Revisamos que la estructura respete la sección 5.1 de la consigna |
| 10/11/2026 | Claude | Primera versión del código de los módulos, a partir del diseño que definimos como grupo | Pedido de implementar el diseño respetando el nivel de la materia y el material de la cátedra | Compilamos sin advertencias y probamos las validaciones con entradas incorrectas |
| 11/11/2026 | Claude | Al revisar la tabla de asignación notamos que habíamos trabajado con el escenario equivocado; adaptamos `simulador.c/.h` al escenario 4 (las validaciones y la estructura se mantuvieron) | Consulta sobre el planteo físico de los incisos (teorema trabajo-energía, desaceleración media y fuerza del agua) y pedido de adaptar el simulador | Comparamos los resultados con el cálculo manual de los cuatro incisos y probamos todos los resultados posibles del juego. Cambios del grupo: (completar) |
