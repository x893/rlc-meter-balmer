################################################################################
# Automatically-generated file. Do not edit!
################################################################################


DEPS += \
	output/CMSIS/startup_stm32f30x.d \
	output/base/src/hw_config.d \
	output/base/src/hw_pcd8544.d \
	output/base/src/main.d \
	output/base/src/pcd8544.d \
	output/base/src/stm32f30x_it.d \
	output/base/src/system_stm32f30x.d \
	output/base/src/vg_pcd8544.d \
	output/base/STM32F3_Discovery/stm32f3_discovery.d \
	output/base/STM32F3_Discovery/stm32f3_discovery_l3gd20.d \
	output/base/STM32F3_Discovery/stm32f3_discovery_lsm303dlhc.d \
	output/StdPeriph_Driver/stm32f30x_adc.d \
	output/StdPeriph_Driver/stm32f30x_dac.d \
	output/StdPeriph_Driver/stm32f30x_dma.d \
	output/StdPeriph_Driver/stm32f30x_exti.d \
	output/StdPeriph_Driver/stm32f30x_gpio.d \
	output/StdPeriph_Driver/stm32f30x_i2c.d \
	output/StdPeriph_Driver/stm32f30x_misc.d \
	output/StdPeriph_Driver/stm32f30x_rcc.d \
	output/StdPeriph_Driver/stm32f30x_spi.d \
	output/StdPeriph_Driver/stm32f30x_syscfg.d \
	output/StdPeriph_Driver/stm32f30x_tim.d \


OBJS += \
	output/CMSIS/startup_stm32f30x.o \
	output/base/src/hw_config.o \
	output/base/src/hw_pcd8544.o \
	output/base/src/main.o \
	output/base/src/pcd8544.o \
	output/base/src/stm32f30x_it.o \
	output/base/src/system_stm32f30x.o \
	output/base/src/vg_pcd8544.o \
	output/base/STM32F3_Discovery/stm32f3_discovery.o \
	output/base/STM32F3_Discovery/stm32f3_discovery_l3gd20.o \
	output/base/STM32F3_Discovery/stm32f3_discovery_lsm303dlhc.o \
	output/StdPeriph_Driver/stm32f30x_adc.o \
	output/StdPeriph_Driver/stm32f30x_dac.o \
	output/StdPeriph_Driver/stm32f30x_dma.o \
	output/StdPeriph_Driver/stm32f30x_exti.o \
	output/StdPeriph_Driver/stm32f30x_gpio.o \
	output/StdPeriph_Driver/stm32f30x_i2c.o \
	output/StdPeriph_Driver/stm32f30x_misc.o \
	output/StdPeriph_Driver/stm32f30x_rcc.o \
	output/StdPeriph_Driver/stm32f30x_spi.o \
	output/StdPeriph_Driver/stm32f30x_syscfg.o \
	output/StdPeriph_Driver/stm32f30x_tim.o \


output/CMSIS/startup_stm32f30x.o: ../3code/Libraries/CMSIS/Device/ST/STM32F30x/Source/Templates/gcc_ride7/startup_stm32f30x.S
	@echo 'Building target: startup_stm32f30x.S'
	@$(CC) $(ASM_FLAGS) -o "$@" "$<"

output/base/src/hw_config.o: ./src/hw_config.c
	@echo 'Building target: hw_config.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/hw_pcd8544.o: ./src/hw_pcd8544.c
	@echo 'Building target: hw_pcd8544.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/main.o: ./src/main.c
	@echo 'Building target: main.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/pcd8544.o: ./src/pcd8544.c
	@echo 'Building target: pcd8544.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/stm32f30x_it.o: ./src/stm32f30x_it.c
	@echo 'Building target: stm32f30x_it.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/system_stm32f30x.o: ./src/system_stm32f30x.c
	@echo 'Building target: system_stm32f30x.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/vg_pcd8544.o: ./src/vg_pcd8544.c
	@echo 'Building target: vg_pcd8544.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/STM32F3_Discovery/stm32f3_discovery.o: ./STM32F3_Discovery/stm32f3_discovery.c
	@echo 'Building target: stm32f3_discovery.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/STM32F3_Discovery/stm32f3_discovery_l3gd20.o: ./STM32F3_Discovery/stm32f3_discovery_l3gd20.c
	@echo 'Building target: stm32f3_discovery_l3gd20.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/STM32F3_Discovery/stm32f3_discovery_lsm303dlhc.o: ./STM32F3_Discovery/stm32f3_discovery_lsm303dlhc.c
	@echo 'Building target: stm32f3_discovery_lsm303dlhc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_adc.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_adc.c
	@echo 'Building target: stm32f30x_adc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_dac.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dac.c
	@echo 'Building target: stm32f30x_dac.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_dma.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dma.c
	@echo 'Building target: stm32f30x_dma.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_exti.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_exti.c
	@echo 'Building target: stm32f30x_exti.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_gpio.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_gpio.c
	@echo 'Building target: stm32f30x_gpio.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_i2c.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_i2c.c
	@echo 'Building target: stm32f30x_i2c.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_misc.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_misc.c
	@echo 'Building target: stm32f30x_misc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_rcc.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rcc.c
	@echo 'Building target: stm32f30x_rcc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_spi.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_spi.c
	@echo 'Building target: stm32f30x_spi.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_syscfg.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_syscfg.c
	@echo 'Building target: stm32f30x_syscfg.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_tim.o: ../3code/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_tim.c
	@echo 'Building target: stm32f30x_tim.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"



