"""
Created on 2021-03-26

Raspberry Pi UART communication tool for UART uDMA functional test.

@author: Leo Galanakis (lg5g16@soton.ac.uk)
"""

import serial
from time import sleep

ser = serial.Serial("/dev/ttyUSB0", 56000) #open port and set baud rate

while True:
    print("Waiting for data to receive:")
    recvd_data = ser.read(255)
    sleep(0.2)
    print(recvd_data)
    print(f'SendingC')
    ser.write(recvd_data)

    """
    if ser.in_waiting > 0:
        data += ser.read()
        
        if (len(data) >= 255):
            print(data)
            ser.write(data)
            data = b''
            exit(0)
    """
