# CMakeLists.txt for the UART module

# Library declaration
if (${UOS3_TARGET_TM4C})
    add_library(Udma
        Udma_public_tm4c.c
    )
else()
    add_library(Udma
        Udma_public_linux.c
    )
endif()

target_link_libraries(Udma
    ${STANDARD_LINK_LIBS}
)
