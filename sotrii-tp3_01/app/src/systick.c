#include "main.h"
#include "dwt.h"
#include "systick.h"

void systick_delay_us(uint32_t delay_us)
{
    /* Si DWT no estaba activo, lo aseguramos */
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        cycle_counter_init();
    }

    uint32_t cycles_per_us = SystemCoreClock / 1000000UL;
    if (cycles_per_us == 0) cycles_per_us = 16; /* Fallback si SystemCoreClock no fue actualizado */

    uint32_t total_cycles = delay_us * cycles_per_us;
    uint32_t start_cycles = cycle_counter_get();

    while ((cycle_counter_get() - start_cycles) < total_cycles) {
        __NOP();
    }
}
