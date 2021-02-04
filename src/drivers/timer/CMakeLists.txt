# CMakeLists.txt for the Timer module

# Library declaration
if (${UOS3_TARGET_TM4C})
    add_library(Timer
        Timer_public_tm4c.c
        Timer_private.c
        Timer_interrupts.c
        Timer_state.c
    )
else()
    add_library(Timer
        Timer_public_linux.c
    )
    target_link_libraries(Timer
        rt
    )
endif()

target_link_libraries(Timer
    ${STANDARD_LINK_LIBS}
)