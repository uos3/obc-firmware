# CMakeLists.txt for the EventManager module

# Library declaration
add_library(EventManager
    EventManager_public.c
)
target_link_libraries(EventManager
    ${STANDARD_LINK_LIBS}
)