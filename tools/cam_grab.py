# cam_grab.py
# dumps memory of chip
#
# usage: python cam_grab <target.jpg) - defaults to test.jpg
# S Lucarotti to test with camtest

import serial, time, sys


def init_serial(baudrate):
	return serial.Serial('/dev/ttyACM1', baudrate, timeout=2)


def write_word(s,x):
        b1=(x>>8)&255 
        b2=x & 255 
        s.write(chr(b1))
        s.write(chr(b2))

def write_word4(s,x):
        b1=(x>>24)&255 
        b2=(x>>16)&255 
        b3=(x>>8)&255 
        b4=x & 255 
      #  print "writing word {0:x},{1:x},{2:x},{3:x}".format(b1,b2,b3,b4)
        s.write(chr(b1))
        s.write(chr(b2))
        s.write(chr(b3))
        s.write(chr(b4))


def read_word4(s):
     b1=ord(s.read())
     b2=ord(s.read()) 
     b3=ord(s.read()) 
     b4=ord(s.read())
     print "reading word {0:x},{1:x},{2:x},{3:x}".format(b1,b2,b3,b4)
     return (b1<<24)|(b2<<16)|(b3<<8)|b4

def write_file(fname,data):
        print "Writing '{0}'".format(fname)
    	file = open(fname,'wb')
	file.write(''.join(map(chr,data)))
	file.close()

def get_mem(start,length,s):
 # max gettable with this python function is 16k (16384) or start to run into string indexing problems
        if (length>16384):
           print "Max grab size 16k per chunk"
           raise ValueError("Get_Mem failure")
      #  string="getting memory: start : 0x{0:x} : length : 0x{1:x}".format(start,length)
       # string+=" "+'@'+(start&255)+((start>>8)&255)+chr((start>>16)&255)+chr((start>>24)&255)+" "+chr(length&255)+chr(length>>8)
      #  print string

        s.write('@')
        write_word4(s,start)
        write_word(s,length)

        #re=0
       # while (re!='A'):
         #    re=s.read(1) # skip any rubbish on line
          #   print "...> {0} ='{1}'".format(ord(re),re)

    
        re=0
        while (re!='@'):
             re=s.read(1) # skip any rubbish on line
             print re,
        ret=s.read(length+2)
        print "read length : 0x{0:x}".format(len(ret))
       # wait=raw_input()
        if (len(ret)<length+2):
               length=len(ret)-2 # adjust length to read as much as possible
               print "did not return all values, only 0x{0:x}".format(len(ret)-2)
               raise ValueError("Get_Mem failure")
        ans=[]
        check=0
        for i in range(0,length):
             c=ord(ret[i])
             ans.append(c) # this is a hack to get it all to work, inefficient, but this should only be for debugging
             check^=c
   #          print " -> {0} = '{1}' i={2}".format(c,chr(c),i)
    #    print "========="
        if (ret[length]!='|'):
            print "end of block not found"
            raise ValueError("Get_Mem failure")
        else:
            checksum=ord(ret[length+1])
    #        print "{0} ?= {1}".format(checksum,check)
            if checksum!=check: 
                print "checksum doesn't match"  
                raise ValueError("Get_Mem failure")
   #     print tuple(ans) 
   #     wait=raw_input()   
        return tuple(ans)

def get_chunks(base_addr,total_size,chunksize,s):
     ch=[]
     cur_chunk=chunksize
     chunks=((total_size-1)/chunksize)+1
     print "Getting chunks"
     print "total_size = {0}".format(total_size)
     print "chunksize = {0}".format(chunksize)
     print "chunks = {0}".format(chunks)
     print "remainder = {0}".format(total_size-(chunks-1)*chunksize)
     print "press a key"
     wait=raw_input()
   #  chunksize=0x4000
     for j in range(0,chunks):
        print j
        if (j==chunks-1): 
            cur_chunk=total_size-j*chunksize
        ch+=get_mem(base_addr+j*chunksize,cur_chunk,s)
     return tuple(ch)

def show_debug(wait_char):
    c=s.read()
    while (c!=wait_char and c!=''):
        if (c!=''):
            if (ord(c)>31):
                print c,
            else:
                print " [{0:x}]".format(ord(c)),

        else:
            print ".(empty UART)",
        c=s.read()     

def send_packet(cmd,value):
    packet_not_sent=True
    while packet_not_sent:
        print "sending packet {0}".format(cmd)
        s.write(cmd) 
        write_word4(s,value) # this takes picture and stores in memory
        s.flush()
        print "awaiting acknowledgement"
      #  time.sleep(.1) # delay so can catch acknowledgement
        c=s.read()
     #   print "'{0}'".format(c)
        if (c=='A'):
            c=s.read(3)
            if (len(c)>=3 and (c[0]=='C') and c[1]=='K' and c[2]==cmd): 
                packet_not_sent=False
    print "packet successfully received"

def receive_packet(cmd):
    print "Receiving packet of type : '{0}'".format(cmd)
    c=s.read()
    while c!=cmd:
        if (c!=''):
            print c,
        c=s.read()
    data=read_word4(s)
    print "received data : {0}".format(data)
    s.write("ACK")
    s.write(cmd)
    s.flush()
    print "acknowledgement sent"
    return data

def main():
    global s
    
    s = init_serial('115200')
    print "Taking picture"

    storage_address=0x8000

    #before=get_chunks(storage_address,1,256,s)
   #write_file("before.dat",before);

    show_debug(':')
    print "press enter"
    wait=raw_input()

    send_packet('#',storage_address) # this takes picture and stores in memory
    length=receive_packet('$')
    #length=4
 
    print "waiting for response."
    
    show_debug(':')
    print "press enter"
    wait=raw_input()

    show_debug(':')

    #after=get_chunks(storage_address,256,256,s)
    #write_file("after.dat",after);

    
    print "Received picture length is : 0x{0:x}".format(length)
   

    print("Prepare to get chunks")
    wait = raw_input("PRESS ENTER TO CONTINUE.")
    print("something2")

    print "getting chunks ({0})".format(length)


    picture=get_chunks(storage_address,length,256,s) # this downloads it from memory
    # chop end off picture, delete extraneous bytes, shouldnt be longer than length

    s.close()

    filename = 'test.jpg'	
    if(len(sys.argv) > 1):
        filename = sys.argv[1]
    
    write_file(filename,picture)
    print "Image saved"


if __name__ == "__main__":
    main()
