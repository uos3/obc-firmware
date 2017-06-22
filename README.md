# UoS3 Cubesat Firmware Repository

This folder contains all the source (apart from licensed drivers) to build the executable file to be flashed directly to the hardware.

The firmware is built from a top-level C program file and resolves all the references and includes from the src/ and specified hardware folders

The design is such so that the main hardware code and hardware can be easily varied. eg camera code which can be used with different boards goes in the src folder, but the actual board specific drivers go in the relevant board folder.

## To use the build and flash tools

Open a terminal in the root folder of this firmware repository, then run the following commands:

`./build uos3-proto blinky` - will compile blinky.c with the folder 'src/board/uos3-proto/' included

  * The resultant ELF filename will be 'builds/_<board>-<program>-<gitref>_' eg 'builds/uos3-proto-blinky-fd8ea0c'

`./flash <binary>` - will flash a board (using a USB blackmagic probe) with specified ELF file

  * `./flash` with no arguments will use the most recent build from _builds/_


## To write a test program for the 'uos3-proto' circuit board

1. Install the software dependencies below

2. Create a C source file in _src/main/_, eg. *test.c*. A minimal example is below.
```
#include "../firmware.h"

int main(void)
{
  Board_init();

  /* Test Code goes here */
}
```

  * *firmware.h* should include all board header files.
  * `Board_init()` should run hardware setup functions required for the board.
  * The C source file must contain *main()* as this is the function that will be run on the hardware.

3. Compile the application with: `./build uos3-proto test`

4. Flash the test program to the circuit board with: `./flash`

## Dependencies 

1. Enable user access to serial devices (programmer)
    * Ubuntu:
      ```
      sudo gpasswd --add "$USER" dialout
      ```

2. Install Latest Official ARM-Embedded Toolchain
    * Ubuntu:
      ```
      sudo apt-add-repository ppa:team-gcc-arm-embedded/ppa
      sudo apt update
      sudo apt install gcc-arm-embedded
      ```

### For TI tm4c (eg. UoS3)

3. Acquire and unpack TI Tiva Driver Package
  * Due to the licensing agreement, this has to be manually added.
  * Download [SW-TM4C-2.1.4.178.exe](http://software-dl.ti.com/tiva-c/SW-TM4C/latest/index_FDS.html) (login required)
  * Unpack to _src/platform/tm4c123g/TivaWare_C_Series-2.1.4.178_
  * Ask Phil if you have difficulties

# Contact

Phil Crump <phil@philcrump.co.uk>