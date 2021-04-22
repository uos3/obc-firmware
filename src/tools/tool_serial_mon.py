'''
Simple hexadecimal serial monitor
'''

import serial
from time import sleep

ser = serial.Serial("/dev/serial0", 9600)

print('Reading serial data')

while True:
    byte = ser.read()
    print(byte, end='')