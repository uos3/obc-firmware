# CMakeLists.txt for the GPIO module

# Library declaration
if (${UOS3_TARGET_TM4C})
    add_library(Gpio
        Gpio_private_tm4c.c
        Gpio_public_tm4c.c
    )
else()
    add_library(Gpio
        Gpio_public_linux.c
    )
endif()

target_link_libraries(Gpio
    ${STANDARD_LINK_LIBS}
)