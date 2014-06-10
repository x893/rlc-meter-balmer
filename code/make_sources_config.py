#
# "base" - base directory
# "dirs" - directiry with sources, all source files in this directory added (without recursion)
# "files" - raw list of source files
# "output" - output path prefix

sources = [
{
	"base" : "Libraries/CMSIS/Device/ST/STM32F10x/Source/Templates/gcc_ride7",
	"output" : "CMSIS",
	"files" : [
		"startup_stm32f10x_cl.S"
		#"startup_stm32f10x_md.S"
		]

},
{
	"base" : ".",
	"output" : "base",
	"dirs" : [
		"src",
		"src/SysTick",
		"Libraries/STM32_USB-FS-Device_Driver/src",
		"Libraries/STM32F10x_StdPeriph_Driver/src"
		],

}
]


