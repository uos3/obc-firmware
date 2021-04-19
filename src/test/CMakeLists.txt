# CMakeLists.txt for tests.

if(NOT DEFINED ${UOS3_TARGET_TM4C})
    # test_all_modules only works on unix as cmocka isn't cross-compiled for
    # the TM4C, so we don't try to build it if we're on that target.
    FIND_PACKAGE(CMOCKA REQUIRED)
    INCLUDE_DIRECTORIES(${LIBCMOCKA_INCLUDE_DIR})

    add_executable(test_all_modules
        ${STARTUP_SOURCE}
        test_all_modules.c
    )
    target_link_libraries(test_all_modules
        ${STANDARD_LINK_LIBS}
        ${CMOCKA_LIBRARY}
        Crypto
        Power
        OpModeManager
        Rtc
    )
endif()

add_executable(test_mem_store_mgr_config
    ${STARTUP_SOURCE}
    test_mem_store_mgr_config.c
)
target_link_libraries(test_mem_store_mgr_config
    ${STANDARD_LINK_LIBS}
)

add_executable(test_power
    ${STARTUP_SOURCE}
    test_power.c
)
target_link_libraries(test_power
    ${STANDARD_LINK_LIBS}
    Power
    Rtc
    Timer
    OpModeManager
)

add_executable(test_gpio
    ${STARTUP_SOURCE}
    test_gpio.c
)
target_link_libraries(test_gpio
    ${STANDARD_LINK_LIBS}
    Uart
    Udma
    Gpio
    Led
    EventManager
)

add_executable(test_uart
    ${STARTUP_SOURCE}
    test_uart.c
)
target_link_libraries(test_uart
    ${STANDARD_LINK_LIBS}
    Uart
    Udma
    Gpio
    Led
    EventManager
)

if(${UOS3_TARGET_LAUNCHPAD} OR ${UOS3_TARGET_TOBC})
    add_executable(test_boot_reg
        ${STARTUP_SOURCE}
        test_boot_reg.c
    )
    target_link_libraries(test_boot_reg
        ${STANDARD_LINK_LIBS}
    )
endif()

add_executable(test_delay
    ${STARTUP_SOURCE}
    test_delay.c
)
target_link_libraries(test_delay
    ${STANDARD_LINK_LIBS}
    Delay
    Rtc
)

add_executable(test_bu
    ${STARTUP_SOURCE}
    test_bu.c
)
target_link_libraries(test_bu
    ${STANDARD_LINK_LIBS}
    Delay
    Rtc
    Eps
    Power
    OpModeManager
)