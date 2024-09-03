################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/User/libs/rc522/rc522.c 

OBJS += \
./Core/User/libs/rc522/rc522.o 

C_DEPS += \
./Core/User/libs/rc522/rc522.d 


# Each subdirectory must supply rules for building sources it contributes
Core/User/libs/rc522/%.o Core/User/libs/rc522/%.su Core/User/libs/rc522/%.cyclo: ../Core/User/libs/rc522/%.c Core/User/libs/rc522/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xB -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"/home/lekhacvuong/STM32CubeIDE/workspace_1.14.0/smartHome/Core/User/libs/host" -I"/home/lekhacvuong/STM32CubeIDE/workspace_1.14.0/smartHome/Core/User/libs/rc522" -I"/home/lekhacvuong/STM32CubeIDE/workspace_1.14.0/smartHome/Core/User/utils" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-User-2f-libs-2f-rc522

clean-Core-2f-User-2f-libs-2f-rc522:
	-$(RM) ./Core/User/libs/rc522/rc522.cyclo ./Core/User/libs/rc522/rc522.d ./Core/User/libs/rc522/rc522.o ./Core/User/libs/rc522/rc522.su

.PHONY: clean-Core-2f-User-2f-libs-2f-rc522

