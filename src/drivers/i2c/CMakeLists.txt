# CMakeLists.txt for the I2c module

# Library declaration
add_library(I2c
    I2c_public.c
    I2c_private.c
)
target_link_libraries(I2c
    ${STANDARD_LINK_LIBS}
)