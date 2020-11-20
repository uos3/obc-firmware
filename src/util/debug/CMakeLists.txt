# CMakeLists.txt for the Debug module

add_library(Debug
    Debug_public.c
)
target_link_libraries(Debug
    ${TIVAWARE_LIBS}
)