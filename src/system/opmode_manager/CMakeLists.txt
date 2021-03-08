# OpModeManager module CMakeLists.txt

add_library(OpModeManager
OpModeManager_public.c
)
target_link_libraries(OpModeManager
    ${STANDARD_LINK_LIBS}
)