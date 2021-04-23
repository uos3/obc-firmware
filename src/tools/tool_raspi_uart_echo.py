"""
Created on 2021-03-26

Raspberry Pi UART communication tool for UART uDMA functional test.

@author: Leo Galanakis (lg5g16@soton.ac.uk)
"""

import serial
from time import sleep

ser = serial.Serial("/dev/ttyUSB0", 56000) #open port and set baud rate

data = b''

while True:
    print("Waiting for data to receive")
    recvd_data = ser.read(7)
    sleep(0.03)
    data_left = ser.inWaiting()
    recvd_data += ser.read(data_left)
    print(recvd_data)
    send = [r + 1 for r in recvd_data]
    print(f'Sending {send}')
    ser.write(send)

    """
    if ser.in_waiting > 0:
        data += ser.read()
        
        if (len(data) >= 255):
            print(data)
            ser.write(data)
            data = b''
            exit(0)
    """
