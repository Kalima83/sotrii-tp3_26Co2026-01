################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/src/app.c \
../app/src/app_it.c \
../app/src/logger.c \
../app/src/sis_1.c \
../app/src/sis_2.c \
../app/src/sis_3.c \
../app/src/sis_4.c \
../app/src/sistemas_tp3.c \
../app/src/systick.c 

OBJS += \
./app/src/app.o \
./app/src/app_it.o \
./app/src/logger.o \
./app/src/sis_1.o \
./app/src/sis_2.o \
./app/src/sis_3.o \
./app/src/sis_4.o \
./app/src/sistemas_tp3.o \
./app/src/systick.o 

C_DEPS += \
./app/src/app.d \
./app/src/app_it.d \
./app/src/logger.d \
./app/src/sis_1.d \
./app/src/sis_2.d \
./app/src/sis_3.d \
./app/src/sis_4.d \
./app/src/sistemas_tp3.d \
./app/src/systick.d 


# Each subdirectory must supply rules for building sources it contributes
app/src/%.o app/src/%.su app/src/%.cyclo: ../app/src/%.c app/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/danie/OneDrive/SE/RTOS_II/TPs/sotrii-tp3_26Co2026-01/sotrii-tp3_01/app/src" -I"C:/Users/danie/OneDrive/SE/RTOS_II/TPs/sotrii-tp3_26Co2026-01/sotrii-tp3_01/app/inc" -I"C:/Users/danie/OneDrive/SE/RTOS_II/TPs/sotrii-tp3_26Co2026-01/sotrii-tp3_01/app" -I"C:/Users/danie/OneDrive/SE/RTOS_II/TPs/sotrii-tp3_26Co2026-01/sotrii-tp3_01/Core/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-src

clean-app-2f-src:
	-$(RM) ./app/src/app.cyclo ./app/src/app.d ./app/src/app.o ./app/src/app.su ./app/src/app_it.cyclo ./app/src/app_it.d ./app/src/app_it.o ./app/src/app_it.su ./app/src/logger.cyclo ./app/src/logger.d ./app/src/logger.o ./app/src/logger.su ./app/src/sis_1.cyclo ./app/src/sis_1.d ./app/src/sis_1.o ./app/src/sis_1.su ./app/src/sis_2.cyclo ./app/src/sis_2.d ./app/src/sis_2.o ./app/src/sis_2.su ./app/src/sis_3.cyclo ./app/src/sis_3.d ./app/src/sis_3.o ./app/src/sis_3.su ./app/src/sis_4.cyclo ./app/src/sis_4.d ./app/src/sis_4.o ./app/src/sis_4.su ./app/src/sistemas_tp3.cyclo ./app/src/sistemas_tp3.d ./app/src/sistemas_tp3.o ./app/src/sistemas_tp3.su ./app/src/systick.cyclo ./app/src/systick.d ./app/src/systick.o ./app/src/systick.su

.PHONY: clean-app-2f-src

