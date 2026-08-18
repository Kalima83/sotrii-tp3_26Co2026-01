#ifndef APP_H_
#define APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

#define TASK_QTY 4ul

extern uint32_t g_app_cnt;
extern uint32_t g_app_task_cnt;
extern volatile uint32_t g_app_tick_cnt;
extern uint32_t g_task_idle_cnt;
extern uint32_t g_app_stack_overflow_cnt;

/* TP3-02 observability variables (watch them in the debugger). */
extern volatile uint32_t g_rm_exec_cnt[4];
extern volatile uint32_t g_rm_deadline_miss_cnt[4];
extern volatile TickType_t g_rm_last_response_ticks[4];
extern volatile TickType_t g_rm_max_response_ticks[4];
extern TaskHandle_t h_rm_task[4];

void app_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H_ */
