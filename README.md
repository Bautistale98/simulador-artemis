# Simulador de Misiones Artemis: Centro de Control Orbital Orion

## Integrantes
- Bautista Lago Escobar
- Lucia Marlen Cardozo
- Dahiana Cano

Comisión: K1.4. Grupo Lab C N°14.
Materia: Algoritmos y Estructuras de Datos — UTN FRRe.

## Escenario asignado
**Escenario 2: Inserción y Estabilidad en Órbita Terrestre Baja (LEO), MCU y Gravitación.**

Después de apagar los motores de ascenso, la nave Orion queda en una órbita de estacionamiento a 192 km de altura. Ahí realiza un Movimiento Circular Uniforme (MCU): la única fuerza que actúa es la gravedad de la Tierra, que cumple el papel de fuerza centrípeta. Al igualar ambas fuerzas se cancela la masa de la nave y se obtiene la velocidad orbital:

```
G·MT·m / r² = m·v² / r   →   v = √(G·MT / r)        con r = RT + h
```

El programa resuelve los cuatro incisos de la consigna con los datos del enunciado o con valores que ingresa el usuario:

| Inciso | Qué calcula | Fórmula | Valor de referencia |
|---|---|---|---|
| A | Velocidad orbital y fuerza | v = √(G·MT / r), Fg = G·MT·m / r² | 7792.95 m/s, 305362.10 N |
| B | Período | T = 2·π·r / v | 5291.52 s (88.19 min) |
| C | Aceleración centrípeta | ac = v² / r | 9.2534 m/s² |
| D | Pulso lateral | Δv = a·Δt, d = ½·a·Δt² | 5 m/s, 25 m |

**Hipótesis del inciso D:** tomamos la aceleración del propulsor constante, en una dirección fija perpendicular a la velocidad inicial y con velocidad lateral inicial nula. Es una aproximación local, válida porque 10 s es solo el 0.19 % del período: en ese tiempo la dirección de la velocidad casi no cambia. La trayectoria se desvía levemente y la órbita deja de ser un círculo perfecto, pero el programa no simula la órbita nueva.

## Objetivo y reglas del simulador
El usuario es el responsable del Centro de Control de la misión y tiene que mantener a Orion estable usando el propulsor lateral.

- La misión dura **5 turnos**. En cada uno, el control de tierra pide desviar la nave una cierta distancia lateral.
- El jugador elige la **aceleración** del propulsor (0.01 a 0.50 m/s², el máximo del enunciado) y la **duración del pulso** (1 a 30 s).
- El programa calcula el desvío con d = ½·a·Δt² y lo compara con el pedido:
  - **Precisa:** error menor o igual al 10 % (2 puntos).
  - **Aceptable:** error menor o igual al 25 % (1 punto).
  - **Fallida:** error mayor al 25 % (0 puntos).
- **La misión es exitosa** si al menos 3 de las 5 maniobras son precisas o aceptables. El puntaje máximo es 10.
- La altitud se puede configurar entre 160 y 2000 km (rango de órbita baja) y la masa entre 1000 y 100000 kg. Cambiar la configuración reinicia el historial.

## Funcionamiento
Menú principal:
1. **Configurar misión:** usar los valores del enunciado o ingresar altitud y masa.
2. **Análisis orbital:** muestra el procedimiento de los incisos A, B, C y D, con la sustitución de valores.
3. **Supervisión por turnos:** el juego. Se puede pausar y continuar después.
4. **Historial de maniobras:** tabla con todas las maniobras realizadas.
5. **Informe final:** telemetría, historial, resumen, puntaje y resultado de la misión.
6. **Salir.**

Todas las entradas se validan: letras, entradas vacías, caracteres de más (por ejemplo `12abc`), coma en lugar de punto decimal, números demasiado grandes y valores fuera de rango. Ante un error, el programa muestra un mensaje y vuelve a pedir el dato.

## Arquitectura
| Archivo | Responsabilidad |
|---|---|
| `main.c` | Función `main`, menú principal y navegación. |
| `simulador.h` | Constantes (`#define`), estructuras (`typedef struct`) y prototipos. |
| `simulador.c` | Cálculos físicos, ciclo de turnos, historial e informe final. |
| `validaciones.h` / `validaciones.c` | Lectura segura de enteros y reales: se lee la línea completa con `fgets` y se convierte con `sscanf`, como en *Intro a C* (Marzal y Gracia). |
| `Makefile` | Compilación del proyecto. |

**Estructuras utilizadas**
- `EstadoOrbital`: altitud, masa, radio, velocidad, período, aceleración centrípeta y fuerza.
- `Maniobra`: datos y resultado de un pulso del propulsor.
- `Mision`: una órbita, un **arreglo** `historial[5]` de maniobras y la cantidad realizada.

Las funciones que modifican la misión reciben un **puntero** (`Mision *m`, `EstadoOrbital *e`), así trabajan sobre el dato original y no sobre una copia. Los desvíos pedidos en cada turno se guardan en un arreglo de constantes.

## Diagrama general
```
                 +------------------+
                 |      INICIO      |
                 | carga valores    |
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
 Config. Analisis Turnos     Historial  Informe   Salir   |
   |    (a,b,c,d)   |            |          |        |    |
   |       |        v            |          |        v    |
   |       |   +----+-------+    |          |      FIN    |
   |       |   | pide desvio |   |          |             |
   |       |   | lee a y t   |   |          |             |
   |       |   | calcula d   |   |          |             |
   |       |   | clasifica   |   |          |             |
   |       |   | guarda en   |   |          |             |
   |       |   | historial[] |   |          |             |
   |       |   +----+--------+   |          |             |
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
El `-lm` es necesario para usar `sqrt` de `math.h`.

## Pruebas realizadas
- Valores del enunciado: los resultados coinciden con el cálculo manual (v ≈ 7793 m/s, T ≈ 88.2 min, ac ≈ 9.25 m/s², d = 25 m).
- Otra configuración (h = 500 km, m = 40000 kg): v = 7616.28 m/s, T = 94.47 min.
- Misión completa de 5 turnos, pausa y continuación, historial e informe final.
- Validaciones: letras, vacío, espacios, `2abc`, `1.5` en una opción entera, negativos, cero, fuera de rango, `0,5`, `nan`, `inf`, `1e999`, números enormes y líneas de 150 caracteres.
- Compilación con `-Wall -Wextra` sin advertencias.

## Registro de uso de IA
| Fecha | Herramienta | Tarea | Consulta | Cómo se verificó / qué cambiamos |
|---|---|---|---|---|
| 09/11/2026 | Claude | Dudas sobre la interpretación física del inciso d (dirección del pulso lateral y validez de la aproximación d = ½·a·Δt²) | Consulta sobre qué modelo usar para la desviación lateral y por qué | Lo contrastamos con la consigna oficial y con lo visto en Física I; el modelo lo elegimos como grupo |
| 09/11/2026 | Claude | Armado del Makefile (compilar varios .c con gcc y enlazar math.h con -lm) y de la estructura base de archivos del repositorio | Consulta sobre cómo compilar un proyecto con varios archivos .c y .h | Revisamos que la estructura respete la sección 5.1 de la consigna |
| 10/11/2026 | Claude | Primera versión del código de los módulos, a partir del diseño que definimos como grupo (reglas del juego, rangos, tolerancias y estructuras) | Pedido de implementar el diseño respetando el nivel de la materia y el material de la cátedra | Compilamos sin advertencias, comparamos los resultados con el cálculo manual y probamos las validaciones con entradas incorrectas. Cambios del grupo: (completar) |
