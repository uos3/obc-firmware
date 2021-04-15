"""
Created on 2021-03-26

Raspberry Pi UART communication tool for UART uDMA functional test.

@author: Leo Galanakis (lg5g16@soton.ac.uk)
"""

import serial
from time import sleep

# NOTE: ttyAMA0 is UART, ttyS0 would be mini UART. 9600 is an arbitrary
# baud rade, must check.

#Opens the port with a specified baud rate.
serial_port = serial.Serial("/dev/ttyAMA0", 9600)

# Reads data from the port (4 bytes for this test, must be the same data
#  size as in test_uart.c in the obs). inWaiting checks if there are any
#  remaining bytes to receive.
data_recvd = serial_port.read(4)
data_recvd += serial_port.inWaiting()

# Wait.
sleep(0.05)

# Send the same data to the TX (test_uart.c will then check to see if both the
#  send and recv have functioned as required).
serial_port.write(data_recvd)
