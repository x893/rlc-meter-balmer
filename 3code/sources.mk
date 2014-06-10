################################################################################
# Automatically-generated file. Do not edit!
################################################################################


DEPS += \
	output/CMSIS/startup_stm32f30x.d \
	output/base/src/adc.d \
	output/base/src/calc.d \
	output/base/src/calc_rc.d \
	output/base/src/corrector.d \
	output/base/src/dac.d \
	output/base/src/display_light.d \
	output/base/src/format_print.d \
	output/base/src/hw_config.d \
	output/base/src/hw_pcd8544.d \
	output/base/src/lcd_interface.d \
	output/base/src/main.d \
	output/base/src/mcp6s21.d \
	output/base/src/menu.d \
	output/base/src/pcd8544.d \
	output/base/src/process_measure.d \
	output/base/src/quadrature_encoder.d \
	output/base/src/stm32f30x_it.d \
	output/base/src/system_stm32f30x.d \
	output/base/src/systick.d \
	output/base/src/usb_desc.d \
	output/base/src/usb_endp.d \
	output/base/src/usb_istr.d \
	output/base/src/usb_prop.d \
	output/base/src/usb_pwr.d \
	output/base/src/vbat.d \
	output/base/src/voltage.d \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_core.d \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_init.d \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_int.d \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_mem.d \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_regs.d \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_sil.d \
	output/StdPeriph_Driver/stm32f30x_adc.d \
	output/StdPeriph_Driver/stm32f30x_dac.d \
	output/StdPeriph_Driver/stm32f30x_dma.d \
	output/StdPeriph_Driver/stm32f30x_exti.d \
	output/StdPeriph_Driver/stm32f30x_flash.d \
	output/StdPeriph_Driver/stm32f30x_gpio.d \
	output/StdPeriph_Driver/stm32f30x_misc.d \
	output/StdPeriph_Driver/stm32f30x_rcc.d \
	output/StdPeriph_Driver/stm32f30x_spi.d \
	output/StdPeriph_Driver/stm32f30x_syscfg.d \
	output/StdPeriph_Driver/stm32f30x_tim.d \


OBJS += \
	output/CMSIS/startup_stm32f30x.o \
	output/base/src/adc.o \
	output/base/src/calc.o \
	output/base/src/calc_rc.o \
	output/base/src/corrector.o \
	output/base/src/dac.o \
	output/base/src/display_light.o \
	output/base/src/format_print.o \
	output/base/src/hw_config.o \
	output/base/src/hw_pcd8544.o \
	output/base/src/lcd_interface.o \
	output/base/src/main.o \
	output/base/src/mcp6s21.o \
	output/base/src/menu.o \
	output/base/src/pcd8544.o \
	output/base/src/process_measure.o \
	output/base/src/quadrature_encoder.o \
	output/base/src/stm32f30x_it.o \
	output/base/src/system_stm32f30x.o \
	output/base/src/systick.o \
	output/base/src/usb_desc.o \
	output/base/src/usb_endp.o \
	output/base/src/usb_istr.o \
	output/base/src/usb_prop.o \
	output/base/src/usb_pwr.o \
	output/base/src/vbat.o \
	output/base/src/voltage.o \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_core.o \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_init.o \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_int.o \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_mem.o \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_regs.o \
	output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_sil.o \
	output/StdPeriph_Driver/stm32f30x_adc.o \
	output/StdPeriph_Driver/stm32f30x_dac.o \
	output/StdPeriph_Driver/stm32f30x_dma.o \
	output/StdPeriph_Driver/stm32f30x_exti.o \
	output/StdPeriph_Driver/stm32f30x_flash.o \
	output/StdPeriph_Driver/stm32f30x_gpio.o \
	output/StdPeriph_Driver/stm32f30x_misc.o \
	output/StdPeriph_Driver/stm32f30x_rcc.o \
	output/StdPeriph_Driver/stm32f30x_spi.o \
	output/StdPeriph_Driver/stm32f30x_syscfg.o \
	output/StdPeriph_Driver/stm32f30x_tim.o \


output/CMSIS/startup_stm32f30x.o: Libraries/CMSIS/Device/ST/STM32F30x/Source/Templates/gcc_ride7/startup_stm32f30x.S
	@echo 'Building target: startup_stm32f30x.S'
	@$(CC) $(ASM_FLAGS) -o "$@" "$<"

output/base/src/adc.o: ./src/adc.c
	@echo 'Building target: adc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/calc.o: ./src/calc.c
	@echo 'Building target: calc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/calc_rc.o: ./src/calc_rc.c
	@echo 'Building target: calc_rc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/corrector.o: ./src/corrector.c
	@echo 'Building target: corrector.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/dac.o: ./src/dac.c
	@echo 'Building target: dac.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/display_light.o: ./src/display_light.c
	@echo 'Building target: display_light.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/format_print.o: ./src/format_print.c
	@echo 'Building target: format_print.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/hw_config.o: ./src/hw_config.c
	@echo 'Building target: hw_config.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/hw_pcd8544.o: ./src/hw_pcd8544.c
	@echo 'Building target: hw_pcd8544.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/lcd_interface.o: ./src/lcd_interface.c
	@echo 'Building target: lcd_interface.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/main.o: ./src/main.c
	@echo 'Building target: main.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/mcp6s21.o: ./src/mcp6s21.c
	@echo 'Building target: mcp6s21.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/menu.o: ./src/menu.c
	@echo 'Building target: menu.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/pcd8544.o: ./src/pcd8544.c
	@echo 'Building target: pcd8544.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/process_measure.o: ./src/process_measure.c
	@echo 'Building target: process_measure.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/quadrature_encoder.o: ./src/quadrature_encoder.c
	@echo 'Building target: quadrature_encoder.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/stm32f30x_it.o: ./src/stm32f30x_it.c
	@echo 'Building target: stm32f30x_it.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/system_stm32f30x.o: ./src/system_stm32f30x.c
	@echo 'Building target: system_stm32f30x.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/systick.o: ./src/systick.c
	@echo 'Building target: systick.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usb_desc.o: ./src/usb_desc.c
	@echo 'Building target: usb_desc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usb_endp.o: ./src/usb_endp.c
	@echo 'Building target: usb_endp.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usb_istr.o: ./src/usb_istr.c
	@echo 'Building target: usb_istr.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usb_prop.o: ./src/usb_prop.c
	@echo 'Building target: usb_prop.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usb_pwr.o: ./src/usb_pwr.c
	@echo 'Building target: usb_pwr.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/vbat.o: ./src/vbat.c
	@echo 'Building target: vbat.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/voltage.o: ./src/voltage.c
	@echo 'Building target: voltage.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_core.o: ./Libraries/STM32_USB-FS-Device_Driver/src/usb_core.c
	@echo 'Building target: usb_core.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_init.o: ./Libraries/STM32_USB-FS-Device_Driver/src/usb_init.c
	@echo 'Building target: usb_init.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_int.o: ./Libraries/STM32_USB-FS-Device_Driver/src/usb_int.c
	@echo 'Building target: usb_int.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_mem.o: ./Libraries/STM32_USB-FS-Device_Driver/src/usb_mem.c
	@echo 'Building target: usb_mem.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_regs.o: ./Libraries/STM32_USB-FS-Device_Driver/src/usb_regs.c
	@echo 'Building target: usb_regs.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/Libraries/STM32_USB-FS-Device_Driver/src/usb_sil.o: ./Libraries/STM32_USB-FS-Device_Driver/src/usb_sil.c
	@echo 'Building target: usb_sil.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_adc.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_adc.c
	@echo 'Building target: stm32f30x_adc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_dac.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dac.c
	@echo 'Building target: stm32f30x_dac.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_dma.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_dma.c
	@echo 'Building target: stm32f30x_dma.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_exti.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_exti.c
	@echo 'Building target: stm32f30x_exti.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_flash.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_flash.c
	@echo 'Building target: stm32f30x_flash.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_gpio.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_gpio.c
	@echo 'Building target: stm32f30x_gpio.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_misc.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_misc.c
	@echo 'Building target: stm32f30x_misc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_rcc.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rcc.c
	@echo 'Building target: stm32f30x_rcc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_spi.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_spi.c
	@echo 'Building target: stm32f30x_spi.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_syscfg.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_syscfg.c
	@echo 'Building target: stm32f30x_syscfg.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f30x_tim.o: Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_tim.c
	@echo 'Building target: stm32f30x_tim.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"



