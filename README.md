# UoS³ Flight Computer Firmwware

> :warning: This repo is under development

## Development

### Software Writing Standards

There is a standards document that you should follow when writing code for this
repository, it can be found [here](docs/standards/sws.md).

### Requirements

Developing this repository requires the following software:

- `gcc-arm-none-eabi` - the GCC cross-compiler for ARM targets
- CMake - used as the build system
- lm4flash - used to flash the TM4C launchpad
- Python 3 - used for build script
- cmocka - framework used to test the software

On ubuntu the following command should install most of these:

```shell
sudo apt install gcc-arm-none-eabi cmake lm4flash
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