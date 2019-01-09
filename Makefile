################################################################################
#
# The MIT License (MIT)
# Copyright (c) 2018 STMicroelectronics
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
################################################################################

# -----------------------------------------------------------------------------
# Atollic Compiler or GCC
OS_NAME := LINUX

ifeq (C:,$(findstring C:, $(PATH)))
	OS_NAME := WINDOWS
endif

ifeq (c:,$(findstring c:, $(PATH)))
	OS_NAME := WINDOWS
endif

ifeq ($(TRUESTUDIO_BUILD), 1)
	# -----------------------------------------------------------------------------
	# Windows or Linux
	ifeq ($(OS_NAME),WINDOWS)
		GCC_PREFIX := arm-atollic-eabi
		CC = arm-atollic-eabi-gcc
		CXX = arm-atollic-eabi-g++
		OBJCOPY = arm-atollic-eabi-objcopy
		SIZE = arm-atollic-eabi-size
		MKDIR_CMD = mkdir
		MKDIR_FROM_CHAR =/
		MKDIR_TO_CHAR =\\
		MKDIR_OPTION = 2> NUL || echo off

		SHELL = cmd
	else
		GCC_PREFIX := arm-atollic-eabi
		CC = arm-atollic-eabi-gcc
		CXX = arm-atollic-eabi-g++
		OBJCOPY = arm-atollic-eabi-objcopy
		SIZE = arm-atollic-eabi-size
		MKDIR_CMD = mkdir -p
		MKDIR_FROM_CHAR = \
		MKDIR_TO_CHAR = /
		MKDIR_OPTION = 2> /dev/null

		SHELL = /bin/sh
	endif
else
	GCC_PREFIX := arm-none-eabi
	CC = ./toolchain/gcc-arm-none-eabi-6-2017-q2-update/bin/arm-none-eabi-gcc
	CXX = ./toolchain/gcc-arm-none-eabi-6-2017-q2-update/bin/arm-none-eabi-g++
	OBJCOPY = ./toolchain/gcc-arm-none-eabi-6-2017-q2-update/bin/arm-none-eabi-objcopy
	SIZE = ./toolchain/gcc-arm-none-eabi-6-2017-q2-update/bin/arm-none-eabi-size
	MKDIR_CMD = mkdir -p
	MKDIR_FROM_CHAR = \
	MKDIR_TO_CHAR = /
	MKDIR_OPTION = 2> /dev/null

	SHELL = /bin/sh
endif

RM=rm -rf

# -----------------------------------------------------------------------------
# System configuration
TARGET :=

BUILD_OPTION := -c -Wall
TARGET_MCU :=
TARGET_FPU :=
TARGET_INSTRUCTION :=
TARGET_C_EXTRA :=
TARGET_CXX_EXTRA :=
TARGET_A_EXTRA :=

TARGET_HAL_DEFINITION :=
TARGET_MODEL_DEFINITION :=

TARGET_HAL_VERSION :=

# -----------------------------------------------------------------------------
# for compiler
CFLAGS :=
CXXFLAGS :=
AFLAGS :=
LFLAGS :=
# Link Script
LINK_SCCRIPT_FILES := ldscript/$(TARGET_BOARD)/$(TARGET_BOARD)_FLASH.ld

# -----------------------------------------------------------------------------
# for path & source
ASM_SRC :=
C_SRC :=
CXX_SRC :=

SATIC_LIBRARY :=

INCLUDE_DIR :=
HAL_LIBRARY_C_SRC :=
MIDDLEWARE_C_SRC :=
DRIVERS_C_SRC :=
MODEL_C_SRC :=

HAL_LIBRARY_CXX_SRC :=
MIDDLEWARE_CXX_SRC :=
DRIVERS_CXX_SRC :=
MODEL_CXX_SRC :=

# -----------------------------------------------------------------------------
# Define output directory
OBJECT_DIR = OUTPUT
BIN_DIR = $(OBJECT_DIR)

# -----------------------------------------------------------------------------
# device driver
USB_DEVICE := 
USE_USB_DEVICE := 
SENSOR_LSM6DSL :=
TOUCH_DEVICE :=

NET_LWIP :=
RTOS_FREERTOS :=
STEMWIN :=
CLEANFLIGHT_CLI :=

P_NUCLEO_53L0A1 :=
SENSOR_VL53L0X :=
GPIO_STMPE1600 :=

QSPI_FLASH_USE :=
QSPI_MEMORY_TYPE :=

# -----------------------------------------------------------------------------
# for TARGET Board

# "STEVAL_FCU001V1"
ifeq ("$(TARGET_BOARD)", "STEVAL_FCU001V1")
	TARGET_MCU += -mcpu=cortex-m4
	TARGET_FPU += -mfpu=fpv4-sp-d16
	TARGET_INSTRUCTION += -mthumb -mhard-float
	TARGET_C_EXTRA += -ffunction-sections -fdata-sections -specs=nano.specs
	TARGET_CXX_EXTRA += -fno-threadsafe-statics -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti
	TARGET_A_EXTRA += -specs=nano.specs -x assembler-with-cpp

	TARGET_HAL_DEFINITION += -D"STM32F4XX" -D"STM32F40XX" -D"STM32F401xC"  -DUSE_HAL_DRIVER
	TARGET_MODEL_DEFINITION += -D$(TARGET_BOARD) -DUART_DEBUG_PORT=1 -DSUPPORT_DEBUG_OUTPUT -DUART_DEBUG_OUTPUT -DDEBUG_STRING_LEVEL_ERROR
	TARGET_MODEL_DEFINITION += -DUSE_USB_CDC_DEVICE -DUSE_USB_FS
	TARGET_MODEL_DEFINITION += -DTIM2_REMOTE_CONTROL -DMOTOR_DC -UMOTOR_ESC 

	# HAL Library Version
	TARGET_HAL_VERSION := F4_V1.22.0

	# supported usb device
	USB_DEVICE := CDC
	USE_USB_DEVICE := USED
	SENSOR_LSM6DSL := SENSOR_LSM6DSL

	ifeq ("$(DEBUG)", "1")
		TARGET := $(TARGET_BOARD)_DEBUG
		BUILD_OPTION += -g -O0
		TARGET_MODEL_DEFINITION += -DDEBUG_STRING_LEVEL_WARN -DDEBUG_STRING_LEVEL_DEBUG -DDEBUG_STRING_LEVEL_FN_TRACE -DDEBUG_STRING_LEVEL_INFO -DDEBUG_STRING_LEVEL_DUMP
	else
		TARGET := $(TARGET_BOARD)_RELEASE
		BUILD_OPTION += -O3
	endif

	# for HAL library & CMSIS
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/CMSIS/Include
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/CMSIS/Device/ST/STM32F4xx/Include
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Inc
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Inc/Legacy
endif

# "DISCOVERY_STM32F7_BOOTLOADER"
ifeq ("$(TARGET_BOARD)", "DISCOVERY_STM32F7_BOOTLOADER")
	TARGET_MCU += -mcpu=cortex-m7
	TARGET_FPU += -mfpu=fpv5-sp-d16
	TARGET_INSTRUCTION += -mthumb -mhard-float
	TARGET_C_EXTRA += -ffunction-sections -fdata-sections -specs=nano.specs
	TARGET_CXX_EXTRA += -fno-threadsafe-statics -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti
	TARGET_A_EXTRA += -specs=nano.specs -x assembler-with-cpp

	TARGET_HAL_DEFINITION += -DSTM32F7xx -DSTM32F746xx -DUSE_HAL_DRIVER
	TARGET_MODEL_DEFINITION += -D$(TARGET_BOARD) -DUART_DEBUG_PORT=6 -DSUPPORT_DEBUG_OUTPUT -DUART_DEBUG_OUTPUT -DDEBUG_STRING_LEVEL_ERROR
#	TARGET_MODEL_DEFINITION += -DSYSTEM_CLOCK_MAX_216MHZ
	TARGET_MODEL_DEFINITION += -DSYSTEM_CLOCK_200MHZ
	TARGET_MODEL_DEFINITION += -DUSE_USB_BULK_DEVICE -DUSE_USB_HS

	# HAL Library Version
	TARGET_HAL_VERSION := F7_V1.14.0

	# supported usb device
	USB_DEVICE := BULK
	USE_USB_DEVICE := USED

	# QSPI NOR Flash Memory
	QSPI_FLASH_USE := QSPI_FLASH_USE
	QSPI_MEMORY_TYPE := MICRON_N25Q128A

	ifeq ("$(DEBUG)", "1")
		TARGET := $(TARGET_BOARD)_DEBUG
		BUILD_OPTION += -g -O0
		TARGET_MODEL_DEFINITION += -DDEBUG_STRING_LEVEL_WARN -DDEBUG_STRING_LEVEL_DEBUG -DDEBUG_STRING_LEVEL_FN_TRACE -DDEBUG_STRING_LEVEL_INFO -DDEBUG_STRING_LEVEL_DUMP
	else
		TARGET := $(TARGET_BOARD)_RELEASE
		BUILD_OPTION += -O3
	endif

	# for HAL library & CMSIS
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/CMSIS/Include
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/CMSIS/Device/ST/STM32F7xx/Include
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Inc
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Inc/Legacy
endif


# "DISCOVERY_STM32F7"
ifeq ("$(TARGET_BOARD)", "DISCOVERY_STM32F7")
	TARGET_MCU += -mcpu=cortex-m7
	TARGET_FPU += -mfpu=fpv5-sp-d16
	TARGET_INSTRUCTION += -mthumb -mhard-float
	TARGET_C_EXTRA += -ffunction-sections -fdata-sections -specs=nano.specs
	TARGET_CXX_EXTRA += -fno-threadsafe-statics -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti
	TARGET_A_EXTRA += -specs=nano.specs -x assembler-with-cpp

	TARGET_HAL_DEFINITION += -DSTM32F7xx -DSTM32F746xx -DUSE_HAL_DRIVER
	TARGET_MODEL_DEFINITION += -D$(TARGET_BOARD) -DUART_DEBUG_PORT=6 -DSUPPORT_DEBUG_OUTPUT -DUART_DEBUG_OUTPUT -DDEBUG_STRING_LEVEL_ERROR
#	TARGET_MODEL_DEFINITION += -DSYSTEM_CLOCK_MAX_216MHZ
	TARGET_MODEL_DEFINITION += -DSYSTEM_CLOCK_200MHZ
	TARGET_MODEL_DEFINITION += -DUSE_USB_CDC_DEVICE -DUSE_USB_HS

	# Link Script File
	LINK_SCCRIPT_FILES := ldscript/$(TARGET_BOARD)/$(TARGET_BOARD)_FLASH_WITH_QFLASH.ld

	# HAL Library Version
	TARGET_HAL_VERSION := F7_V1.14.0

	# supported usb device
	USB_DEVICE := CDC
	USE_USB_DEVICE := USED
	TOUCH_DEVICE := FT5536
	# STM32 Expansion Board
	P_NUCLEO_53L0A1 := P_NUCLEO_53L0A1

	# QSPI NOR Flash Memory
	QSPI_FLASH_USE := QSPI_FLASH_USE
	QSPI_MEMORY_TYPE := MICRON_N25Q128A

	# supported middleware
#	NET_LWIP := NET_LWIP
	RTOS_FREERTOS := RTOS_FREERTOS
	STEMWIN := STEMWIN
	CLEANFLIGHT_CLI := CLEANFLIGHT_CLI

	ifeq ("$(DEBUG)", "1")
		TARGET := $(TARGET_BOARD)_DEBUG
		BUILD_OPTION += -g -O0
		TARGET_MODEL_DEFINITION += -DDEBUG_STRING_LEVEL_WARN -DDEBUG_STRING_LEVEL_DEBUG -DDEBUG_STRING_LEVEL_FN_TRACE -DDEBUG_STRING_LEVEL_INFO -DDEBUG_STRING_LEVEL_DUMP
	else
		TARGET := $(TARGET_BOARD)_RELEASE
		BUILD_OPTION += -O3
	endif
	# for HAL library & CMSIS
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/CMSIS/Include
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/CMSIS/Device/ST/STM32F7xx/Include
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Inc
	INCLUDE_DIR += -I./Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Inc/Legacy
endif

# "NUCLEO_H743ZI"
ifeq ("$(TARGET_BOARD)", "NUCLEO_H743ZI")
	TARGET_MCU += -mcpu=cortex-m7
	TARGET_FPU += -mfpu=fpv5-sp-d16
	TARGET_INSTRUCTION += -mthumb -mhard-float
	TARGET_C_EXTRA += -ffunction-sections -fdata-sections -specs=nano.specs
	TARGET_CXX_EXTRA += -fno-threadsafe-statics -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti
	TARGET_A_EXTRA += -specs=nano.specs -x assembler-with-cpp

	TARGET_HAL_DEFINITION += -D"STM32H7xx" -D"STM32H743xx"  -DUSE_HAL_DRIVER
	TARGET_MODEL_DEFINITION += -D$(TARGET_BOARD) -DUART_DEBUG_PORT=6 -DSUPPORT_DEBUG_OUTPUT -DUART_DEBUG_OUTPUT -DDEBUG_STRING_LEVEL_ERROR
	TARGET_MODEL_DEFINITION += -DUSE_USB_CDC_DEVICE -D"USE_USB_FS"
	# for test
	TARGET_MODEL_DEFINITION += -D"USED_AUTOGENERATION_CLOCK"

	# HAL Library Version
	TARGET_HAL_VERSION := H7_V1.3.0

	# supported usb device
	USB_DEVICE := CDC
	USE_USB_DEVICE := USED

	# supported middleware
	RTOS_FREERTOS := RTOS_FREERTOS
	CLEANFLIGHT_CLI := CLEANFLIGHT_CLI

	ifeq ("$(DEBUG)", "1")
		TARGET := $(TARGET_BOARD)_DEBUG
		BUILD_OPTION += -g -O0
		TARGET_MODEL_DEFINITION += -DDEBUG_STRING_LEVEL_WARN -DDEBUG_STRING_LEVEL_DEBUG -DDEBUG_STRING_LEVEL_FN_TRACE -DDEBUG_STRING_LEVEL_INFO -DDEBUG_STRING_LEVEL_DUMP
	else
		TARGET := $(TARGET_BOARD)_RELEASE
		BUILD_OPTION == -O3
	endif

	# for HAL library & CMSIS
	INCLUDE_DIR += -IDrivers/$(TARGET_HAL_VERSION)/CMSIS/Include
	INCLUDE_DIR += -IDrivers/$(TARGET_HAL_VERSION)/CMSIS/Device/ST/STM32H7xx/Include
	INCLUDE_DIR += -IDrivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Inc
	INCLUDE_DIR += -IDrivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Inc/Legacy
endif

# -----------------------------------------------------------------------------
# HAL Source

# "STEVAL_FCU001V1"
ifeq ("$(TARGET_BOARD)", "STEVAL_FCU001V1")
	# HAL Library Source
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c

	ifeq ($(USE_USB_DEVICE),USED)
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
	endif
endif

# "DISCOVERY_STM32F7_BOOTLOADER"
ifeq ("$(TARGET_BOARD)", "DISCOVERY_STM32F7_BOOTLOADER")
	# HAL Library Source
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_usart.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_fmc.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_sdram.c

	ifeq ($(QSPI_FLASH_USE), QSPI_FLASH_USE)
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_qspi.c
	endif

	ifeq ($(USE_USB_DEVICE),USED)
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd_ex.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_usb.c
	endif
endif


# "DISCOVERY_STM32F7"
ifeq ("$(TARGET_BOARD)", "DISCOVERY_STM32F7")
	# HAL Library Source
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_usart.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_fmc.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_ltdc.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma2d.c

	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_sdram.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_eth.c

	ifeq ($(QSPI_FLASH_USE), QSPI_FLASH_USE)
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_qspi.c
	endif

	ifeq ($(USE_USB_DEVICE),USED)
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd_ex.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_usb.c
	endif
endif

# "NUCLEO_H743ZI"
ifeq ("$(TARGET_BOARD)", "NUCLEO_H743ZI")
	# HAL Library Source
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc_ex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c
	HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c

	ifeq ($(USE_USB_DEVICE),USED)
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd_ex.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c
		HAL_LIBRARY_C_SRC += Drivers/$(TARGET_HAL_VERSION)/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_usb.c
	endif
endif

# -----------------------------------------------------------------------------
# Middle Source

# for CDC of USB
ifeq ($(USB_DEVICE),CDC)
	MIDDLEWARE_C_SRC += $(wildcard ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c)
	MIDDLEWARE_C_SRC += $(wildcard ./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*.c)
	MIDDLEWARE_C_SRC += $(wildcard ./src/drivers/usb/cdc/src/*.c)
	INCLUDE_DIR += -I./Middlewares/ST/STM32_USB_Device_Library/Core/Inc
	INCLUDE_DIR += -I./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
	INCLUDE_DIR += -I./src/drivers/usb/cdc/inc
endif

# for BULK of USB
ifeq ($(USB_DEVICE),BULK)
	MIDDLEWARE_C_SRC += $(wildcard ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c)
	MIDDLEWARE_C_SRC += $(wildcard ./Middlewares/ST/STM32_USB_Device_Library/Class/BULK/Src/*.c)
	MIDDLEWARE_C_SRC += $(wildcard ./src/drivers/usb/bulk/src/*.c)
	INCLUDE_DIR += -I./Middlewares/ST/STM32_USB_Device_Library/Core/Inc
	INCLUDE_DIR += -I./Middlewares/ST/STM32_USB_Device_Library/Class/BULK/Inc
	INCLUDE_DIR += -I./src/drivers/usb/bulk/inc
endif

# for LWIP of Net
ifeq ($(NET_LWIP),NET_LWIP)
	TARGET_MODEL_DEFINITION += -D$(NET_LWIP)

	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/ipv6cp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/mem.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/pppapi.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/utils.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/api/netbuf.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/pbuf.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/vj.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/ppp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv4/icmp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/chap-new.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/pppos.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/ccp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv4/ip4.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/api/sockets.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/api/api_lib.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/tcp_out.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv4/etharp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/demand.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/slipif.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/tcp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/api/api_msg.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/memp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/api/err.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/auth.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv4/autoip.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/api/netdb.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/api/netifapi.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/eui64.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/raw.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/magic.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/upap.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/tcp_in.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/init.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/pppcrypt.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv4/igmp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/def.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/stats.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ip.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/chap_ms.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/sys.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/timeouts.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/polarssl/md5.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/polarssl/md4.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/netif.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/lcp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/polarssl/arc4.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/chap-md5.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/pppoe.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/ipcp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ethernet.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/dns.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/polarssl/des.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_frag.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/pppol2tp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv4/dhcp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/mppe.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/fsm.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/api/tcpip.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/ecp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/lowpan6.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/eap.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/inet_chksum.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/udp.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/polarssl/sha1.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/src/netif/ppp/multilink.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/LwIP/system/OS/sys_arch.c

	INCLUDE_DIR += -IMiddlewares/Third_Party/LwIP/src/include
	INCLUDE_DIR += -IMiddlewares/Third_Party/LwIP/system
	INCLUDE_DIR += -IMiddlewares/Third_Party/LwIP/test/unit
endif

# for Free RTOS
ifeq ($(RTOS_FREERTOS),RTOS_FREERTOS)
	TARGET_MODEL_DEFINITION += -D$(RTOS_FREERTOS)

	MIDDLEWARE_C_SRC += Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/port.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/FreeRTOS/Source/tasks.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/FreeRTOS/Source/timers.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/FreeRTOS/Source/queue.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/FreeRTOS/Source/croutine.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/FreeRTOS/Source/list.c
	MIDDLEWARE_C_SRC += Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c

	INCLUDE_DIR += -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1
	INCLUDE_DIR += -IMiddlewares/Third_Party/FreeRTOS/Source/include
	INCLUDE_DIR += -IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
endif

# for STemWin of Graphic Library
ifeq ($(STEMWIN),STEMWIN)
	TARGET_MODEL_DEFINITION += -D$(STEMWIN)

	ifeq ($(RTOS_FREERTOS),RTOS_FREERTOS)
		TARGET_MODEL_DEFINITION += -DOS_SUPPORT
		MIDDLEWARE_C_SRC += Middlewares/ST/STemWin/OS/GUI_X_OS.c
	else
		MIDDLEWARE_C_SRC += Middlewares/ST/STemWin/OS/GUI_X.c
	endif

	DRIVERS_C_SRC += src/drivers/stemwin/src/GUIConf.c
	DRIVERS_C_SRC += src/drivers/stemwin/src/LCDConf.c

	INCLUDE_DIR += -Isrc/drivers/stemwin/inc
	INCLUDE_DIR += -IMiddlewares/ST/STemWin/inc

	ifeq ($(RTOS_FREERTOS),RTOS_FREERTOS)
			SATIC_LIBRARY += -LMiddlewares/ST/STemWin/Lib -l:STemWin_CM7_OS_wc32.a
	else
			SATIC_LIBRARY += -LMiddlewares/ST/STemWin/Lib -l:STemWin_CM7_wc32.a
	endif

#	SATIC_LIBRARY += -LMiddlewares/ST/STemWin/Lib -l:STemWin_CM7_wc32.a
endif

# for CLI of Cleanflight
ifeq ($(CLEANFLIGHT_CLI),CLEANFLIGHT_CLI)
	TARGET_MODEL_DEFINITION += -D$(CLEANFLIGHT_CLI)

	DRIVERS_C_SRC += src/interface/src/cleanflight_cli.c
	INCLUDE_DIR += -Isrc/interface/inc
endif

# for 52L0A1 Expansion Board fo ST
ifeq ($(P_NUCLEO_53L0A1),P_NUCLEO_53L0A1)
	SENSOR_VL53L0X := SENSOR_VL53L0X
	GPIO_STMPE1600 := GPIO_STMPE1600

	TARGET_MODEL_DEFINITION += -D$(P_NUCLEO_53L0A1)

	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/vl53l0x_platform.c
endif

# for STMPE1600 GPIO Extension
ifeq ($(GPIO_STMPE1600), GPIO_STMPE1600)
	TARGET_MODEL_DEFINITION += -D$(GPIO_STMPE1600)

	DRIVERS_C_SRC += src/drivers/stmpe1600/src/gpio_stmpe1600.c
	INCLUDE_DIR += -Isrc/drivers/stmpe1600/inc
endif

# for VL53L0X Sensor
ifeq ($(SENSOR_VL53L0X), SENSOR_VL53L0X)
	TARGET_MODEL_DEFINITION += -D$(SENSOR_VL53L0X)

	DRIVERS_C_SRC += src/drivers/vl53l0x/src/vl53l0x_api.c
	DRIVERS_C_SRC += src/drivers/vl53l0x/src/vl53l0x_api_calibration.c
	DRIVERS_C_SRC += src/drivers/vl53l0x/src/vl53l0x_api_core.c
	DRIVERS_C_SRC += src/drivers/vl53l0x/src/vl53l0x_api_ranging.c
	DRIVERS_C_SRC += src/drivers/vl53l0x/src/vl53l0x_api_strings.c
	DRIVERS_C_SRC += src/drivers/vl53l0x/src/vl53l0x_platform_log.c

	INCLUDE_DIR += -Isrc/drivers/vl53l0x/inc
endif

# for QSPI Nor Flash Memory
ifeq ($(QSPI_FLASH_USE), QSPI_FLASH_USE)
	TARGET_MODEL_DEFINITION += -D$(QSPI_FLASH_USE) -D$(QSPI_MEMORY_TYPE)

	DRIVERS_C_SRC += src/drivers/qspi/src/qspi_flash.c

	INCLUDE_DIR += -Isrc/drivers/qspi/inc
endif

# -----------------------------------------------------------------------------
# Common Driver Source
ifeq ($(USE_USB_DEVICE),USED)
	DRIVERS_C_SRC += src/drivers/usb/usb_device.c
	INCLUDE_DIR += -Isrc/drivers/usb/inc
endif

# C Source of common
DRIVERS_C_SRC += src/common/src/printf.c
DRIVERS_C_SRC += src/common/src/uart_debug.c
DRIVERS_C_SRC += src/common/src/ring_buffer.c

ifneq ($(RTOS_FREERTOS),RTOS_FREERTOS)
	DRIVERS_C_SRC += src/common/src/scheduler.c
endif

INCLUDE_DIR += -Isrc/common/inc

ifeq ($(SENSOR_LSM6DSL),SENSOR_LSM6DSL)
	TARGET_MODEL_DEFINITION += -D$(SENSOR_LSM6DSL)

	DRIVERS_C_SRC += src/drivers/sensor_imu/src/lsm6dsl.c
	INCLUDE_DIR += -Isrc/drivers/sensor_imu/inc
endif

ifeq ($(TOUCH_DEVICE),FT5536)
	TARGET_MODEL_DEFINITION += -D$(TOUCH_DEVICE)

	DRIVERS_C_SRC += src/drivers/touch/src/touch_ft5536.c
	INCLUDE_DIR += -Isrc/drivers/touch/inc
endif


# -----------------------------------------------------------------------------
# Model Source

# "STEVAL_FCU001V1"
ifeq ("$(TARGET_BOARD)", "STEVAL_FCU001V1")
	INCLUDE_DIR += -Isrc/model/$(TARGET_BOARD)/inc
	INCLUDE_DIR += -Isrc/drivers/$(TARGET_BOARD)/inc

	ASM_SRC += src/startup/$(TARGET_BOARD)/startup_stm32f401xc.s

	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/stm32f4xx_hal_msp.c
	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/stm32f4xx_it.c
	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/system_stm32f4xx.c

	# C Source of user driver
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/steval_fcu001v1_driver.c
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/sensor_spi2.c

	# 외부로 뺄 함수들
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/battery_gauge.c
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/blue_nrg_spi1.c
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/led.c
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/lis2mdl.c
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/lps22hd.c
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/motor.c
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/motor_ext_esc.c
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/remote_controller.c

	MODEL_CXX_SRC +=
endif

# "DISCOVERY_STM32F7_BOOTLOADER"
ifeq ("$(TARGET_BOARD)", "DISCOVERY_STM32F7_BOOTLOADER")
	INCLUDE_DIR += -Isrc/model/$(TARGET_BOARD)/inc
	INCLUDE_DIR += -Isrc/drivers/$(TARGET_BOARD)/inc

	ASM_SRC += src/startup/$(TARGET_BOARD)/startup_stm32f746xx.s

	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/stm32f7xx_hal_msp.c
	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/stm32f7xx_it.c
	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/system_stm32f7xx.c

	# C Source of user driver
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/discovery_stm32f7_driver.c
endif

# "DISCOVERY_STM32F7"
ifeq ("$(TARGET_BOARD)", "DISCOVERY_STM32F7")
	INCLUDE_DIR += -Isrc/model/$(TARGET_BOARD)/inc
	INCLUDE_DIR += -Isrc/drivers/$(TARGET_BOARD)/inc

	ASM_SRC += src/startup/$(TARGET_BOARD)/startup_stm32f746xx.s

	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/stm32f7xx_hal_msp.c
	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/stm32f7xx_it.c
	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/system_stm32f7xx.c

	# C Source of user driver
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/discovery_stm32f7_driver.c
	ifeq ($(NET_LWIP),NET_LWIP)
		INCLUDE_DIR += -Isrc/drivers/lwip/inc

		DRIVERS_C_SRC += src/drivers/lwip/src/lwip.c
		DRIVERS_C_SRC += src/drivers/lwip/src/ethernetif.c
		DRIVERS_C_SRC += src/drivers/lwip/src/dhcp_ethernet.c
	endif
endif

# "NUCLEO_H743ZI"
ifeq ("$(TARGET_BOARD)", "NUCLEO_H743ZI")
	INCLUDE_DIR += -Isrc/model/$(TARGET_BOARD)/inc
	INCLUDE_DIR += -Isrc/drivers/$(TARGET_BOARD)/inc

	ASM_SRC += src/startup/$(TARGET_BOARD)/startup_stm32h753xx.s

	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/stm32h7xx_hal_msp.c
	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/stm32h7xx_it.c
	MODEL_C_SRC += src/model/$(TARGET_BOARD)/src/system_stm32h7xx.c

	MODEL_CXX_SRC +=

	# C Source of user driver
	DRIVERS_C_SRC += src/drivers/$(TARGET_BOARD)/src/nucleo_h743zi_driver.c
endif

# -----------------------------------------------------------------------------
MODEL_CXX_SRC += src/main.cpp 

C_SRC := $(HAL_LIBRARY_C_SRC) $(MIDDLEWARE_C_SRC) $(DRIVERS_C_SRC) $(MODEL_C_SRC)
CXX_SRC := $(HAL_LIBRARY_CXX_SRC) $(MIDDLEWARE_CXX_SRC) $(DRIVERS_CXX_SRC) $(MODEL_CXX_SRC)

# -----------------------------------------------------------------------------
CFLAGS :=   $(BUILD_OPTION) $(TARGET_MCU) $(TARGET_FPU) $(TARGET_INSTRUCTION) $(TARGET_C_EXTRA)   $(TARGET_HAL_DEFINITION) $(TARGET_MODEL_DEFINITION) $(INCLUDE_DIR)
CXXFLAGS := $(BUILD_OPTION) $(TARGET_MCU) $(TARGET_FPU) $(TARGET_INSTRUCTION) $(TARGET_CXX_EXTRA) $(TARGET_HAL_DEFINITION) $(TARGET_MODEL_DEFINITION) $(INCLUDE_DIR)
AFLAGS :=   $(BUILD_OPTION) $(TARGET_MCU) $(TARGET_FPU) $(TARGET_INSTRUCTION) $(TARGET_A_EXTRA)   $(TARGET_HAL_DEFINITION) $(TARGET_MODEL_DEFINITION) $(INCLUDE_DIR)
LFLAGS :=   $(TARGET_MCU) $(TARGET_FPU) $(TARGET_INSTRUCTION) -Wl,--gc-sections -static  -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group -specs=nano.specs -specs=nosys.specs -Wl,-cref
LFLAGS +=   -Wl,-V $(SATIC_LIBRARY)
LFLAGS +=   "-Wl,-Map=$(BIN_DIR)/$(TARGET).map"  -Wl,--defsym=malloc_getpagesize_P=0x1000  -T$(LINK_SCCRIPT_FILES)

# -----------------------------------------------------------------------------
OBJS := $(C_SRC:%.c=$(OBJECT_DIR)/%.o)
OBJS += $(ASM_SRC:%.s=$(OBJECT_DIR)/%.o)
# OBJS += $(ASM_SRC:%.S=$(OBJECT_DIR)/%.o)
OBJS += $(CXX_SRC:%.cpp=$(OBJECT_DIR)/%.o)

###############
# Build project
# Major targets
###############
all: DEBUG_MAKE_BEFORE $(TARGET) DEBUG_MAKE_AFTER

DEBUG_MAKE_BEFORE:
	@echo =======================================================================================
#	@echo "Buils System : $(OS_NAME)"
#	@echo CFLAGS : $(CFLAGS)
#	@echo CXXFLAGS : $(CXXFLAGS)
#	@echo AFLAGS : $(AFLAGS)
#	@echo LFLAGS : $(LFLAGS)
#	@echo =======================================================================================
#	@echo ASM_SRC : $(ASM_SRC)
#	@echo C_SRC   : $(C_SRC)
#	@echo CXX_SRC : $(CXX_SRC)
#	@echo OBJS    : $(OBJS)
#	@echo =======================================================================================

DEBUG_MAKE_AFTER:
#	@echo =======================================================================================
#	@echo "Buils System   : $(OS_NAME)"
#	@echo "Build Compiler : $(GCC_PREFIX)"
#	@echo "Target Board   : $(TARGET_BOARD)"
#	@echo OBJS : $(OBJS)
#	@echo =======================================================================================

$(TARGET): $(OBJS) 
	$(CXX) -o "$(BIN_DIR)/$(TARGET).elf" $(OBJS) $(LFLAGS)
	$(OBJCOPY) -O ihex "$(BIN_DIR)/$(TARGET).elf" "$(BIN_DIR)/$(TARGET).hex"
	$(OBJCOPY) -O binary "$(BIN_DIR)/$(TARGET).elf" "$(BIN_DIR)/$(TARGET).bin"
	@echo =======================================================================================
	@echo Buils System   : $(OS_NAME)
	@echo Build Compiler : $(GCC_PREFIX)
	@echo Target Board   : $(TARGET_BOARD)
	@echo Link Script    : $(LINK_SCCRIPT_FILES)
	@echo ---------------------------------------------------------------------------------------
	$(SIZE) "$(BIN_DIR)/$(TARGET).elf"
	@echo =======================================================================================

clean:
	$(RM) $(OBJS) "$(BIN_DIR)/$(TARGET).elf" "$(BIN_DIR)/$(TARGET).map" "$(BIN_DIR)/$(TARGET).hex" "$(BIN_DIR)/$(TARGET).bin"

##################
# Implicit targets
##################
#$(OBJECT_DIR)/%.o: %.c
#	@echo ---------------------------------------------------------------------------------------
#	@echo      : $(PATH)
#	@echo      : $(OS_NAME)
#	@echo OBJS : $@
#	@echo      : $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@))
#	@echo      : $(MKDIR_CMD) $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@))
#	@echo ---------------------------------------------------------------------------------------
$(OBJECT_DIR)/%.o: %.c
	@$(MKDIR_CMD) $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@)) $(MKDIR_OPTION)
	$(CC) $(CFLAGS) $< -o $@

#$(OBJECT_DIR)/%.o: %.cpp
#	@echo ---------------------------------------------------------------------------------------
#	@echo      : $(OS_NAME)
#	@echo OBJS : $@
#	@echo      : $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@))
#	@echo      : $(MKDIR_CMD) $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@))
#	@echo ---------------------------------------------------------------------------------------
$(OBJECT_DIR)/%.o: %.cpp
	@$(MKDIR_CMD) $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@)) $(MKDIR_OPTION)
	$(CXX) $(CXXFLAGS) $< -o $@

#$(OBJECT_DIR)/%.o: %.s
#	@echo ---------------------------------------------------------------------------------------
#	@echo      : $(OS_NAME)
#	@echo OBJS : $@
#	@echo      : $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@))
#	@echo      : $(MKDIR_CMD) $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@))
#	@echo ---------------------------------------------------------------------------------------
$(OBJECT_DIR)/%.o: %.s
	@$(MKDIR_CMD) $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@)) $(MKDIR_OPTION)
	$(CC) $(AFLAGS) $< -o $@
	
#$(OBJECT_DIR)/%.o: %.S
#	@echo ---------------------------------------------------------------------------------------
#	@echo      : $(OS_NAME)
#	@echo OBJS : $@
#	@echo      : $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@))
#	@echo      : $(MKDIR_CMD) $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@))
#	@echo ---------------------------------------------------------------------------------------
$(OBJECT_DIR)/%.o: %.S
	@$(MKDIR_CMD) $(subst $(MKDIR_FROM_CHAR),$(MKDIR_TO_CHAR),$(dir $@)) $(MKDIR_OPTION)
	$(CC) $(AFLAGS) $< -o $@

