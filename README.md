# UoS3 Cubesat Firmware

## To use

`./build [board] [program]` - will compile src/main/[program].c for [board]

  * The resultant filename will be builds/_[board]-[program]-[gitref]_

`./flash [binary]` - will flash a board (using blackmagic probe) with [binary] ELF file

  * `./flash` with no arguments will use the most recent build from _builds/_

eg.

`./build uos3-proto blinky` - builds the blinky.c program for the UoS3 Flight Computer Prototype Board

`./flash builds/uos3-proto-blinky-fd8ea0c` - flashes a built binary to a board

## To develop

To develop a new program (demonstration, test process, etc), create a new <program>.c in _src/main/_ from which you can call the relevant functions You can now build this application with `./build [board] [program]`

#### _src/main/<program>_

This folder contains unique entry files for applications, and can be thought of as the top-level logic. One of the these will be the flight program, while others may demonstrate specific functions such as Radio Testing or Sensor Control.

#### _src/*.c_

This folder contains the general logic of the cubesat, such as tasks, protocol implementations, data buffers, etc.

Subfolders may be created when suitable for modules, will need to be added to the _firmware.mk_ file to be compiled.

#### _src/board/<board>/_

These folders contain the low-level peripheral drivers for each board.

This abstraction allows an application to be compiled for another board simply by changing the <board> in the `./build ...` command.

## Dependencies

1. Enable user access to serial devices (programmer)
    * Ubuntu:
      ```
      sudo gpasswd --add "$USER" dialout
      ```

2. Install Latest ARM-Embedded Toolchain
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