# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected.
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

# Verbosity. TODO: Set this with a ./build flag
V=@

FW_ROOT=../../..
include ${FW_ROOT}/src/firmware.mk
BOARD_INCLUDE="-DBOARD_INCLUDE=\"board/${BOARD}/board.h\""

# Configure the following settings according to the device in use
DEVICE     = m328p
CLOCK      = 1000000
PROGRAMMER = -c avrispmkII -P usb -B 5
SOURCES	   = $(FW_SRCS) $(wildcard *.c)
FUSES      = -U hfuse:w:0xdf:m -U lfuse:w:0x62:m

# End configuration
OBJECTS = $(SOURCES:.c=.o)
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE) -C /etc/avrdude.conf

# Compiler flags. Optimise for code size. Allow C99 standards.
COMPILE = avr-gcc -Wall -Os -gdwarf-2 -std=gnu99 -DF_CPU=$(CLOCK) -mmcu=atmega168

OUT_ELF = ${FW_ROOT}/${OUTDIR}/${OUTFILE}

# symbolic targets:
all: $(OUT_ELF)

.c.o:
	$(V) $(COMPILE) $(BOARD_INCLUDE) -c $< -o $@

.S.o:
	$(V) $(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(V) $(COMPILE) -S $< -o $@

flash:	all
	$(V) $(AVRDUDE) -U flash:w:$(OUT_ELF).hex:i

eeprom: all
	$(V) $(AVRDUDE) -U eeprom:w:$(OUT_ELF).eep:i

fuse:
	$(V) $(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	$(V) bootloadHID $(OUT_ELF).hex

clean:
	$(V) rm -f $(OBJECTS) $(OUT_ELF) $(OUT_ELF).hex

# file targets:
${OUT_ELF}: $(OBJECTS)
	$(V) $(COMPILE) -o ${OUT_ELF} $(OBJECTS)
	$(V) avr-size -C --mcu=${DEVICE} $@

main.hex: $(OUT_ELF)
	$(V) rm -f $(OUT_ELF).hex
	$(V) avr-objcopy -j .text -j .data -O ihex $(OUT_ELF) $(OUT_ELF).hex
	#avr-objcopy -j .eeprom -O ihex $(OUT_ELF) $(OUT_ELF).eep

# Targets for code debugging and analysis:
disasm:	$(OUT_ELF)
	$(V) avr-objdump -d $(OUT_ELF)