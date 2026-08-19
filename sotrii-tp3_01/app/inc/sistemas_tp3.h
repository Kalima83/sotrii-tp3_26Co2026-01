#ifndef APP_INC_SISTEMAS_TP3_H_
#define APP_INC_SISTEMAS_TP3_H_

#include <stdint.h>

/* Prototipos exactos de los Jobs */
void T1_Job(void);
void T1_Job_Half(void);
void T2_Job(uint32_t units);
void T3_Job(uint32_t units);
void T4_Job(uint32_t units);

/* Prototipos de los ejecutores de hiperperíodos */
void run_system_1_one_hyperperiod(void);
void run_system_2_one_hyperperiod(void);
void run_system_3_one_hyperperiod(void);
void run_system_4_one_hyperperiod(void);

#endif /* APP_INC_SISTEMAS_TP3_H_ */
