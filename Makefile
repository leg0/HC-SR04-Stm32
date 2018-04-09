-include local.mk

TARGET=sonar

BINPATH?=
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

STM32CUBE_FW?=./
C_DEFS=-DSTM32L432xx -DUSE_FULL_LL_DRIVER
C_INCLUDES=-I$(STM32CUBE_FW)Drivers/CMSIS/Device/ST/STM32L4xx/Include \
-I$(STM32CUBE_FW)Drivers/CMSIS/Include \
-I$(STM32CUBE_FW)Drivers/STM32L4xx_HAL_Driver/Inc

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

LDSCRIPT = STM32L432KCUx_FLASH.ld
LIBS = -lc -lm -lnosys 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS) -Wl,-Map=$(TARGET).map,--cref -Wl,--gc-sections

SRC = main.c stm32f10x_HC-SR04.c system_stm32l4xx.c startup_stm32l432xx.s
SRC += stm32l4xx_ll_tim.c stm32l4xx_ll_rcc.c stm32l4xx_ll_gpio.c
OBJ1=$(SRC:.c=.o)
OBJ=$(OBJ1:.s=.o)

#vpath %.c $(STM32CUBE_FW)Drivers/STM32L4xx/STM32L4xx_HAL_Driver/Src
vpath %.c /home/lego/STM32Cube/Repository/STM32Cube_FW_L4_V1.11.0/Drivers/STM32L4xx_HAL_Driver/Src

all: $(TARGET).elf $(TARGET).hex $(TARGET).bin

#startup_stm32l432xx.o: startup_stm32l432xx.s
%.o: %.s
	$(AS) -c $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(notdir $(<:.c=.lst)) $< -o $@

$(TARGET).elf: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@
	$(SZ) $@

%.hex: %.elf
	$(HEX) $< $@

%.bin: %.elf
	$(BIN) $< $@

clean:
	rm -f *.o *.elf *.hex *.bin *.map *.d *.lst