# Simulador de Misiones Artemis: Centro de Control Orbital Orion

## Integrantes
- Bautista Lago Escobar
- Lucia Marlen Cardozo
- Dahiana Cano

Comisión: K1.4. Grupo Lab C N°14.

## Escenario asignado
Escenario 2: Inserción y Estabilidad en Órbita Terrestre Baja (LEO), MCU y Gravitación.

## Reglas del simulador
(pendiente)

## Funcionamiento
(pendiente)

## Arquitectura
- `main.c`: menú principal y navegación.
- `simulador.h` / `simulador.c`: constantes, estructuras, cálculos físicos, turnos e informe.
- `validaciones.h` / `validaciones.c`: ingreso seguro de datos y mensajes de error.

## Diagrama
(pendiente)

## Compilación y ejecución
```
mingw32-make      (en Linux: make)
./simulador
```

## Registro de uso de IA
| Fecha | Herramienta | Tarea | Consulta | Cómo se verificó / qué cambiamos |
|---|---|---|---|---|
| 09/11/2026 | Claude | Dudas sobre la interpretación física del inciso d (dirección del pulso lateral y validez de la aproximación d = ½·a·Δt²) | Consulta sobre qué modelo usar para la desviación lateral y por qué | Lo contrastamos con la consigna oficial y con lo visto en Física I; el modelo lo elegimos como grupo |
| 09/11/2026 | Claude | Armado del Makefile (compilar varios .c con gcc y enlazar math.h con -lm) y de la estructura base de archivos del repositorio | Consulta sobre cómo compilar un proyecto con varios archivos .c y .h | Revisamos que la estructura respete la sección 5.1 de la consigna; la compilación se verifica al cargar el código |