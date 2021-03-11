# MemStoreManager CMakeLists.txt

# Library declaration
add_library(MemStoreManager
    MemStoreManager_public.c
    MemStoreManager_private.c
)
target_link_libraries(MemStoreManager
    Debug
    Board
    DataPool
    EventManager
    Eeprom
    Crypto
)

# Set the backup default software configuration file. This directly links the
# raw binary into the library. 
#
# The actual file has to be copied into each executable's build dir, so it gets
# the right "backup_cfg_file" name, which is done be redefining add_executable
# in the main CMakeLists.txt file.
message(STATUS "Using ${PROJECT_SOURCE_DIR}/config/${UOS3_BACKUP_CONFIG_FILE_NAME} as backup config file")
target_link_options(MemStoreManager
    PUBLIC "LINKER:--format=binary,backup_cfg_file,--format=default"
)