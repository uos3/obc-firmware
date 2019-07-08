ifneq ($(CC),)
  CC=gcc
else
  CC=$(CC)
endif

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

ifneq ($(TESTCOVERAGE_ON),)
  CFLAGS += --coverage
endif

CSRCS = $(FW_SRCS) \
	$(wildcard ./*.c)

OSRCS = $(patsubst %.c,%.o,$(filter %.c,${CSRCS}))

ASRCS = liblabrador_ldpc.a

LIBS = -lm -lrt

%.o: %.c
	@if [ ! 'x${VERBOSE_ON}' = x ];                               \
	then                                                    \
	    echo $(CC) -c $(USE_OPT) $(CFLAGS) $(BOARD_INCLUDE) $(DEFINES) -o $@ $<; \
	fi
	@$(CC) -c $(USE_OPT) $(CFLAGS) $(BOARD_INCLUDE) $(DEFINES) -o $@ $<

${FW_ROOT}/${OUTDIR}/${OUTFILE}: $(OSRCS)
	@mkdir -p ${FW_ROOT}/${OUTDIR}/
	@if [ ! 'x${VERBOSE_ON}' = x ];                               \
	then                                                    \
	    echo $(CC) $(USE_OPT) $(CFLAGS) $(BOARD_INCLUDE) $(DEFINES) $(OSRCS) $(ASRCS) -o ${FW_ROOT}/${OUTDIR}/${OUTFILE} $(LIBS); \
	fi
	@$(CC) $(USE_OPT) $(CFLAGS) $(BOARD_INCLUDE) $(DEFINES) $(OSRCS) $(ASRCS) -o ${FW_ROOT}/${OUTDIR}/${OUTFILE} $(LIBS)
	@rm -fr $(OSRCS)

clean:
	@rm -fr $(OSRCS) ${FW_ROOT}/${OUTDIR}/${OUTFILE}
