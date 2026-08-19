/*
 * sis_1.c
 *
 *  Created on: 17 ago 2026
 *      Author: danie
 */

#include "sis_1.h"
#include "sistemas_tp3.h"
#include "logger.h"

/********************** Macros y Constantes del Sistema 1 **********************/
#define TIME_UNIT_MS        100U    /* 1 Unidad de tiempo = 100 ms */
#define FRAME_UNITS         2U      /* Tamaño de trama f = 2 U */
#define HYPERPERIOD_FRAMES  10U     /* H / f = 20 / 2 = 10 tramas por hiperperíodo */

/********************** Funciones del Sistema 1 **********************/
void run_system_1_one_hyperperiod(void)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(FRAME_UNITS * TIME_UNIT_MS);

    for (uint16_t frame = 0; frame < HYPERPERIOD_FRAMES; frame++)
    {
        LOGGER_INFO("--- [Sys1] Frame F%u ---", frame);

        switch (frame) {
            case 0: T1_Job(); T2_Job(2); break;
            case 1: T1_Job(); T3_Job(5); break;
            case 2: T1_Job(); T2_Job(2); break;
            case 3: T1_Job(); break;
            case 4: T1_Job(); T2_Job(2); break;
            case 5: T1_Job(); break;
            case 6: T1_Job(); T2_Job(2); break;
            case 7: T1_Job(); break;
            case 8: T1_Job(); T2_Job(2); break;
            case 9: T1_Job(); break;
            default: break;
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

    LOGGER_INFO("=== [Sys1] Hiperperiodo Completado ===");
}
