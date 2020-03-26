# High level, what does this need to do?
# 	1) needs to compile any possible main file (those in the main folder)
# 	based on the dependancies only IT requires.

TIVAWARE=../TivaWare_C_Series-2.1.4.178
#******************************************************************************
#
# Makefile - Rules for building the blinky example.
#
# Copyright (c) 2011-2017 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 2.1.4.178 of the DK-TM4C123G Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=TM4C123GH6PGE

#
# The base directory for TivaWare.
#
FW_ROOT=.
ROOT=../TivaWare_C_Series-2.1.4.178
OUTDIR?=builds
# MAINFILE?=demo_antenna

# 8KB Heap
HEAPSIZE = 0x2000
# 2KB Stack
STACKSIZE = 0x800
# VERBOSE_ON="yes"

include builds/firmware_sources.mk


BOARD_INCLUDE="-DBOARD_INCLUDE=\"../driver/board.h\""

# Moved from makedefs
os     := ${shell uname -s}
ifndef COMPILER
  COMPILER = gcc
endif
PREFIX := ${shell type arm-stellaris-eabi-gcc > /dev/null 2>&1 && \
         echo arm-stellaris-eabi || echo arm-none-eabi}
CC      = ${PREFIX}-gcc
ARMGCC_ROOT:=${shell dirname '${shell sh -c "which ${CC}"}'}/..
CPU     = -mcpu=cortex-m4
FPU     = -mfpu=fpv4-sp-d16 -mfloat-abi=hard
AFLAGS  = -mthumb \
	       ${CPU}  \
	       ${FPU}  \
	       -MD
CFLAGS  = -mthumb          \
	       ${CPU}              \
	       ${FPU}              \
	       -ffunction-sections \
	       -fdata-sections     \
	       -MD                 \
	       -std=c99            \
           -c                  \
	       -DPART_${PART}      \
           -DHEAPSIZE=${HEAPSIZE} \
           -DSTACKSIZE=${STACKSIZE}
AR      = ${PREFIX}-ar
LD      = ${PREFIX}-gcc
LDFLAGS =   -mthumb \
            ${CPU} \
            ${FPU} \
            -Wl,--gc-sections \
            -Wl,--print-memory-usage \
            -nostartfiles \
            --specs=nano.specs \
            --specs=nosys.specs \
            -Wl,--defsym,HEAPSIZE=${HEAPSIZE} \
            -Wl,--defsym,STACKSIZE=${STACKSIZE}
LIBGCC := ${shell ${CC} ${CFLAGS} -print-libgcc-file-name}
LIBC   := ${shell ${CC} ${CFLAGS} -print-file-name=libc.a}
LIBM   := ${shell ${CC} ${CFLAGS} -print-file-name=libm.a}
OBJCOPY = ${PREFIX}-objcopy
CFLAGS += ${CFLAGSgcc}
AFLAGS += ${patsubst %,-I%,${subst :, ,${IPATH}}}
CFLAGS += ${patsubst %,-I%,${subst :, ,${IPATH}}}

CFLAGS += -ggdb3
ifneq ($(OSIZE_ON),)
  CFLAGS += -Os
else
  CFLAGS += -O0
endif 
WARNINGS += -Wall \
			-Wextra \
			-Wpedantic \
			-Wswitch-default \
			-Wswitch-enum \
			-Wduplicated-cond \
			-Wconversion \
			-Wlogical-op \
			-Wstrict-prototypes \
			-Wmissing-prototypes \
			-Wmissing-declarations
ifneq ($(WERROR_ON),)
  CFLAGS += -Werror
endif

ifneq ($(TESTVERBOSE_ON),)
  CFLAGS += -DTEST_VERBOSE=true
endif

# FW_SRCS+= $(wildcard ./src/driver/*.c) liblabrador_ldpc.a

#
# Where to find header files that do not live in the source directory.
#
IPATH=${ROOT}

# The rule for building the object file from each C source file.
#
%.o: %.c
	@if [ 'x${VERBOSE_ON}' = x ];                          \
	 then                                                 \
	     echo "  CC    ${<}";                             \
	 else                                                 \
	     echo ${CC} ${CFLAGS} ${BOARD_INCLUDE} -D${COMPILER} -o ${@} ${<}; \
	 fi
	@${CC} ${CFLAGS} ${BOARD_INCLUDE} -D${COMPILER} -o ${@} ${<}

#
# The rule for building the object file from each assembly source file.
#
%.o: %.S
	@if [ 'x${VERBOSE_ON}' = x ];                               \
	 then                                                    \
	     echo "  AS    ${<}";                                \
	 else                                                    \
	     echo ${CC} ${AFLAGS} -D${COMPILER} -o ${@} -c ${<}; \
	 fi
	@${CC} ${AFLAGS} -D${COMPILER} -o ${@} -c ${<}

#
# The rule for creating an object library.
#
%.a:
	@if [ 'x${VERBOSE_ON}' = x ];     \
	 then                          \
	     echo "  AR    ${@}";      \
	 else                          \
	     echo ${AR} -cr ${@} ${^}; \
	 fi
	@${AR} -cr ${@} ${^}

${OUTDIR}/${MAINFILE}.bin: src/main/${MAINFILE}.o
	@if [ 'x${VERBOSE_ON}' = x ];                                            \
	 then                                                                 \
	     echo "  LD    ${@} ${LNK_SCP}";echo "${LNK_SCP}"; \
	 else                                                                 \
	     echo ${LD} -T ${SCATTERgcc}                                         \
	          --entry ${ENTRY_SYM}                       \
	          ${LDFLAGSgcc_${notdir ${@:.axf=}}}                          \
	          ${LDFLAGS} -o ${@}.axf $(filter %.o, ${^}) $(filter %.a, ${^})      \
	          '${LIBM}' '${LIBC}' '${LIBGCC}';                            \
	 fi;                                                                  \
	${LD} -T ${SCATTERgcc}                                                   \
	      --entry ${ENTRY_SYM}                           \
	      ${LDFLAGSgcc_${notdir ${@:.axf=}}}                              \
	      ${LDFLAGS} -o ${@}.axf $(filter %.o, ${^}) $(filter %.a, ${^})  \
	      '${LIBM}' '${LIBC}' '${LIBGCC}';                          
	@${OBJCOPY} -O elf32-littlearm ${@}.axf ${@};
	@rm -f ${@}.axf;
	@$(MAKE) -s clean;

#
# The rule to create the target directory.
#
${OUTDIR}:
	@mkdir -p ${OUTDIR}


${OUTDIR}/${MAINFILE}.bin: $(patsubst %.c,%.o,${FW_SRCS})
${OUTDIR}/${MAINFILE}.bin: ${ROOT}/driverlib/gcc/libdriver.a
${OUTDIR}/${MAINFILE}.bin: src/driver-tobc/tm4c_startup_${COMPILER}.o
${OUTDIR}/${MAINFILE}.bin: src/driver-tobc/tm4c123g.ld
SCATTERgcc=src/driver-tobc/tm4c123g.ld
ENTRY_SYM=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1

clean:
	@rm -rf src/*/*.o
	@rm -rf src/*/*.d

clean-builds:
	@rm builds/*.bin
	@rm builds/*.mk

