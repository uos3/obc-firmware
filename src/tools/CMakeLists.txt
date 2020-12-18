# CMakeLists for generic tools

# Find tomlc99 for tool_config_pack
find_library(TOML_LIB
    NAMES toml libtoml
)

# Config Pack Tool
if(TOML_LIB)
    add_executable(tool_config_pack
        tool_config_pack.c
    )
    target_link_libraries(tool_config_pack
        ${TOML_LIB}
        Debug
        Crypto
    )
    # Exclude from all build, need to parse tool_config_pack
    set_target_properties(tool_config_pack
        PROPERTIES EXCLUDE_FROM_ALL 1
    )
endif()

# Config Flash Tool
if (NOT "${UOS3_CFG_FILE_PATH}" STREQUAL "")
    add_executable(tool_config_flash
        ${STARTUP_SOURCE}
        tool_config_flash.c
    )
    target_link_libraries(tool_config_flash
        ${STANDARD_LINK_LIBS}
        Eeprom
        MemStoreManager
    )
    configure_file("${PROJECT_SOURCE_DIR}/${UOS3_CFG_FILE_PATH}" "cfg_file" COPYONLY)
    target_link_options(tool_config_flash
        PRIVATE "LINKER:--format=binary,cfg_file,--format=default"
    )
    # Exclude from all build, need to parse tool_config_pack
    set_target_properties(tool_config_flash
        PROPERTIES EXCLUDE_FROM_ALL 1
    )
endif()