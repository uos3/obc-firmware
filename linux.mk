
CC = gcc
OUTDIR?=builds
include builds/firmware_sources.mk

${OUTDIR}/${MAINFILE}.out: src/main/${MAINFILE}.c $(FW_SRCS)
	$(CC) src/main/${MAINFILE}.c $(FW_SRCS) -o ${OUTDIR}/${MAINFILE}.out
	@make -f linux.mk -s clean

clean:
	@rm -rf src/*/*.o
	@rm -rf src/*/*.d

clean-builds:
	@rm builds/*.out
	@rm builds/*.mk