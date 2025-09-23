################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LCD/app/examples.c 

OBJS += \
./LCD/app/examples.o 

C_DEPS += \
./LCD/app/examples.d 


# Each subdirectory must supply rules for building sources it contributes
LCD/app/%.o LCD/app/%.su LCD/app/%.cyclo: ../LCD/app/%.c LCD/app/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/81904/STM32CubeIDE/workspace_LCD/20250913_lcd/LCD" -I"C:/Users/81904/STM32CubeIDE/workspace_LCD/20250913_lcd/LCD/app" -I"C:/Users/81904/STM32CubeIDE/workspace_LCD/20250913_lcd/LCD/if" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-LCD-2f-app

clean-LCD-2f-app:
	-$(RM) ./LCD/app/examples.cyclo ./LCD/app/examples.d ./LCD/app/examples.o ./LCD/app/examples.su

.PHONY: clean-LCD-2f-app

