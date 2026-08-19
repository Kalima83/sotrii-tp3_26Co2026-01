/*
 * app.c
 *
 *  Created on: 17 ago 2026
 *      Author: danie
 */

/********************** inclusions *******************************************/
#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"
#include "app.h"
#include "systick.h"
#include "sistemas_tp3.h"

/********************** external data declaration ****************************/
TaskHandle_t h_benchmark_task = NULL;

/********************** code *************************************************/

/**
 * @brief Tarea orquestadora que ejecuta los 4 sistemas en secuencia.
 * @param parameters No utilizado.
 */
void benchmark_runner_task(void *parameters)
{
    (void)parameters;

    LOGGER_INFO("==================================================");
    LOGGER_INFO("  INICIANDO EVALUACION SECUENCIAL AUTOMATICA (TP3)");
    LOGGER_INFO("==================================================");

    /* ----------------------------------------------------
     * ETAPA 1: Sistema 1 (H = 20 U, f = 2 U, 10 frames)
     * ---------------------------------------------------- */
    LOGGER_INFO("\r\n>>> 1/4. Ejecutando Sistema 1 (H=20, f=2) <<<");
    run_system_1_one_hyperperiod();

    /* ----------------------------------------------------
     * ETAPA 2: Sistema 2 (H = 90 U, f = 6 U, 15 frames)
     * ---------------------------------------------------- */
    LOGGER_INFO("\r\n>>> 2/4. Ejecutando Sistema 2 (H=90, f=6) <<<");
    run_system_2_one_hyperperiod();

    /* ----------------------------------------------------
     * ETAPA 3: Sistema 3 (H = 1320 U, f = 8 U, 165 frames)
     * ---------------------------------------------------- */
    LOGGER_INFO("\r\n>>> 3/4. Ejecutando Sistema 3 (H=1320, f=8) <<<");
    run_system_3_one_hyperperiod();

    /* ----------------------------------------------------
     * ETAPA 4: Sistema 4 (H = 120 U, f = 3 U, 40 frames)
     * ---------------------------------------------------- */
    LOGGER_INFO("\r\n>>> 4/4. Ejecutando Sistema 4 (H=120, f=3) <<<");
    run_system_4_one_hyperperiod();

    LOGGER_INFO("\r\n==================================================");
    LOGGER_INFO("  PRUEBAS FINALIZADAS CON EXITO EN TODOS LOS SISTEMAS");
    LOGGER_INFO("==================================================");

    /* Elimina la tarea del benchmark al finalizar */
    vTaskDelete(NULL);
}

/**
 * @brief Inicialización global de la aplicación y arranque de la tarea de prueba.
 */
void app_init(void)
{
    /* Habilita el contador de ciclos del núcleo Cortex-M via DWT */
    cycle_counter_init();

    LOGGER_INFO("Inicializando App - Test Runner de Sistemas de Tiempo Real");

    /* Crea la tarea orquestadora secuencial en FreeRTOS */
    xTaskCreate(
        benchmark_runner_task,
        "Benchmark",
        configMINIMAL_STACK_SIZE * 4,
        NULL,
        (tskIDLE_PRIORITY + 1ul),
        &h_benchmark_task
    );
}
