BUILDDIR			:= build
TARGET				:= stm32f103rbt6
CROSS_COMPILE		:= arm-none-eabi-
LINKERFILE			:= Linker/$(TARGET).ld

DEF					:= \
						STM32F10X_MD_VL \
						STM32F103CBTx \
						SYSCLK_FREQ_HSE
						
INC					:= \
						CMSIS/DeviceSupport \
						CMSIS/CoreSupport \
						CMSIS/Startup \
						inc \
						../Common \
						../stm32f10x_cpp/inc

SRC					:= \
						CMSIS/DeviceSupport/system_stm32f10x.c \
						CMSIS/DeviceSupport/syscalls.c \
						CMSIS/DeviceSupport/sysmem.c \
						CMSIS/Startup/startup_stm32f10x_md_vl.s \
						../stm32f10x_cpp/src/gpio.cpp \
						../stm32f10x_cpp/src/timer.cpp \
						../stm32f10x_cpp/src/clock.cpp \
						../stm32f10x_cpp/src/uart.cpp \
						src/main.cpp

ASMFLAGS			:= -mcpu=cortex-m3
CFLAGS_EXTRA			:= -O2
CFLAGS				:= -std=c17
CPPLAGS				:= -std=c++17

include flags.embedded
include Makefile.embedded

install_toolchain:
	@echo "Installing ARM GCC toolchain..."
	sudo apt -y install gcc-arm-none-eabi