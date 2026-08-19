/*
 * sis_4.c
 *
 * Created on: 17 ago 2026
 * Author: danie
 */

#include "sis_4.h"
#include "sistemas_tp3.h"
#include "logger.h"

/********************** Macros y Constantes del Sistema 4 **********************/
#define TIME_UNIT_MS        100U    /* 1 U = 100 ms */
#define FRAME_UNITS         3U      /* f = 3 unidades */
#define HYPERPERIOD_FRAMES  40U     /* H / f = 120 / 3 = 40 tramas */

/********************** Funciones del Sistema 4 **********************/
void run_system_4_one_hyperperiod(void)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(FRAME_UNITS * TIME_UNIT_MS);

    for (uint16_t frame = 0; frame < HYPERPERIOD_FRAMES; frame++)
    {
        LOGGER_INFO("--- [Sys4] Frame F%u ---", frame);

        /* Usamos T1_Job_Half() para C1 = 0.5 U */
        switch (frame % 8) {
            case 0: T1_Job_Half(); T2_Job(1); break;
            case 1: T1_Job_Half(); T3_Job(2); break;
            case 2: T1_Job_Half(); T2_Job(1); break;
            case 3: T1_Job_Half(); T4_Job(2); break; /* T4 slice 1 */
            case 4: T1_Job_Half(); T2_Job(1); break;
            case 5: T1_Job_Half(); T4_Job(2); break; /* T4 slice 2 */
            case 6: T1_Job_Half(); T2_Job(1); break;
            case 7: T1_Job_Half(); T4_Job(2); break; /* T4 slice 3 */
            default: break;
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

    LOGGER_INFO("=== [Sys4] Hiperperiodo Completado ===");
}
