# CMakeLists.txt for the Board module

# Library declaration
add_library(Board
    Board_public.c
)
target_link_libraries(Board
    DataPool
)