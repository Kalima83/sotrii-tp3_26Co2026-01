/*
 * SOTR II - TP3 - Activity 02 - Rate Monotonic Scheduling
 * Adapted from the supplied sotri-cooperative template.
 *
 * The RM policy is implemented with:
 *   - configUSE_PREEMPTION = 1
 *   - fixed priorities inverse to each task period
 *   - periodic releases using vTaskDelayUntil()
 */
#include "main.h"
#include "cmsis_os.h"

#include "logger.h"
#include "dwt.h"
#include "board.h"
#include "app_it.h"
#include "app.h"

/* Select the TP system to execute: 1, 2, 3 or 4. */
#define TP3_RM_SYSTEM           1u

/* One abstract time unit from the statement is mapped to 20 ms.
 * All C/T/D ratios are preserved. System 4 has C1=0.5, represented as 10 ms. */
#define TP3_TIME_UNIT_MS        20u
#define TP3_TIME_UNIT_US        (TP3_TIME_UNIT_MS * 1000u)

/* C is stored in half-units to represent C=0.5 without floating point. */
typedef struct
{
    const char *name;
    uint8_t task_index;
    uint8_t c_half_units;
    uint16_t period_units;
    UBaseType_t priority;
} rm_task_param_t;

uint32_t g_app_cnt = 0ul;
uint32_t g_app_task_cnt = 0ul;
volatile uint32_t g_app_tick_cnt = 0ul;
uint32_t g_task_idle_cnt = 0ul;
uint32_t g_app_stack_overflow_cnt = 0ul;

volatile uint32_t g_rm_exec_cnt[4] = {0ul, 0ul, 0ul, 0ul};
volatile uint32_t g_rm_deadline_miss_cnt[4] = {0ul, 0ul, 0ul, 0ul};
volatile TickType_t g_rm_last_response_ticks[4] = {0, 0, 0, 0};
volatile TickType_t g_rm_max_response_ticks[4] = {0, 0, 0, 0};
TaskHandle_t h_rm_task[4] = {NULL, NULL, NULL, NULL};

/* Lower period => higher fixed priority (Rate Monotonic). */
#if (TP3_RM_SYSTEM == 1u)
static const rm_task_param_t g_rm_plan[] = {
    {"T1", 0u,  2u,  4u, tskIDLE_PRIORITY + 3u}, /* C=1, T=4  */
    {"T2", 1u,  4u,  5u, tskIDLE_PRIORITY + 2u}, /* C=2, T=5  */
    {"T3", 2u, 10u, 20u, tskIDLE_PRIORITY + 1u}  /* C=5, T=20 */
};
#elif (TP3_RM_SYSTEM == 2u)
static const rm_task_param_t g_rm_plan[] = {
    {"T1", 0u, 2u,  6u, tskIDLE_PRIORITY + 3u},
    {"T2", 1u, 4u, 10u, tskIDLE_PRIORITY + 2u},
    {"T3", 2u, 4u, 18u, tskIDLE_PRIORITY + 1u}
};
#elif (TP3_RM_SYSTEM == 3u)
static const rm_task_param_t g_rm_plan[] = {
    {"T1", 0u,  2u,  8u, tskIDLE_PRIORITY + 4u},
    {"T2", 1u,  6u, 15u, tskIDLE_PRIORITY + 3u},
    {"T3", 2u,  8u, 20u, tskIDLE_PRIORITY + 2u},
    {"T4", 3u, 12u, 22u, tskIDLE_PRIORITY + 1u}
};
#elif (TP3_RM_SYSTEM == 4u)
static const rm_task_param_t g_rm_plan[] = {
    {"T1", 0u,  1u,  4u, tskIDLE_PRIORITY + 4u}, /* C=0.5 */
    {"T2", 1u,  2u,  5u, tskIDLE_PRIORITY + 3u},
    {"T3", 2u,  4u, 10u, tskIDLE_PRIORITY + 2u},
    {"T4", 3u, 18u, 24u, tskIDLE_PRIORITY + 1u}
};
#else
#error "TP3_RM_SYSTEM must be 1, 2, 3 or 4"
#endif

#define RM_TASK_COUNT ((uint32_t)(sizeof(g_rm_plan) / sizeof(g_rm_plan[0])))

/* Busy work based on the DWT cycle counter. Interrupts remain enabled, so a
 * higher-priority RM task can preempt a lower-priority task during this loop. */
static void rm_execute_wcet(uint32_t duration_us)
{
    const uint32_t cycles_per_us = SystemCoreClock / 1000000u;
    const uint32_t required_cycles = duration_us * cycles_per_us;
    const uint32_t start = cycle_counter_get();

    while ((uint32_t)(cycle_counter_get() - start) < required_cycles)
    {
        __NOP();
    }
}

static void rm_task(void *argument)
{
    const rm_task_param_t *p = (const rm_task_param_t *)argument;
    const TickType_t period_ticks = pdMS_TO_TICKS((uint32_t)p->period_units * TP3_TIME_UNIT_MS);
    const uint32_t wcet_us = ((uint32_t)p->c_half_units * TP3_TIME_UNIT_US) / 2u;
    TickType_t last_wake = xTaskGetTickCount();

    for (;;)
    {
        const TickType_t release_tick = last_wake;

        rm_execute_wcet(wcet_us);

        const TickType_t finish_tick = xTaskGetTickCount();
        const TickType_t response = finish_tick - release_tick;
        const uint8_t i = p->task_index;

        g_rm_exec_cnt[i]++;
        g_rm_last_response_ticks[i] = response;
        if (response > g_rm_max_response_ticks[i])
        {
            g_rm_max_response_ticks[i] = response;
        }
        if (response > period_ticks) /* D = T in the TP statement. */
        {
            g_rm_deadline_miss_cnt[i]++;
        }

        vTaskDelayUntil(&last_wake, period_ticks);
    }
}

void app_init(void)
{
    BaseType_t ret;

    app_it_init();
    cycle_counter_init();

    LOGGER_INFO(" ");
    LOGGER_INFO("TP3-02 Rate Monotonic - System %u", (unsigned)TP3_RM_SYSTEM);
    LOGGER_INFO("FreeRTOS preemptive: configUSE_PREEMPTION=1");
    LOGGER_INFO("Scale: 1 TP unit = %u ms", (unsigned)TP3_TIME_UNIT_MS);

    for (uint32_t i = 0u; i < RM_TASK_COUNT; i++)
    {
        ret = xTaskCreate(rm_task,
                          g_rm_plan[i].name,
                          (configMINIMAL_STACK_SIZE + 128u),
                          (void *)&g_rm_plan[i],
                          g_rm_plan[i].priority,
                          &h_rm_task[i]);
        configASSERT(pdPASS == ret);
    }
}
