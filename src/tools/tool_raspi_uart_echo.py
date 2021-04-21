"""
Created on 2021-03-26

Raspberry Pi UART communication tool for UART uDMA functional test.

@author: Leo Galanakis (lg5g16@soton.ac.uk)
"""

import serial
from time import sleep

ser = serial.Serial("/dev/ttyUSB0", 9600) #open port and set baud rate

count = 0

while True:
    print("Waiting for bytes to receive\n")
    received_data = ser.read() #read serial port
    sleep(0.03)
    data_left = ser.inWaiting() #check for remaining bytes
    received_data += ser.read(data_left)
    print("\nDATA RECEIVED:\n")
    print(received_data) #print received data 
    ser.write(received_data) #transmit data back
    count += 1
