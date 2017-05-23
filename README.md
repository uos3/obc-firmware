# UoS3 Cubesat Firmware

## To use

`./build [platform] [main]` - will compile src/main/[main].c for [platform]

`./flash [binary]` - will flash a board with [binary] ELF file

eg.

`./build tm4c blinky` - builds the blinky.c application for the TI TM4C Platform (eg. UoS3)

`./flash builds/uos3-tm4c-blinky-no_git` - flashes a built binary to a board

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

### For TI tm4c platform (eg. UoS3)

3. Acquire and unpack TI Tiva Driver Package
	* Due to the licensing agreement, this has to be manually added.
	* Download [SW-TM4C-2.1.4.178.exe](http://software-dl.ti.com/tiva-c/SW-TM4C/latest/index_FDS.html) (login required)
	* Unpack to _src/platform/tm4c/TivaWare_C_Series-2.1.4.178_
	* Ask Phil if you're not sure about this step.