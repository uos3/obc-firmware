"""
Created on 2021-03-26

Raspberry Pi UART communication tool for UART uDMA functional test.

@author: Leo Galanakis (lg5g16@soton.ac.uk)
"""

import serial
from time import sleep

ser = serial.Serial("dev/ttyS0", 56000) #open port and set baud rate
while True:
  received_data = ser.read() #read serial port
  sleep(0.03)
  data_left = ser.inWaiting() #check for remaining bytes
  received_data += ser.read(data_left)
  print(received_data) #print received data 
  ser.write(received_data) #transmit data back
