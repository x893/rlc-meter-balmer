################################################################################
# Automatically-generated file. Do not edit!
################################################################################


DEPS += \
	output/CMSIS/startup_stm32f30x.d \
	output/base/src/main.d \
	output/base/src/system_stm32f30x.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_adc.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_can.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_comp.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_crc.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dac.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dbgmcu.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dma.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_exti.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_flash.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_gpio.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_i2c.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_iwdg.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_misc.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_opamp.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_pwr.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rcc.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rtc.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_spi.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_syscfg.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_tim.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_usart.d \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_wwdg.d \


OBJS += \
	output/CMSIS/startup_stm32f30x.o \
	output/base/src/main.o \
	output/base/src/system_stm32f30x.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_adc.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_can.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_comp.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_crc.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dac.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dbgmcu.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dma.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_exti.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_flash.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_gpio.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_i2c.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_iwdg.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_misc.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_opamp.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_pwr.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rcc.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rtc.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_spi.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_syscfg.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_tim.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_usart.o \
	output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_wwdg.o \


output/CMSIS/startup_stm32f30x.o: Libraries/CMSIS/Device/ST/STM32F30x/Source/Templates/gcc_ride7/startup_stm32f30x.S
	@echo 'Building target: startup_stm32f30x.S'
	@$(CC) $(ASM_FLAGS) -o "$@" "$<"

output/base/src/main.o: ./src/main.c
	@echo 'Building target: main.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/system_stm32f30x.o: ./src/system_stm32f30x.c
	@echo 'Building target: system_stm32f30x.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_adc.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_adc.c
	@echo 'Building target: stm32f30x_adc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_can.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_can.c
	@echo 'Building target: stm32f30x_can.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_comp.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_comp.c
	@echo 'Building target: stm32f30x_comp.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_crc.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_crc.c
	@echo 'Building target: stm32f30x_crc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dac.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dac.c
	@echo 'Building target: stm32f30x_dac.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dbgmcu.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dbgmcu.c
	@echo 'Building target: stm32f30x_dbgmcu.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dma.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dma.c
	@echo 'Building target: stm32f30x_dma.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_exti.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_exti.c
	@echo 'Building target: stm32f30x_exti.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_flash.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_flash.c
	@echo 'Building target: stm32f30x_flash.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_gpio.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_gpio.c
	@echo 'Building target: stm32f30x_gpio.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_i2c.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_i2c.c
	@echo 'Building target: stm32f30x_i2c.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_iwdg.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_iwdg.c
	@echo 'Building target: stm32f30x_iwdg.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_misc.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_misc.c
	@echo 'Building target: stm32f30x_misc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_opamp.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_opamp.c
	@echo 'Building target: stm32f30x_opamp.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_pwr.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_pwr.c
	@echo 'Building target: stm32f30x_pwr.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rcc.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rcc.c
	@echo 'Building target: stm32f30x_rcc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rtc.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rtc.c
	@echo 'Building target: stm32f30x_rtc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_spi.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_spi.c
	@echo 'Building target: stm32f30x_spi.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_syscfg.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_syscfg.c
	@echo 'Building target: stm32f30x_syscfg.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_tim.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_tim.c
	@echo 'Building target: stm32f30x_tim.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_usart.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_usart.c
	@echo 'Building target: stm32f30x_usart.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_wwdg.o: ./Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_wwdg.c
	@echo 'Building target: stm32f30x_wwdg.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"



