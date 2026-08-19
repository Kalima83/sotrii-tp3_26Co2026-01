/*
 * sis_3.c
 *
 * Created on: 17 ago 2026
 * Author: danie
 */

#include "sis_3.h"
#include "sistemas_tp3.h"
#include "logger.h"

/********************** Macros y Constantes del Sistema 3 **********************/
#define TIME_UNIT_MS        100U    /* 1 U = 100 ms */
#define FRAME_UNITS         8U      /* f = 8 unidades */
#define HYPERPERIOD_FRAMES  165U    /* H / f = 1320 / 8 = 165 tramas */

/********************** Funciones del Sistema 3 **********************/
void run_system_3_one_hyperperiod(void)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(FRAME_UNITS * TIME_UNIT_MS);

    for (uint16_t frame = 0; frame < HYPERPERIOD_FRAMES; frame++)
    {
        LOGGER_INFO("--- [Sys3] Frame F%u ---", frame);

        /* Patrón repetitivo cada 3 tramas */
        switch (frame % 3) {
            case 0: T1_Job(); T2_Job(3); T3_Job(4); break; /* 1 + 3 + 4 = 8 U */
            case 1: T1_Job(); T2_Job(3); T4_Job(4); break; /* T4 fragmentada: 4 U */
            case 2: T1_Job(); T4_Job(2); break;            /* T4 fragmentada: 2 U restantes */
            default: break;
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

    LOGGER_INFO("=== [Sys3] Hiperperiodo Completado ===");
}
