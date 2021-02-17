# CMakeLists.txt for the UART module

# Library declaration
if (${UOS3_TARGET_TM4C})
    add_library(Uart
        Uart_private_tm4c.c
        Uart_public_tm4c.c
    )
else()
    add_library(Uart
        Uart_public_linux.c
    )
endif()

target_link_libraries(Uart
    ${STANDARD_LINK_LIBS}
)