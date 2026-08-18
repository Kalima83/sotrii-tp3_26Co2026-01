# CESE - Sistemas Operativos de Tiempo Real II

## Trabajo Práctico N° 3 - Task Scheduling

### Actividad 02 - Rate Monotonic

Proyecto STM32F446RE + FreeRTOS, adaptado desde la plantilla `sotri-cooperative`.

La entrega de esta actividad está en Markdown:

- [`sotrii-tp3_02-rate_monolitic.md`](sotrii-tp3_02-rate_monolitic.md): datos de la entrega, planificación RM de los cuatro sistemas (prioridades, factor de uso, hiperperíodo, período secundario, test de garantía y Gantt) y configuración FreeRTOS.
- Este `README.md`: cómo usar el proyecto.

No se entregan `.docx` ni `.pdf`.

## Qué implementa

- FreeRTOS preemptivo (`configUSE_PREEMPTION = 1`)
- Prioridades fijas Rate Monotonic (menor período = mayor prioridad numérica)
- Activación periódica con `vTaskDelayUntil()`
- Sistemas 1 a 4 seleccionables desde `app/src/app.c`

## Uso rápido

1. Importar el proyecto en STM32CubeIDE.
2. Abrir `app/src/app.c`.
3. Seleccionar el sistema con `#define TP3_RM_SYSTEM 1u` (valores 1, 2, 3 o 4).
4. Compilar y programar la NUCLEO-F446RE.
5. En el depurador observar `g_rm_exec_cnt[]`, `g_rm_deadline_miss_cnt[]` y `g_rm_max_response_ticks[]`.

Escala: `1 unidad del enunciado = 20 ms`.

## GitHub

Inicializar Git en esta carpeta, crear el repositorio remoto y subir el proyecto (incluido `sotrii-tp3_02-rate_monolitic.md` y este `README.md`). El directorio `Debug/` no debe versionarse.
