"""
Created on 2021-03-26

Raspberry Pi UART communication tool for UART uDMA functional test.

@author: Leo Galanakis (lg5g16@soton.ac.uk)
"""

import serial
from time import sleep

ser = serial.Serial("/dev/ttyS0", 56000) #open port and set baud rate

count = 0

while True:
  print("Waiting for bytes to receive\n")
  received_data = ser.read() #read serial port
  sleep(0.03)
  data_left = ser.inWaiting() #check for remaining bytes
  received_data += ser.read(data_left)
  print("\nDATA RECEIVED:\n")
  print(received_data) #print received data 
  print("\nConverted to string:\n")
  decoded = bytes.fromhex(received_data).decode('utf-8')
  print(decoded)
  ser.write(received_data) #transmit data back
  count += 1
