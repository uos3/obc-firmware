# CMakeLists.txt for demonstration files

# Executables supporting the Launchpad target only
if(${UOS3_TARGET_LAUNCHPAD})
    # Launchpad blinky demo 
    add_executable(demo_launchpad_blinky
        ${STARTUP_SOURCE}
        demo_launchpad_blinky.c
    )
    target_link_libraries(demo_launchpad_blinky
        ${STANDARD_LINK_LIBS}
    )
endif()

# Executables supporting both targets

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

# Imu demo
add_executable(demo_imu
    ${STARTUP_SOURCE}
    demo_imu.c
)
target_link_libraries(demo_imu
    ${STANDARD_LINK_LIBS}
    Imu
)

# Timer demo
add_executable(demo_timer
    ${STARTUP_SOURCE}
    demo_timer.c
)
target_link_libraries(demo_timer
    ${STANDARD_LINK_LIBS}
    Timer
)