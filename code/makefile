RM := rm -rf
TC=arm-none-eabi
CC=$(TC)-gcc
LD=$(TC)-ld -v
OBJCOPY=$(TC)-objcopy
AR=$(TC)-ar
GDB=$(TC)-gdb


C_INCLIDES= -Iinc -ILibraries/STM32_USB-FS-Device_Driver/inc -ILibraries/STM32F10x_StdPeriph_Driver/inc -ILibraries/CMSIS/Include
C_INCLIDES+= -ILibraries/CMSIS/Device/ST/STM32F10x/Include
C_INCLIDES+= -includeinc/stm32f10x_conf.h

ifeq ($(MAKECMDGOALS),debug)
C_DEFINES= -DDEBUG
C_FLAGS= -O0 -g3
endif

ifeq ($(MAKECMDGOALS),release)
C_DEFINES= -DNDEBUG
C_FLAGS= -Os
endif

# C_DEFINES+= -DSTM32F10X_MD
C_DEFINES+= -DSTM32F10X_CL
C_DEFINES+= -DHSE_VALUE=8000000
C_DEFINES+= -std=c99

C_FLAGS+= -Wall -c -mthumb -mcpu=cortex-m3 -mfix-cortex-m3-ldrd -MMD $(C_INCLIDES) $(C_DEFINES) 

LDFLAGS = -nostartfiles -mthumb -mcpu=cortex-m3 -T "stm32_flash_md.ld" -mfix-cortex-m3-ldrd  -Wl,-Map=linker.map -Wl,-cref  -Wl,--gc-sections 
ASM_FLAGS = -x assembler-with-cpp -c -mthumb -mcpu=cortex-m3

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
