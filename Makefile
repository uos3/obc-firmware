# High level, what does this need to do?
# 	1) needs to compile any possible main file (those in the main folder)
# 	based on the dependancies only IT requires.

TIVAWARE=src/driver/TivaWare_C_Series-2.1.4.178
CFLAGS = -linttypes -lstdbool -I$(TIVAWARE) 
PART=TM4C123GH6PGE

# 8KB Heap
HEAPSIZE = 0x2000
# 2KB Stack
STACKSIZE = 0x800
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

IPATH=${TIVAWARE}

CFLAGS += -ggdb3
ifneq ($(OSIZE_ON),)
  CFLAGS += -Os
else
  CFLAGS += -O0
endif 
# CFLAGS += -Wall \
# 			-Wextra \
# 			-Wpedantic \
# 			-Wswitch-default \
# 			-Wswitch-enum \
# 			-Wduplicated-cond \
# 			-Wconversion \
# 			-Wlogical-op \
# 			-Wstrict-prototypes \
# 			-Wmissing-prototypes \
# 			-Wmissing-declarations
ifneq ($(WERROR_ON),)
  CFLAGS += -Werror
endif

ifneq ($(TESTVERBOSE_ON),)
  CFLAGS += -DTEST_VERBOSE=true
endif


builds/demo_antenna.out: src/main/demo_antenna.o
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
	@$(MAKE) -s clean;

src/main/demo_antenna.o: src/utility/debug.o \
 src/component/led.o \
 src/driver/board.o \
 src/driver/gpio.o \
 src/driver/delay.o \
 src/driver/board.o \
 src/driver/watchdog_ext.o \
 src/driver/uart.o \
 src/driver/rtc.o \
 src/driver/wdt.o \


clean:
	@rm -rf src/*/*.o
	@rm -rf src/*/*.d


SCATTERgcc=src/driver/tm4c123g.ld
ENTRY_SYM=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1