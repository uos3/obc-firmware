# Build the main firmware exec
add_executable(obc_firmware
    ${STARTUP_SOURCE}
    obc_firmware_main.c
    obc_firmware_init.c
    obc_firmware_step.c
)
target_link_libraries(obc_firmware
    # Includes critical modules
    ${STANDARD_LINK_LIBS}

    # Drivers
    Timer
    Gpio
    #Uart
    I2c
    Rtc

    # System
    OpModeManager

    # Components
    Eps
    Led
    Imu

    # Applications
    Power

)