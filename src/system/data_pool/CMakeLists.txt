# CMakeLists.txt for the DataPool module

# Library declaration
add_library(DataPool
    DataPool_public.c
)
target_link_libraries(DataPool
    ${STANDARD_LINK_LIBS}
)