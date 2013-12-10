#
# "base" - base directory
# "dirs" - directiry with sources, all source files in this directory added (without recursion)
# "files" - raw list of source files
# "output" - output path prefix

sources = [
{
	"base" : "Libraries/CMSIS/Device/ST/STM32F30x/Source/Templates/gcc_ride7",
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
		"Libraries/STM32F30x_StdPeriph_Driver/src"
		],

}
]


