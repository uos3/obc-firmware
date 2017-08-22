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
FW_ROOT=../../..
ROOT=TivaWare_C_Series-2.1.4.178

#
# Include the common make definitions.
#
include ${FW_ROOT}/src/firmware.mk
BOARD_INCLUDE="-DBOARD_INCLUDE=\"board/${BOARD}/board.h\""

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
	       -DPART_${PART}      \
	       -c
AR      = ${PREFIX}-ar
LD      = ${PREFIX}-gcc
LDFLAGS =   -mthumb \
            ${CPU} \
            ${FPU} \
            -Wl,--gc-sections \
            -Wl,--print-memory-usage \
            -nostartfiles \
            --specs=nano.specs \
            --specs=nosys.specs
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
CFLAGS += -Wall \
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

FW_SRCS+= $(wildcard ./*.c)

#
# Where to find header files that do not live in the source directory.
#
IPATH=${ROOT}

#
# The default rule, which causes the blinky example to be built.
#
all: ${FW_ROOT}/${OUTDIR}
all: ${FW_ROOT}/${OUTDIR}/${OUTFILE}

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${wildcard ${FW_ROOT}/${OUTDIR}/${OUTFILE}.*}
	@rm -rf ${wildcard *.o} ${wildcard *~}
	@rm -rf $(patsubst %.c,%.o,${FW_SRCS})
	@rm -rf $(patsubst %.c,%.d,${FW_SRCS})

#
# The rule to create the target directory.
#
${FW_ROOT}/${OUTDIR}:
	@mkdir -p ${FW_ROOT}/${OUTDIR}/

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
#
# The rule for linking the application.
#
${FW_ROOT}/${OUTDIR}/${OUTFILE}:
	@if [ 'x${VERBOSE_ON}' = x ];                                            \
	 then                                                                 \
	     echo "  LD    ${@} ${LNK_SCP}";                                  \
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

#
# Rules for building the blinky example.
#
${FW_ROOT}/${OUTDIR}/${OUTFILE}: $(patsubst %.c,%.o,${FW_SRCS})
${FW_ROOT}/${OUTDIR}/${OUTFILE}: tm4c_startup_${COMPILER}.o
${FW_ROOT}/${OUTDIR}/${OUTFILE}: ${ROOT}/driverlib/gcc/libdriver.a
${FW_ROOT}/${OUTDIR}/${OUTFILE}: tm4c123g.ld
SCATTERgcc=tm4c123g.ld
ENTRY_SYM=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
