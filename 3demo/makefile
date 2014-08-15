RM := rm -rf
TC=arm-none-eabi
CC=$(TC)-gcc
CPP=$(TC)-gcc
LD=$(TC)-ld -v
OBJCOPY=$(TC)-objcopy
AR=$(TC)-ar
GDB=$(TC)-gdb


C_INCLIDES= -Iinc -I../3code/Libraries/STM32_USB-FS-Device_Driver/inc -I../3code/Libraries/STM32F30x_StdPeriph_Driver/inc -I../3code/Libraries/CMSIS/Include
C_INCLIDES+= -ISTM32F3_Discovery
C_INCLIDES+= -I../3code/Libraries/CMSIS/Device/ST/STM32F30x/Include
C_INCLIDES+= -includesrc/stm32f30x_conf.h

ifeq ($(MAKECMDGOALS),debug)
C_DEFINES= -DDEBUG
C_FLAGS= -O0 -g3
endif

ifeq ($(MAKECMDGOALS),release)
C_DEFINES= -DNDEBUG
C_FLAGS= -O3
endif

C_DEFINES+= -DHSE_VALUE=8000000
C_DEFINES+= -std=c99

C_FLAGS+= -Wall -c -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD $(C_INCLIDES) $(C_DEFINES) 
#C_FLAGS+= -g -Wa,-adhls #asm listing

LDFLAGS = -nostartfiles -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
LDFLAGS += -T "stm32_flash_md.ld"  -Wl,-Map=linker.map -Wl,-cref  -Wl,--gc-sections 
ASM_FLAGS = -x assembler-with-cpp -c -mthumb -mcpu=cortex-m4

TARGET=output/program

# All of the sources participating in the build are defined here
include sources.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

# All Target
#all: executable

release: executable

debug: executable

# Tool invocations
executable: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	@$(TC)-g++ $(LDFLAGS) -o "$(TARGET)_$(MAKECMDGOALS)" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo 'Create Binary'
	$(OBJCOPY) -S  -O binary  "$(TARGET)_$(MAKECMDGOALS)" "$(TARGET)_$(MAKECMDGOALS).bin"
	@echo ' '

# Other Targets
clean:
	-@echo 'Clean files'	
	-@$(RM) $(OBJS)$(DEPS) $(TARGET)
	-@echo ' '

flashd:
	python /balmer/radio/stm32/tools/stm32_flash.py "${CURDIR}/$(TARGET)_debug.bin"

flashr:
	python /balmer/radio/stm32/tools/stm32_flash.py "${CURDIR}/$(TARGET)_release.bin"

.PHONY: all clean dependents
