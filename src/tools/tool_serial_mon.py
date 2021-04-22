'''
Simple hexadecimal serial monitor
'''

import serial
from time import sleep

ser = serial.Serial("/dev/ttyUSB0", 56000)

print('Reading serial data')

while True:
    byte = ser.read()
    print(byte, end='')