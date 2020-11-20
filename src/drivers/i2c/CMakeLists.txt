# CMakeLists.txt for the I2c module

# Library declaration
if (${UOS3_TARGET_TM4C})
    add_library(I2c
        I2c_public_tm4c.c
        I2c_private_tm4c.c
    )
else()
    add_library(I2c
        I2c_public_linux.c
        I2c_private_linux.c
    )
endif()

target_link_libraries(I2c
    ${STANDARD_LINK_LIBS}
)