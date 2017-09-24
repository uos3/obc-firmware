CC=gcc
FW_ROOT=../../..

#
# Include the common make definitions.
#
include ${FW_ROOT}/src/firmware.mk

ifneq ($(RUNTESTS),)
  include ${FW_ROOT}/src/test.mk
endif

BOARD_INCLUDE="-DBOARD_INCLUDE=\"board/${BOARD}/board.h\""

CFLAGS += -std=gnu99 \
      -Wall \
			-Wextra \
			-Wpedantic \
			-Wswitch-default \
			-Wswitch-enum \
			-Wconversion \
			-Wlogical-op \
			-Wstrict-prototypes \
			-Wmissing-prototypes \
			-Wmissing-declarations

ifneq ($(WERROR_ON),)
  CFLAGS += -Werror
endif

CFLAGS += -ggdb3

ifneq ($(OSIZE_ON),)
  CFLAGS += -Os
else
  CFLAGS += -O0
endif

ifneq ($(TESTVERBOSE_ON),)
  CFLAGS += -DTEST_VERBOSE=true
endif

SRCS = $(FW_SRCS) \
	$(wildcard ./*.c)

LIBS = -lm -lrt

compile:
	@mkdir -p ${FW_ROOT}/${OUTDIR}/
	@if [ 'x${VERBOSE_ON}' = x ];                               \
	then                                                    \
	    echo "Compiling..";                                \
	else                                                    \
	    echo $(CC) $(USE_OPT) $(CFLAGS) $(BOARD_INCLUDE) $(DEFINES) $(SRCS) -o ${FW_ROOT}/${OUTDIR}/${OUTFILE} $(LIBS); \
	fi
	@$(CC) $(USE_OPT) $(CFLAGS) $(BOARD_INCLUDE) $(DEFINES) $(SRCS) -o ${FW_ROOT}/${OUTDIR}/${OUTFILE} $(LIBS)

clean:
	@rm -fr *.o ${FW_ROOT}/${OUTDIR}/${OUTFILE}