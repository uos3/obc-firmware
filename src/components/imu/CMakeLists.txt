# IMU component CMakeLists.txt

add_library(Imu
    Imu_public.c
    Imu_private.c
)
target_link_libraries(Imu
    ${STANDARD_LINK_LIBS}
    I2c
)