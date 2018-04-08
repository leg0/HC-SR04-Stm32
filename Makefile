TARGET=sonar

BINPATH?=/home/lego/opt/gcc-arm-none-eabi-7-2017-q4-major/bin/
PREFIX?=arm-none-eabi-
CC=$(BINPATH)$(PREFIX)gcc
AS=$(BINPATH)$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)$(PREFIX)objcopy
AR = $(BINPATH)$(PREFIX)ar
SZ = $(BINPATH)$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

C_DEFS=-DSTM32L432xx
C_INCLUDES=-I/home/lego/STM32Cube/Repository/STM32Cube_FW_L4_V1.11.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include \
-I/home/lego/STM32Cube/Repository/STM32Cube_FW_L4_V1.11.0/Drivers/CMSIS/Include

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

LDSCRIPT = STM32L432KCUx_FLASH.ld
LIBS = -lc -lm -lnosys 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS) -Wl,-Map=$(TARGET).map,--cref -Wl,--gc-sections

SRC=main.c stm32f10x_HC-SR04.c
OBJ=$(SRC:.c=.o)

all: $(TARGET).elf $(TARGET).hex $(TARGET).bin

startup_stm32l432xx.o: startup_stm32l432xx.s
	$(AS) -c $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(notdir $(<:.c=.lst)) $< -o $@

$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) -o $@
	$(SZ) $@

%.hex: %.elf
	$(HEX) $< $@

%.bin: %.elf
	$(BIN) $< $@

clean:
	rm -f *.o *.elf *.hex *.bin *.map *.d
