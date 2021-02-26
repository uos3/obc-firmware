
# Minimum CMake version here is the one that apt installs on the raspberry pi.
cmake_minimum_required(VERSION 3.13.4)

# Project declaration
project(obc_firmware)

# Target options
option(UOS3_TARGET_TOBC "If set the build shall target the TOBC board" OFF)
option(UOS3_TARGET_LAUNCHPAD "If set the build shall target the TM4C Launchpad board" OFF)

# If either of those are set add defines and set the TARGET_TM4C, so that
# there's a simple way of telling if we're running on a TM4C or not
if(${UOS3_TARGET_TOBC})
    set(UOS3_TARGET_TM4C ON)
    add_definitions(-DTARGET_TM4C_TOBC)
elseif(${UOS3_TARGET_LAUNCHPAD})
    set(UOS3_TARGET_TM4C ON)
    add_definitions(-DTARGET_TM4C_LAUNCHPAD)
endif()

# Option to enable test building or not
option(UOS3_BUILD_TESTS "If set cmocka tests will be build" OFF)

# Export compile commands so VSCode can use the right definitions
set(CMAKE_EXPORT_COMPILE_COMMANDS "1")

# Set C flags to use all warnings and the GNU-99 standard.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}\
    -std=gnu99 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Wswitch-default \
    -Wswitch-enum \
    -Wduplicated-cond \
    -Wconversion \
    -Wlogical-op \
    -Wstrict-prototypes \
    -Wmissing-prototypes \
    -Wmissing-declarations \
")

# If building for the TM4C
if(${UOS3_TARGET_TM4C})
    message("")
    message("OBC Firmware building for: TM4C")
    message("")

    # Set the executable suffix to be ".elf", this is to differentiate them
    # from those that can actually be executed on the host system (".exe") and
    # binaries to be flashed onto the TM4C (".bin")
    set(CMAKE_EXECUTABLE_SUFFIX ".elf")

    # Set the tm4c define which is used by platform dependent code.
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

    # Set the executable suffix to be ".exe", this is to show that they can be
    # executed on the host system.
    set(CMAKE_EXECUTABLE_SUFFIX ".exe")

    # Set the unix define
    add_definitions(-DTARGET_UNIX)

    # Set the C/CXX compilers
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)

    # Set tivaware libs, startup source, and standard link libs as empty, this 
    # will result in them not being linked.
    set(TIVAWARE_PATH "")
    set(TIVAWARE_LIBS )
    set(STARTUP_SOURCE )

# If anything else throw an error
else()
    message(FATAL_ERROR "OBC Firmware only supports Unix and TM4C targets, please recompile on either Linux or cross-compile for the TOBC MCU")
endif()

# Set the standard link set
set(STANDARD_LINK_LIBS
    ${TIVAWARE_LIBS}
    Kernel
    Debug
    Board
    DataPool
    EventManager
    Eeprom
    MemStoreManager
)

# Set src as an include directory
include_directories(src/)

# Set debug mode define if needed
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG_MODE")

# Add subdirectories, each managed by its own CMakeLists.txt.
# The ordering here is important as subdirs which have dependencies on other
# must be added AFTER those dependencies have been made.
add_subdirectory(src/system/kernel)
add_subdirectory(src/system/data_pool)
add_subdirectory(src/util)
add_subdirectory(src/drivers)
add_subdirectory(src/system)
add_subdirectory(src/components)
add_subdirectory(src/applications)
add_subdirectory(src/demo)
add_subdirectory(src/tools)

# Build the tests if required
if(${UOS3_BUILD_TESTS})
    message("")
    message("OBC Firmware building tests")
    message("")

    add_subdirectory(src/test)
endif()
