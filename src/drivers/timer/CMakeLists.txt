# CMakeLists.txt for the Timer module

# Library declaration
add_library(Timer
    Timer_public.c
    Timer_private.c
    Timer_interrupts.c
    Timer_state.c
)

target_link_libraries(Timer
    ${STANDARD_LINK_LIBS}
)