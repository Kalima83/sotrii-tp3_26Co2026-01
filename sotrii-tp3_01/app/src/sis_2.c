/*
 * sis_2.c
 *
 * Created on: 17 ago 2026
 * Author: danie
 */

#include "sis_2.h"
#include "sistemas_tp3.h"
#include "logger.h"

/********************** Macros y Constantes del Sistema 2 **********************/
#define TIME_UNIT_MS        100U    /* 1 U = 100 ms */
#define FRAME_UNITS         6U      /* f = 6 unidades */
#define HYPERPERIOD_FRAMES  15U     /* H / f = 90 / 6 = 15 tramas */

/********************** Funciones del Sistema 2 **********************/
void run_system_2_one_hyperperiod(void)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(FRAME_UNITS * TIME_UNIT_MS);

    for (uint16_t frame = 0; frame < HYPERPERIOD_FRAMES; frame++)
    {
        LOGGER_INFO("--- [Sys2] Frame F%u ---", frame);

        switch (frame) {
            case 0:  T1_Job(); T2_Job(2); T3_Job(2); break;
            case 1:  T1_Job(); break;
            case 2:  T1_Job(); T2_Job(2); break;
            case 3:  T1_Job(); T3_Job(2); break;
            case 4:  T1_Job(); T2_Job(2); break;
            case 5:  T1_Job(); break;
            case 6:  T1_Job(); T2_Job(2); T3_Job(2); break;
            case 7:  T1_Job(); break;
            case 8:  T1_Job(); T2_Job(2); break;
            case 9:  T1_Job(); T3_Job(2); break;
            case 10: T1_Job(); T2_Job(2); break;
            case 11: T1_Job(); break;
            case 12: T1_Job(); T2_Job(2); T3_Job(2); break;
            case 13: T1_Job(); break;
            case 14: T1_Job(); T2_Job(2); break;
            default: break;
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

    LOGGER_INFO("=== [Sys2] Hiperperiodo Completado ===");
}
