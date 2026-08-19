#include "sistemas_tp3.h"
#include "systick.h"
#include "logger.h"
#include "cmsis_os.h"

/* 1 unidad de tiempo C = 100 ms (100000 us) */
#define TIME_UNIT_US 1000UL

void T1_Job(void) {
    LOGGER_INFO("  T1 Ejecutando (1 u)");
    systick_delay_us(1 * TIME_UNIT_US);
}

/* Función especial para el Sistema 4 donde T1 tiene C=0,5 (50 ms) */
void T1_Job_Half(void) {
    LOGGER_INFO("  T1 Ejecutando (0.5 u)");
    systick_delay_us(TIME_UNIT_US / 2);
}

void T2_Job(uint32_t units) {
    LOGGER_INFO("  T2 Ejecutando (%lu u)", units);
    systick_delay_us(units * TIME_UNIT_US);
}

void T3_Job(uint32_t units) {
    LOGGER_INFO("  T3 Ejecutando (%lu u)", units);
    systick_delay_us(units * TIME_UNIT_US);
}

void T4_Job(uint32_t units) {
    LOGGER_INFO("  T4 Ejecutando (%lu u)", units);
    systick_delay_us(units * TIME_UNIT_US);
}
