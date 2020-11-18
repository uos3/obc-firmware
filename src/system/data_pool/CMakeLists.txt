# CMakeLists.txt for the DataPool module

# Get dependencies of DataPool_struct.h with the python script
execute_process(
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/DataPool_get_struct_dependencies.py
    OUTPUT_VARIABLE DP_STRUCT_DEPENDENCIES
)

string(REPLACE " " ";" DP_STRUCT_DEPS_LIST ${DP_STRUCT_DEPENDENCIES})

# Generate the datapool code using the python tool, if DataPool_struct.h is 
# changed
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/DataPool_generated.c ${CMAKE_CURRENT_SOURCE_DIR}/DataPool_generated.h
    DEPENDS DataPool_struct.h DataPool_generate.py ${DP_STRUCT_DEPS_LIST}
    DEPENDS DataPool_generate.py
    # IMPLICIT_DEPENDS C DataPool_struct.h
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/DataPool_generate.py
    COMMENT "Generating DataPool code"
)

# Library declaration
add_library(DataPool
    DataPool_public.c
    DataPool_generated.c
)
# DataPool doesn't link to anything else