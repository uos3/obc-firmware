# CMakeLists.txt for the EventManager module

# Library declaration
add_library(EventManager
    EventManager_public.c
    EventManager_private.c
)
target_link_libraries(EventManager
    Debug
    DataPool
)