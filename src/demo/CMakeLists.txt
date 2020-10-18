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
endif()
