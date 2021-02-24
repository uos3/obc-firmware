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