# Taken from https://gitlab.com/yusefkarim/cmake-tm4c-blinky/-/blob/master/tm4c123g.cmake

#Set cross compilation information
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# GCC toolchain prefix
set(TOOLCHAIN_PREFIX "arm-none-eabi")

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-as)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}-ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}-objdump)

enable_language(ASM)

set(CPU "-mcpu=cortex-m4")
set(FPU "-mfpu=fpv4-sp-d16 -mfloat-abi=softfp")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mthumb ${CPU}  ${FPU} -MD")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb ${CPU} ${FPU} -std=gnu99 -Os -ffunction-sections -fdata-sections -MD -Wall -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthumb ${CPU} ${FPU}  -Os -ffunction-sections -fdata-sections -MD -Wall -pedantic -fno-exceptions -fno-rtti")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "-T${PROJECT_SOURCE_DIR}/src/link/tm4c123g.ld -specs=${PROJECT_SOURCE_DIR}/src/link/tiva.specs")

# Processor specific definitions
# TODO: Check revision version is correct
add_definitions(-DPART_TM4C123GH6PM)
add_definitions(-DTARGET_IS_TM4C123_RA1)
add_definitions(-Dgcc)

# TODO: This shouldn't be used but is kept for reference
set(FLASH_EXECUTABLE "lm4flash")
ADD_CUSTOM_TARGET("flash" DEPENDS demo_launchpad_blinky
  COMMAND ${CMAKE_OBJCOPY} -O binary src/demo/demo_launchpad_blinky demo_launchpad_blinky.bin 
  COMMAND ${FLASH_EXECUTABLE} demo_launchpad_blinky.bin
)
