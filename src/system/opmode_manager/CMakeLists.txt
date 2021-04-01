# OpModeManager module CMakeLists.txt

option(UOS3_BU_SHORT_DWELL_TIME "Enable short dwell time in BU mode" OFF)

if (${UOS3_BU_SHORT_DWELL_TIME})
    add_definitions(-DF_BU_SHORT_DWELL_TIME)
endif()

add_library(OpModeManager
    OpModeManager_public.c
    OpModeManager_private.c
    OpModeManager_bu.c
)
target_link_libraries(OpModeManager
    ${STANDARD_LINK_LIBS}
    Timer
)