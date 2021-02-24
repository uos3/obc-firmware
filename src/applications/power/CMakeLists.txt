# Power App CMakeLists.txt

add_library(Power
    Power_public.c
    Power_private.c
)
target_link_libraries(Power
    ${STANDARD_LINK_LIBS}
    Eps
    Timer
)