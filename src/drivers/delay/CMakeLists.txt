# Delay driver

if (${UOS3_TARGET_TM4C})
    add_library(Delay
        Delay_public_tm4c.c
    )
else()
    add_library(Delay
        Delay_public_linux.c
    )
endif()
target_link_libraries(Delay
    ${STANDARD_LINK_LIBS}
)