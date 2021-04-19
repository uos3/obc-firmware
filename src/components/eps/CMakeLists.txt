
add_library(Eps
    Eps_public.c
    Eps_private.c
)
target_link_libraries(Eps
    ${STANDARD_LINK_LIBS}
    Uart
    Timer
)