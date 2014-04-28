
# Put your stlink folder here so make burn will work.
STLINK=/home/superkazuya/Code/STM32_Dev/stlink
DSP_PATH = $(STM_COMMON)Libraries/CMSIS/DSP_Lib/Source/

# Put your source files here (or *.c, etc)
SRCS=$(wildcard *.s) $(wildcard *.c)
#SRCS += $(wildcard $(STM_COMMON)driver/*.c)
SRCS += $(wildcard */*.c)
SRCS += $(DSP_PATH)TransformFunctions/arm_cfft_radix4_init_f32.c
SRCS += $(DSP_PATH)TransformFunctions/arm_cfft_radix4_f32.c
SRCS += $(DSP_PATH)TransformFunctions/arm_rfft_init_f32.c
SRCS += $(DSP_PATH)TransformFunctions/arm_rfft_f32.c
SRCS += $(DSP_PATH)CommonTables/arm_common_tables.c
SRCS += $(DSP_PATH)ComplexMathFunctions/arm_cmplx_mag_squared_f32.c
SRCS += $(DSP_PATH)TransformFunctions/arm_dct4_f32.c
SRCS += $(DSP_PATH)TransformFunctions/arm_dct4_init_f32.c
SRCS += $(DSP_PATH)BasicMathFunctions/arm_scale_f32.c
SRCS += $(DSP_PATH)BasicMathFunctions/arm_mult_f32.c
SRCS += $(DSP_PATH)ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c
SRCS += $(DSP_PATH)FastMathFunctions/arm_cos_f32.c

#SLIB = $(wildcard $(STM_COMMON)PDMFilter/*.a)

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
NAME=record
PROJ_NAME = bin/$(NAME)

# Put your STM32F4 library code directory here
STM_COMMON=/home/superkazuya/Code/STM32_Dev/

# Normally you shouldn't need to change anything below this line!
#######################################################################################

CC=arm-none-eabi-gcc
CCA=arm-none-eabi-as
CCLD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

CFLAGS  = -g
CFLAGS += -Tstm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += -mfloat-abi=hard 
CFLAGS += -mfpu=fpv4-sp-d16
CFLAGS += -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -D__FPU_PRESENT=1
#CFLAGS += -L$(SLIB)
#CFLAGS += -L. -lPDMFilter_GCC
#CFLAGS += -Wa -mimplicit-it=thumb

# Include files from STM libraries
CFLAGS += -I.
CFLAGS += -I$(STM_COMMON)Utilities/STM32F4-Discovery
CFLAGS += -I$(STM_COMMON)Libraries/CMSIS/Include 
CFLAGS += -I$(STM_COMMON)Libraries/CMSIS/ST/STM32F4xx/Include
CFLAGS += -I$(STM_COMMON)Libraries/STM32F4xx_StdPeriph_Driver/inc
LDFLAGS = -L. 
LDFLAGS += -lm
#LDFLAGS += -l arm_cortexM4lf_math

# add startup file to build
SRCS += $(STM_COMMON)Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s 
OBJS = $(SRCS:.c=.o)


.PHONY: proj

all: proj

proj: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin



clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

