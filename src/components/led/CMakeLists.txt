# CMakeLists.txt for the Led module

# Library declaration
add_library(Led
    Led_public.c
    Led_private.c
)

target_link_libraries(Led
    ${STANDARD_LINK_LIBS}
)