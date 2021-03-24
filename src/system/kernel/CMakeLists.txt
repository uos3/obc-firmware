# CMakeLists.txt for Kernel module

add_library(Kernel
    Kernel_public.c
)
target_link_libraries(Kernel
    Debug
    Board
)