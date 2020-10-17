# CMakeLists.txt for demonstration files

# Launchpad blinky demo
add_executable(demo_launchpad_blinky
    ${STARTUP_SOURCE}
    demo_launchpad_blinky.c
)
target_link_libraries(demo_launchpad_blinky
    ${STANDARD_LINK_LIBS}
)
