# UoS³ Flight Computer Firmwware

> :warning: This repo is under development

## Development

### Software Writing Standards

There is a standards document that you should follow when writing code for this
repository, it can be found [here](docs/standards/sws.md).

### Requirements

Developing this repository requires the following software:

- `build-essential` - generic build utilities for GNU.
- `gcc-arm-none-eabi` - the GCC cross-compiler for ARM targets
- `arm-none-eabi-gdb` - the ARM target GDB
- CMake - used as the build system
- `lm4flash` - used to flash the TM4C launchpad
- Python 3 - used for build script
- cmocka - framework used to test the software
- OpenOCD - debugging host which allows GDB to be used to debug the software
- TinyDB and rich - python libraries used in the constant database and lookup
  tool
- pyserial - python module used for serial monitoring/in some tests
- tomlc99 - TOML parsing library used for configuration management

On ubuntu the following command should install most of these:

```shell
sudo apt install build-essential gcc-arm-none-eabi cmake lm4flash openocd
pip install tinydb rich pyserial
```

For `python3`, be careful as you probably already have it installed.

cmocka must be installed from source. Download version 1.1.5 from 
[here](https://cmocka.org/files/1.1/) (cmocka-1.1.5.tar.xz for ubuntu), extract
it and follow the instructions under `INSTALL.md`.

You must also get a copy of the TivaWare drivers from [the
website](https://www.ti.com/tool/SW-TM4C). Extract these one level up from this
repository and create a symbolic link to that folder in this one:

```bash
cd <PARENT OF THIS REPO>
mkdir tivaware_tm4c_<VERSION NUMBER> && cd tivaware_tm4c_<VERSION NUMBER>
unzip <PATH TO DOWNLOADED SW-TM4C-<VERSION NUMBER>.exe> .
make
cd ../obc-firmware
ln -s ../tivaware_tm4c_<VERSION NUMBER> tivaware
```

Finally `arm-none-eabi-gdb` must be installed from source from [ARM's
website](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).

### Recommended VSCode extensions

It is recommended to use Visual Studio Code for development with the following
extensions:

- [GitLens](https://marketplace.visualstudio.com/items?itemName=eamodio.gitlens)
- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)
- [Doxygen Documentation Generator](https://marketplace.visualstudio.com/items?itemName=cschlosser.doxdocgen)
- [Python](https://marketplace.visualstudio.com/items?itemName=ms-python.python)
- [Rewrap](https://marketplace.visualstudio.com/items?itemName=stkb.rewrap)
- [Spell Right](https://marketplace.visualstudio.com/items?itemName=ban.spellright)
- [Todo Tree](https://marketplace.visualstudio.com/items?itemName=Gruntfuggly.todo-tree)

To make Todo Tree highlight TODOs in C block comments change the extension
setting `Todo-tree > Regex` to:

```regex
((//|#|<!--|;|/\*|\s?\*|^)\s*($TAGS)|^\s*- \[ \])
```

### Building the firmware

To build the firmware a build script is provided at `./build`. To use it, check
that the script is executable with

```shell
chmod +x ./build
```

then run 

```shell
./build --help
```

to see help information.

The build files will be output into a `builds` directory, subsequently divided
into `release` and `debug` mode builds. 

## Testing

Most (TODO: all) modules provide unit tests using the `cmocka` unit test
framework. These tests are collected into a binary `test_all_modules.bin`,
which is run as a part of the GitHub actions CI system, and should be run by
the developer before any commit to ensure no regression takes place. 

To build these tests use `./build --tests`. Currently this test has not been
proven to work on the TM4C, though this will be done soon.

In addition `valgrind` should be used to verify that there are no memory leaks
and that the total heap size is kept small. 

```shell
valgrind --tool=memcheck --leak-check=full <PATH_TO_BIN>
valgrind --tool=massif --stacks=yes <PATH_TO_BIN>
```

You can use [massif-visualizer](https://stackoverflow.com/questions/1623771/valgrind-massif-tool-output-graphical-interface)
to see the memory graphs in a nice format.

## Flashing

The `./flash` script can be used to flash specific ELF files to a TM4C, either
a launchpad or the TOBC itself. Make the script executable with

```shell
chmod +x ./flash
```

the run using

```shell
./flash <PATH_TO_ELF>
```

You can use `./flash --help` to see help information. The script expects ELF
files that have been built using `./build -t tm4c`.

You can also flash immediately after building, for instance:
```shell
./build -t launchpad --flash demo_imu.elf
```

## Debugging

### Launchpad

OpenOCD and GDB can be used to debug the software on the launchpad. To use 
these in one terminal start OpenOCD (`openocd`) with the launchpad connected 
over USB to the debug port. Then in another terminal use

```shell
arm-none-eabi-gdb -x openocd.gdb <PATH_TO_ELF>.elf
```

to start the GDB session. The software will immediately run. Make sure to use
the ELF file not a binary, as GDB and OpenOCD will handle the flashing process
for you. You can put breakpoints in the code using `__asm("BKPT")`. Make sure
to remove these before testing without the debugging setup as they will halt 
execution.

If you're encountering faults using GDB will allow you to see where the faults
are coming from. If you see a `SIGTRAP` with the GDB frame being
`debug_fault_handler`, you can inspect the `p_frame` input to see what function
caused the fault:
```shell
p/a *p_fault
$1 = {
  r0 = 0x40037000, 
  r1 = 0x4000022, 
  r2 = 0x0 <g_pfnVectors>, 
  r3 = 0x200000a8 <TIMER_STATE+168>, 
  r12 = 0xf801312e, 
  lr = 0x1fbb <Timer_enable+38>, 
  return_address = 0x2a18 <TimerConfigure>, 
  xpsr = 0x21000000
}
```
This gives information on the function causing the fault (`return_address` and
`lr`, and the inputs to those functions `r1-12`.)

### TOBC

When connected via the BlackMagic probe the TOBC can be debugged with
`arm-none-eabi-gdb` directly. Use

```shell
arm-none-eabi-gdb -x blackmagic_debug.gdb <PATH_TO_ELF>.elf
```

to start the GDB session. You can run the software by using the standard GDB
commands like `run`.

## Monitoring Execution

The firmware uses the `DEBUG_x` statements to print debugging log lines over
UART to a host computer. This allows the execution of the system to be observed
by the operator. The UART used varies between the launchpad and TOBC:

- launchpad - UART1, which is on pins PB0 (RX) and PB1 (TX), these are
  accessible from the launchpad header and can be plugged into the UART pins of
  a raspberry pi for monitoring.
- TOBC - UART4, which is on pins PC4 (RX) and PC5 (TX), and are broken out on
  the PC104 header stack as pins H1P1 and H2P5. These can be connected into the
  UART lines of the blackmagic probe.

When connected the serial devices can be accessed on a linux machine (including
a raspberry pi) at any of the `/dev/tty*` devices. Normally if using the
blackmagic probe this is `/dev/ttyACM1`, and for the launchpad this is
`/dev/serial0`. The user should be in the `dialout` group to access these, some
additional setup may be required on the PI to enable the right serial lines.

To view these log lines the `pyserial` module provides an application which
displays the output:

```shell
python3 -m serial.tools.miniterm /dev/<SERIAL_DEVICE> 115200 --raw
```

The baud rate of the debug serial is 115200, the fastest standard baud rate so
that delays to the execution of the firmware are minimised.

## Build Features

A number of features may be enabled by a build, which can be seen by calling
```shell
./build --list-features
```

Features allow certain parts of the code to be excluded from particular builds,
for instance the IMU calibration code is not required in the flight version of
the software, so it's inclusion in the build is only enabled when using the
`imu_calib` feature.

## Constants

The software uses 16-bit constants for error, event and datapool ID codes.
There are many different codes and looking them up can be a challenge,
therefore a database of all codes and a short python tool to search that
database is provided.

The databse is generated automatically as a part of the build process. The
lookup tool can be used as such:

```shell
python src/tools/tool_const_lookup.py 0x940A
```

and will return

```shell
Events

    EVT_IMU_STATE_CHANGE: 0x940A (37898, 0b1001010000001010)
        Event used to signal a state change in the IMU. This is done so that 
        the new state will run it's first function in the next cycle without 
        the system going to sleep.

Errors
    None

```