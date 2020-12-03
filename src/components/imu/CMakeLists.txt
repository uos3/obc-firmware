# IMU component CMakeLists.txt


option(UOS3_IMU_CALIB "Enable calibration tooling" OFF)

if (${UOS3_IMU_CALIB})
    add_definitions(-DF_IMU_CALIB)
endif()

# Standard source files for the IMU
add_library(Imu
    Imu_public.c
    Imu_private.c
    Imu_step_init.c
    Imu_step_set_gyro_offsets.c
    Imu_step_read_temp.c
    Imu_step_read_gyro.c
    Imu_step_read_magne.c
)

# If the calibration feature is enabled add the calib file
if (${UOS3_IMU_CALIB})
    target_sources(Imu PRIVATE Imu_calib.c)
endif()


target_link_libraries(Imu
    ${STANDARD_LINK_LIBS}
    I2c
)