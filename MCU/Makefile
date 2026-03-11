######################################################################
#  Makefile for CL02 Closed-Loop MCU firmware
#  Target: STM32F373CCTx (Cortex-M4, FPv4-SP-D16)
#  Toolchain: arm-none-eabi-gcc
#
#  Usage:
#    make          — build firmware (CL02.elf + CL02.bin + CL02.hex)
#    make clean    — remove build artifacts
#    make flash    — flash via st-flash (STLink)
#    make size     — show section sizes
######################################################################

# ─── Toolchain ──────────────────────────────────────────────────
PREFIX  = arm-none-eabi-
CC      = $(PREFIX)gcc
AS      = $(PREFIX)gcc -x assembler-with-cpp
CP      = $(PREFIX)objcopy
SZ      = $(PREFIX)size

# ─── Target ─────────────────────────────────────────────────────
TARGET  = CL02
BUILD_DIR = build

# ─── MCU flags ──────────────────────────────────────────────────
CPU     = -mcpu=cortex-m4
FPU     = -mfpu=fpv4-sp-d16
FLOAT   = -mfloat-abi=hard
MCU     = $(CPU) -mthumb $(FPU) $(FLOAT)

# ─── Defines ────────────────────────────────────────────────────
C_DEFS  = -DUSE_HAL_DRIVER -DSTM32F373xC -DARM_MATH_CM4 \
          -D__forceinline='__attribute__((always_inline)) inline' \
          -D'__ldrex(p)=__LDREXW((volatile uint32_t*)(p))' \
          -D'__strex(v,p)=__STREXW((v),(volatile uint32_t*)(p))'

# ─── Include paths ──────────────────────────────────────────────
C_INCLUDES = \
  -IInc \
  -IDrivers/STM32F3xx_HAL_Driver/Inc \
  -IDrivers/STM32F3xx_HAL_Driver/Inc/Legacy \
  -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc \
  -IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
  -IDrivers/CMSIS/Device/ST/STM32F3xx/Include \
  -IDrivers/CMSIS/Include \
  -IDrivers/CMSIS/DSP/Include

# ─── Source files ───────────────────────────────────────────────

# Application / User
C_SOURCES = \
  Src/main.c \
  Src/stm32f3xx_it.c \
  Src/stm32f3xx_hal_msp.c \
  Src/system_stm32f3xx.c \
  Src/dataMGR.c \
  Src/HD32_filter.c \
  Src/CE32_COMMAND.c \
  Src/CE32_USB_INTERCOM.c \
  Src/CE32_ClosedLoop.c \
  Src/CE32_stimulator.c \
  Src/CE32_TTL_Trigger.c \
  Src/usb_device.c \
  Src/usbd_conf.c \
  Src/usbd_desc.c \
  Src/usbd_cdc_if.c

# HAL Drivers
C_SOURCES += \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_cortex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_gpio.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dma.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_exti.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_adc.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_adc_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dac.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dac_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_sdadc.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pcd.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pcd_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_usb.c

# USB Middleware
C_SOURCES += \
  Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
  Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
  Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
  Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c

# Startup (GCC assembly)
ASM_SOURCES = startup_stm32f373xc_gcc.s

# ─── Linker script ──────────────────────────────────────────────
LDSCRIPT = STM32F373CCTx_FLASH.ld

# ─── Compiler flags ────────────────────────────────────────────
OPT = -O1
CFLAGS  = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT)
CFLAGS += -Wall -fdata-sections -ffunction-sections -std=c99
CFLAGS += -g -gdwarf-2

ASFLAGS = $(MCU) -g

# ─── Linker flags ──────────────────────────────────────────────
LIBS    = -lc -lm -lnosys
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS)
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref
LDFLAGS += -Wl,--gc-sections

# ─── Build rules ───────────────────────────────────────────────
OBJECTS  = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))
DIRS     = $(sort $(dir $(OBJECTS)))

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin size

$(DIRS):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c | $(DIRS)
	$(CC) -c $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" $< -o $@

$(BUILD_DIR)/%.o: %.s | $(DIRS)
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O ihex $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O binary -S $< $@

size: $(BUILD_DIR)/$(TARGET).elf
	$(SZ) $<

clean:
	rm -rf $(BUILD_DIR)

# Flash via st-flash (install: sudo apt install stlink-tools)
flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $< 0x08000000

# ─── Debug ──────────────────────────────────────────────────────
# Terminal 1: make gdbserver   (starts ST-Link GDB server on :4242)
# Terminal 2: make debug       (launches GDB, connects, loads firmware)
GDB = gdb-multiarch

gdbserver:
	st-util -p 4242

debug: $(BUILD_DIR)/$(TARGET).elf
	$(GDB) -ex "target extended-remote :4242" \
	       -ex "load" \
	       -ex "monitor reset halt" \
	       $<

# Include dependency files
-include $(OBJECTS:.o=.d)

.PHONY: all clean flash size gdbserver debug
