# CMakeLists.txt for the DataPool module

# Generate the datapool files using the python tool
execute_process(
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/dp_generation_test.py "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Library declaration
add_library(DataPool
    DataPool_public.c
    DataPool_generated.c
)
target_link_libraries(DataPool
    ${STANDARD_LINK_LIBS}
)