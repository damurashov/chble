# Toolchain
PREFIX = riscv-none-embed-

CC      = $(PREFIX)gcc
CXX     = $(PREFIX)g++
AS      = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE    = $(PREFIX)size
AR      = $(PREFIX)ar

# Project
TARGET = HID_Mouse

BUILD_DIR = build

ELF = $(BUILD_DIR)/$(TARGET).elf
HEX = $(BUILD_DIR)/$(TARGET).hex
BIN = $(BUILD_DIR)/$(TARGET).bin
MAP = $(BUILD_DIR)/$(TARGET).map
LST = $(BUILD_DIR)/$(TARGET).lst

# Flags
COMMON_FLAGS = -march=rv32imacxw -mabi=ilp32 -mcmodel=medany \
-msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os \
-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections \
-fno-common -g

INCLUDES = \
-I./SRC/Core \
-I./SRC/Startup \
-I./SRC/Debug \
-I./SRC/Peripheral/inc \
-I./APP/include \
-I./Profile/include \
-I./HAL/include \
-I./LIB \
-I.

DEFINES = -DCH32V20x_D8W

CFLAGS   = $(COMMON_FLAGS) $(INCLUDES) $(DEFINES) -std=gnu99
CXXFLAGS = $(COMMON_FLAGS) -std=gnu++11 -fabi-version=0
ASFLAGS  = $(COMMON_FLAGS) -x assembler-with-cpp

LDFLAGS = $(COMMON_FLAGS) \
-T ./HAL/Link.ld \
-L./ -L./LIB \
-nostartfiles \
-Wl,--gc-sections \
-Wl,--print-memory-usage \
-Wl,-Map=$(MAP) \
--specs=nano.specs --specs=nosys.specs

LIBS = -lwchble -lc

# Sources
SRCS = \
Profile/battservice.c \
Profile/devinfoservice.c \
Profile/hiddev.c \
Profile/hidmouseservice.c \
Profile/scanparamservice.c \
APP/ch32v20x_it.c \
APP/hidmouse.c \
APP/hidmouse_main.c \
APP/system_ch32v20x.c \
SRC/Startup/startup_ch32v20x_D8W.S \
SRC/Peripheral/src/ch32v20x_adc.c \
SRC/Peripheral/src/ch32v20x_bkp.c \
SRC/Peripheral/src/ch32v20x_can.c \
SRC/Peripheral/src/ch32v20x_crc.c \
SRC/Peripheral/src/ch32v20x_dbgmcu.c \
SRC/Peripheral/src/ch32v20x_dma.c \
SRC/Peripheral/src/ch32v20x_exti.c \
SRC/Peripheral/src/ch32v20x_flash.c \
SRC/Peripheral/src/ch32v20x_gpio.c \
SRC/Peripheral/src/ch32v20x_i2c.c \
SRC/Peripheral/src/ch32v20x_iwdg.c \
SRC/Peripheral/src/ch32v20x_misc.c \
SRC/Peripheral/src/ch32v20x_opa.c \
SRC/Peripheral/src/ch32v20x_pwr.c \
SRC/Peripheral/src/ch32v20x_rcc.c \
SRC/Peripheral/src/ch32v20x_rtc.c \
SRC/Peripheral/src/ch32v20x_spi.c \
SRC/Peripheral/src/ch32v20x_tim.c \
SRC/Peripheral/src/ch32v20x_usart.c \
SRC/Peripheral/src/ch32v20x_wwdg.c \
SRC/Debug/debug.c \
SRC/Core/core_riscv.c \
LIB/ble_task_scheduler.S \
HAL/MCU.c \
HAL/RTC.c \
HAL/SLEEP.c

# Object mapping (mirror directory structure)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

# Default target
all: $(ELF)

# Link
$(ELF): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
	$(OBJCOPY) -O ihex $@ $(HEX)
	$(OBJCOPY) -O binary $@ $(BIN)
	$(OBJDUMP) --source --all-headers --demangle -M xw --line-numbers --wide $@ > $(LST)
	$(SIZE) --format=berkeley $@

# Compile C
$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile ASM
$(BUILD_DIR)/%.S.o: %.S
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
