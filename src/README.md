# Folder structure

## driver

Header files for the system interface. The build script allows for the choice
of the corresponding systems:

### linux

For execution on GNU linux. Allows testing of high-level MCU-only code without
having to flash to the TOBC.

### TOBC

The hardware UoS3 ARM-based TOBC board.

## component

Individual UoS3 component drivers. No "main" or high-level mission
functionality, uses the driver headers. Avoid direct system calls here.
Do not include mission files.

## mission

High-level mission related code. Uses component drivers. No direct system calls
should be made from these.

## utility

Files that can be used in any of the other folders, may or may not make
system calls, should not include other UoS3 files

## main

Files that contain 'int main(){}'. Can include from other files. Demos should
go here, as this is where the build script looks when compiling

## support

Leftover files from previous/development versions of UoS3
