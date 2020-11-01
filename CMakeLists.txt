
# Minimum CMake version here is the one that ships with Ubuntu 20.04
cmake_minimum_required(VERSION 3.16.3)

# Project declaration
project(obc_firmware)

# Option to switch between TM4C build and Linux build
option(UOS3_TARGET_TM4C "If set the build shall target the TM4C" OFF)

# Option to enable test building or not
option(UOS3_BUILD_TESTS "If set cmocka tests will be build" OFF)

# Export compile commands so VSCode can use the right definitions
set(CMAKE_EXPORT_COMPILE_COMMANDS "1")

# Use the C99 standard
set(CMAKE_C_STANDARD 99)

# Set all warnings on

# Use pedantic
set(CMAKE_C_FLAGS "-Wall -pedantic")

# If building for the TM4C
if(${UOS3_TARGET_TM4C})
    message("")
    message("OBC Firmware building for: TM4C")
    message("")

    # Set the tm4c define
    add_definitions(-DTARGET_TM4C)

    # CMake file for the TM4C123G, used to specify the custom link script
    include(${PROJECT_SOURCE_DIR}/src/link/tm4c123g.cmake)

    # Include the tivaware files. Must setup the tivaware symbolic link first, 
    # see readme for info.
    set(TIVAWARE_PATH "${PROJECT_SOURCE_DIR}/tivaware")
    include_directories(${TIVAWARE_PATH})

    # Single TIVAWARE_LIBS var so linking to tivaware is easy
    set(TIVAWARE_LIBS
        ${TIVAWARE_PATH}/driverlib/gcc/libdriver.a
    )

    # Set the standard link set, which is Startup + Tivaware
    set(STANDARD_LINK_LIBS
        ${TIVAWARE_LIBS}
        Debug
    )

    # Set the startup source file location. This must be inclued in the source
    # list for all executables that should be run on the TM4C, otherwise the 
    # ResetISR will not be set and the app won't be able to start.
    set(STARTUP_SOURCE
        ${PROJECT_SOURCE_DIR}/src/tm4c_startup_gcc.c
    )

# If building on linux
elseif(UNIX)
    message("")
    message("OBC Firmware building for: UNIX")
    message("")

    # Set the unix define
    add_definitions(-DTARGET_UNIX)

    # Set the C/CXX compilers
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)

    # Set tivaware libs, startup source, and standard link libs as empty, this 
    # will result in them not being linked.
    set(TIVAWARE_PATH "")
    set(TIVAWARE_LIBS )
    set(STANDARD_LINK_LIBS 
        Debug
    )
    set(STARTUP_SOURCE )

# If anything else throw an error
else()
    message(FATAL_ERROR "OBC Firmware only supports Unix and TM4C targets, please recompile on either Linux or cross-compile for the TOBC MCU")
endif()

# Set the executable suffix to be ".bin", this is used to move all the binaries
# into a single build folder
set(CMAKE_EXECUTABLE_SUFFIX ".bin")

# Set src as an include directory
include_directories(src/)

# Set debug mode define if needed
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG_MODE")

# Add subdirectories, each managed by its own CMakeLists.txt.
# The ordering here is important as subdirs which have dependencies on other
# must be added AFTER those dependencies have been made.
add_subdirectory(src/util)
add_subdirectory(src/system)
add_subdirectory(src/demo)

# Build the tests if required
if(${UOS3_BUILD_TESTS})
    message("")
    message("OBC Firmware building tests")
    message("")

    add_subdirectory(src/test)
endif()
