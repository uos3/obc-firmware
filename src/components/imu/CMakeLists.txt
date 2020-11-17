# IMU component CMakeLists.txt


option(UOS3_IMU_CALIB "Enable calibration tooling" OFF)

if (${UOS3_IMU_CALIB})
    add_definitions(-DF_IMU_CALIB)
endif()

if (${UOS3_IMU_CALIB})
    add_library(Imu
        Imu_public.c
        Imu_private.c
        Imu_calib.c
    )
else()
    add_library(Imu
        Imu_public.c
        Imu_private.c
    )
endif()


target_link_libraries(Imu
    ${STANDARD_LINK_LIBS}
    I2c
)