################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/User/utils/sm_crc.c \
../Core/User/utils/sm_logger.c 

OBJS += \
./Core/User/utils/sm_crc.o \
./Core/User/utils/sm_logger.o 

C_DEPS += \
./Core/User/utils/sm_crc.d \
./Core/User/utils/sm_logger.d 


# Each subdirectory must supply rules for building sources it contributes
Core/User/utils/%.o Core/User/utils/%.su Core/User/utils/%.cyclo: ../Core/User/utils/%.c Core/User/utils/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xB -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"/home/lekhacvuong/STM32CubeIDE/workspace_1.14.0/smartHome/Core/User/libs/host" -I"/home/lekhacvuong/STM32CubeIDE/workspace_1.14.0/smartHome/Core/User/libs/rc522" -I"/home/lekhacvuong/STM32CubeIDE/workspace_1.14.0/smartHome/Core/User/utils" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-User-2f-utils

clean-Core-2f-User-2f-utils:
	-$(RM) ./Core/User/utils/sm_crc.cyclo ./Core/User/utils/sm_crc.d ./Core/User/utils/sm_crc.o ./Core/User/utils/sm_crc.su ./Core/User/utils/sm_logger.cyclo ./Core/User/utils/sm_logger.d ./Core/User/utils/sm_logger.o ./Core/User/utils/sm_logger.su

.PHONY: clean-Core-2f-User-2f-utils

