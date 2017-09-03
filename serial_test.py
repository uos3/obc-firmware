# Simple python terminal
# to be developed to allow testing of UART devices easily
#
# and graphing telemetry data / displaying camera images
#
# MIT License
#
# usage: python serial_test 

import serial, time, sys

import matplotlib.pyplot as plt
import numpy as np
from drawnow import drawnow


def init_serial(baudrate):
	return serial.Serial('/dev/ttyACM1', baudrate, timeout=2)

def w(v,off): # rebuild number from two bytes
        x= (v[off*2]|(v[off*2+1]<<8))
        if (x>=0x8000) :
              x-=65536
        return x

def make_fig():
       plt.plot(gyr_x,'-', color='blue',label='Gyr x')
       plt.plot(gyr_y,'-', color='lightblue',label='Gyr y')
       plt.plot(gyr_z,'-', color='darkblue',label='Gyr z')
       plt.plot(acc_x,'-', color='red',label='Acc x')
       plt.plot(acc_y,'-', color='orange',label='Acc y')
       plt.plot(acc_z,'-', color='magenta',label='Acc z')
       plt.plot(mag_x,'-', color='green',label='Mag x')
       plt.plot(mag_y,'-', color='darkgreen',label='Mag y')
       plt.plot(mag_z,'-', color='olive',label='Mag z')
       plt.plot(temp,'-', color='yellow',label='Temp')

gyr_x=list()
gyr_y=list()
gyr_z=list()
acc_x=list() # this is where we are storing saved data
acc_y=list()
acc_z=list()
mag_x=list()
mag_y=list()
mag_z=list()
temp=list()
time=list()


def capture_data_block(s,n):     
       i=0
     
       plt.ion()
       fig=plt.figure()

       print "Preparing to capture data - {} consecutive packets".format(n+1)
       
       for i in range (0, n): # plot for this long
          while True:                 # loop through serial data until find start of frame marker
            c=s.read(1)
            if (c=='@'):
                       break
          gyr=map (ord,s.read(6))
          acc=map (ord,s.read(6))
          mag=map (ord,s.read(6))
          tem=map (ord,s.read(2))
           
          gyr_x.append(w(gyr,0)) 
          gyr_y.append(w(gyr,1)) 
          gyr_z.append(w(gyr,2))
          acc_x.append(w(acc,0)) 
          acc_y.append(w(acc,1)) 
          acc_z.append(w(acc,2))
          mag_x.append(w(mag,0)) 
          mag_y.append(w(mag,1)) 
          mag_z.append(w(mag,2))
          temp.append(w(tem,0)) 
          time.append(i)
       
          plt.plot(gyr_x,'-', color='blue',label='Gyr x')
          plt.plot(gyr_y,'-', color='lightblue',label='Gyr y')
          plt.plot(gyr_z,'-', color='darkblue',label='Gyr z')
          plt.plot(acc_x,'-', color='red',label='Acc x')
          plt.plot(acc_y,'-', color='orange',label='Acc y')
          plt.plot(acc_z,'-', color='magenta',label='Acc z')
          plt.plot(mag_x,'-', color='green',label='Mag x')
          plt.plot(mag_y,'-', color='darkgreen',label='Mag y')
          plt.plot(mag_z,'-', color='olive',label='Mag z')
          plt.plot(temp,'-', color='yellow',label='Temp')
          drawnow(make_fig)
          i=i+1
      
       print "Finished Graphing data"
       ioff()
       plt.axis([0,n,-32768,32767])
       plt.legend()
       plt.grid()
       plt.title("IMU data sampled at {} consecutive packets".format(n+1))
       plt.draw()
       plt.show()
       plt.savefig('imu_perf.jpg',dpi=300)
       plt.close()
   
       

def main():
	s = init_serial(9600)
        print "Analysing serial input..."

        capture_data_block(s,100) # serial port and samples

# this is the code for just displaying the data, thx to pythons wonderfully simple commenting it is a pita to comment out

    #   while True:
    #      while True:                 # loop through serial data until find start of frame marker
    #        c=s.read(1)
    #        if (c=='@'):
    #                   break
    #      gyr=map (ord,s.read(6))
    #      acc=map (ord,s.read(6))
     #     mag=map (ord,s.read(6))
      #    tem=map (ord,s.read(2))
       #    
        #  gyr_x=w(gyr,0) 
         # gyr_y=w(gyr,1) 
          #gyr_z=w(gyr,2)
          #acc_x=w(acc,0) 
          #acc_y=w(acc,1) 
          #acc_z=w(acc,2)
          #mag_x=w(mag,0) 
          #mag_y=w(mag,1) 
          #mag_z=w(mag,2)
          #temp=w(tem,0) 
          
          #print "FRAME! A:({},{},{}) G:({},{},{}) M:({},{},{}) T:{}".format(gyr_x,gyr_y,gyr_z,acc_x,acc_y,acc_z,mag_x,mag_y,mag_z,temp)
           
if __name__ == "__main__":
    main()
