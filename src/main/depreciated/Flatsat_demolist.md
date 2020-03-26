# Flatsat demos list

## FSV_3.1.1.2 TOBC gets data from the EPS

filename: demo_eps.c \
status: written/untested

## FSV_3.1.2.1 FRAM functionality test

filename: demo_buffer.c \
status: TODO \
demo contents:

1. generate arbitary dataset
1. print arbitary data to UART
1. write arbitary data to FRAM, > 1 block
1. read arbitary data from FRAM
1. print to UART

## FSV_3.1.2.2 GNSS functionality and data gathering, watchdog enabled

filename: demo_gnss.c \
status: testing \
demo contents:

1. gather data from GNSS
1. print data to UART
1. confirm with known GPS data

## FSV_3.1.2.3 IMU functionality and data gathering, watchdog enabled

filename: demo_imu.c

## FSV_3.1.2.4 Camera Testing with FRAM

1. Take photo
1. Move photo from camera’s buffer to FRAM
1. Read from FRAM
1. Print to console
1. Pipe console output to jpeg, display as picture 

## FSV_3.1.2.6 Testing the GNSS functionality, with FRAM

1. !!

##	FSV_3.1.2.7 Testing the IMU functionality, with FRAM

1. !!

## FSV_3.1.2.8 Test all payload together, with Mission logic and mode transition, EPS DISABLED

1. !!

## FSV_3.1.2.9 Test all payload together EPS ENABLED

## FSV_3.3.2.1 TOBC pre-written RX/TX demo

## FSV_3.3.2.2 TOBC RX/TX value from FRAM (EPS/GNSS/IMU/Camera data)
