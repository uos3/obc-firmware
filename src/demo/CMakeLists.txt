# CMakeLists.txt for demonstration files

# Executables supporting the TM4C target only
if(${UOS3_TARGET_TM4C})
    # Launchpad blinky demo 
    add_executable(demo_launchpad_blinky
        ${STARTUP_SOURCE}
        demo_launchpad_blinky.c
    )
    target_link_libraries(demo_launchpad_blinky
        ${STANDARD_LINK_LIBS}
    )

    # DataPool demo
    add_executable(demo_data_pool
        ${STARTUP_SOURCE}
        demo_data_pool.c
    )
    target_link_libraries(demo_data_pool
        ${STANDARD_LINK_LIBS}
        DataPool
    )

    # EventManager demo
    add_executable(demo_event_manager
        ${STARTUP_SOURCE}
        demo_event_manager.c
    )
    target_link_libraries(demo_event_manager
        ${STANDARD_LINK_LIBS}
        EventManager
    )
# Executables supporting UNIX only
else()

    # DataPool demo
    add_executable(demo_data_pool
        ${STARTUP_SOURCE}
        demo_data_pool.c
    )
    target_link_libraries(demo_data_pool
        ${STANDARD_LINK_LIBS}
        DataPool
    )

    # EventManager demo
    add_executable(demo_event_manager
        ${STARTUP_SOURCE}
        demo_event_manager.c
    )
    target_link_libraries(demo_event_manager
        ${STANDARD_LINK_LIBS}
        EventManager
    )
endif()

