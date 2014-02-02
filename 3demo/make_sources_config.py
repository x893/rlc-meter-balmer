#
# "base" - base directory
# "dirs" - directiry with sources, all source files in this directory added (without recursion)
# "files" - raw list of source files
# "output" - output path prefix

sources = [
{
	"base" : "../3code/Libraries/CMSIS/Device/ST/STM32F30x/Source/Templates/gcc_ride7",
	"output" : "CMSIS",
	"files" : [
		"startup_stm32f30x.S"
		]

},
{
	"base" : ".",
	"output" : "base",
	"dirs" : [
		"src",
		"STM32F3_Discovery",
		],
},
{
	"base" : "../3code/Libraries/STM32F30x_StdPeriph_Driver/src",
	"output" : "StdPeriph_Driver",
	"files" : [
		"stm32f30x_adc.c",
		#"stm32f30x_can.c",
		#"stm32f30x_comp.c",
		#"stm32f30x_crc.c",
		"stm32f30x_dac.c",
		#"stm32f30x_dbgmcu.c",
		"stm32f30x_dma.c",
		"stm32f30x_exti.c",
		#"stm32f30x_flash.c",
		"stm32f30x_gpio.c",
		"stm32f30x_i2c.c",
		#"stm32f30x_iwdg.c",
		"stm32f30x_misc.c",
		#"stm32f30x_opamp.c",
		#"stm32f30x_pwr.c",
		"stm32f30x_rcc.c",
		#"stm32f30x_rtc.c",
		"stm32f30x_spi.c",
		"stm32f30x_syscfg.c",
		"stm32f30x_tim.c",
		#"stm32f30x_usart.c",
		#"stm32f30x_wwdg.c"
		]
}
]


