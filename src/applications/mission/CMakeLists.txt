# Mission App CMakeLists.txt

add_library(Mission
    Mission_public.c
)
target_link_libraries(Mission
    ${STANDARD_LINK_LIBS}
)