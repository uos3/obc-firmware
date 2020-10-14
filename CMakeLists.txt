
# Minimum CMake version here is the one that ships with Ubuntu 20.04
cmake_minimum_required(VERSION 3.16.3)

project(obc_firmware)

# CMake file for the TM4C123G, used to specify the custom link script
include(${PROJECT_SOURCE_DIR}/src/link/tm4c123g.cmake)

# Include the tivaware files. Must setup the tivaware symbolic link first, see
# readme for info.
set(TIVAWARE_PATH "${PROJECT_SOURCE_DIR}/tivaware")
include_directories(${TIVAWARE_PATH})

# Single TIVAWARE_LIBS var so linking to tivaware is easy
set(TIVAWARE_LIBS
    ${TIVAWARE_PATH}/driverlib/gcc/libdriver.a
)

# Set the standard link set, which is Startup + Tivaware
set(STANDARD_LINK_LIBS
    ${TIVAWARE_LIBS}
)

# Set the startup source file location. This must be inclued in the source list
# for all executables that should be run on the TM4C, otherwise the ResetISR
# will not be set and the app won't be able to start.
set(STARTUP_SOURCE
    ${PROJECT_SOURCE_DIR}/src/tm4c_startup_gcc.c
)

# Add subdirectories, each managed by its own CMakeLists.txt.
# The ordering here is important as subdirs which have dependencies on other
# must be added AFTER those dependencies have been made.
add_subdirectory(src/demo)
