# CMakeLists.txt for tests.

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
)

add_executable(test_mem_store_mgr_config
    ${STARTUP_SOURCE}
    test_mem_store_mgr_config.c
)
target_link_libraries(test_mem_store_mgr_config
    ${STANDARD_LINK_LIBS}
)

add_executable(test_power
    ${STARTUP_SOUCRE}
    test_power.c
)
target_link_libraries(test_power
    ${STANDARD_LINK_LIBS}
    Power
    OpModeManager
)

add_executable(test_gpio
    ${STARTUP_SOUCRE}
    test_gpio.c
)
target_link_libraries(test_gpio
    ${STANDARD_LINK_LIBS}
    Gpio
    Led
)

add_executable(test_uart
    ${STARTUP_SOUCRE}
    test_uart.c
)
target_link_libraries(test_uart
    ${STANDARD_LINK_LIBS}
    Uart
    Udma
    Gpio
    Led
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