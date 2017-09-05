CC=gcc
FW_ROOT=../../..

#
# Include the common make definitions.
#
include ${FW_ROOT}/src/firmware.mk
BOARD_INCLUDE="-DBOARD_INCLUDE=\"board/${BOARD}/board.h\""

CFLAGS += -Wall \
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

SRCS = $(FW_SRCS) \
	$(wildcard ./*.c)

LIBS = -lm -lrt

compile:
	$(CC) $(USE_OPT) $(CFLAGS) $(BOARD_INCLUDE) $(DEFINES) $(SRCS) -o ${FW_ROOT}/${OUTDIR}/${OUTFILE} $(LIBS)

clean:
	rm -frv *.o ${FW_ROOT}/${OUTDIR}/${OUTFILE}