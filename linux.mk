
CC = gcc
OUTDIR?=builds
include builds/firmware_sources.mk

${OUTDIR}/${MAINFILE}.bin: src/main/${MAINFILE}.c $(FW_SRCS)
	$(CC) src/main/${MAINFILE}.c $(FW_SRCS) -o ${OUTDIR}/${MAINFILE}.bin
	@make clean
