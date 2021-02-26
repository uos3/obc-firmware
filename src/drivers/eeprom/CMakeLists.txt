# CMakeLists.txt for the Eeprom module

# Library declaration
if (${UOS3_TARGET_TM4C})
    add_library(Eeprom
        Eeprom_public_tm4c.c
        Eeprom_private_tm4c.c
    )
else()
    add_library(Eeprom
        Eeprom_public_linux.c
    )
endif()

target_link_libraries(Eeprom
    Kernel
    Debug
    Board
    DataPool
    EventManager
)