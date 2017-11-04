# mem_grab.py
# dumps memory of chip
#
# usage: python mem_grab <file_prefix) - defaults to test
# S Lucarotti to test with mem_download

import serial, time, sys


def init_serial(baudrate):
	return serial.Serial('/dev/ttyACM1', baudrate, timeout=2)


def write_word(s,x):
        b1=x & 255 
        b2=(x>>8)&255 
        s.write(chr(b1))
        s.write(chr(b2))

def write_word4(s,x):
        b1=x & 255 
        b2=(x>>8)&255 
        b3=(x>>16)&255 
        b4=(x>>24)&255 
        s.write(chr(b1))
        s.write(chr(b2))
        s.write(chr(b3))
        s.write(chr(b4))

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
 #       string="getting memory: start : 0x{0:x} : length : 0x{1:x}".format(start,length)
   #     string+=" "+'@'+chr(start&255)+chr(start>>8)+" "+chr(length&255)+chr(length>>8)
    #    print string

        s.write('@')
        write_word4(s,start)
        write_word(s,length)

        re=0
        while (re!='@'):
             re=s.read(1) # skip any rubbish on line
   #          print "...> {0} ='{1}'".format(ord(re),re)
        ret=s.read(length+2)
     #   print ret
    #    print "read length : 0x{0:x}".format(len(ret))
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
        if (ret[length]!='@'):
            print "end of block not found"
            raise ValueError("Get_Mem failure")
        else:
            checksum=ord(ret[length+1])
    #        print "{0} ?= {1}".format(checksum,check)
            if checksum!=check: 
                print "checksum doesn't match"  
                raise ValueError("Get_Mem failure")
    #    print tuple(ans)    
        return tuple(ans)

def a2s(arr):
	return ''.join(chr(b) for b in arr)

	return serial.Serial('/dev/ttyUSB0', baudrate, timeout=2)

def get_chunks(base_addr,chunks,chunksize,s):
     ch=[]
   #  chunksize=0x4000
     for j in range(0,chunks):
        ch+=get_mem(base_addr+j*chunksize,chunksize,s)
     return tuple(ch)

def main():
        global s
	s = init_serial('115200')
        print "Grabbing memory, please standby..."

#	while(1):
#		re = s.read(1)
#		print ':'.join(x.encode('hex') for x in re)

	s.flushInput()
        chunksize=0x2000 # 8k
         
        # memory map - chip has 32kb SRAM, 256kb flash, 2kb eeprom, internal rom

        flashstart=0            # code should be here
        sramstart=0x20000000    # can store code here, some aliases 
        peripheralstart=0x40000000 # aliases here
        eeprombase=0x400AF000
        systemcontrolbase=0x400fe000
        externalram=0x60000000      # supposed to be for data
        externaldevice=0xa0000000     # external device memory
        privateperipheralbus=0xe0000000 #nvic, system timer, system ctrl block
        reserved=0xe0100000 # don't use this

        flash=get_chunks(flashstart,256*4,256,s)
        print "Got flash : total length 0x{0:x} = {1}kb".format(len(flash),len(flash)/1024)
        sram=get_chunks(sramstart,128,256,s) # this fails with larger chunk size
        print "Got SRAM : total length 0x{0:x} = {1}kb".format(len(sram),len(sram)/1024)
     #   eeprom=get_chunks(eeprombase,16,256,s) # this faults immediately
     #   print "Got EEPROMBase : total length 0x{0:x} = {1}kb".format(len(eeprom),len(eeprom)/1024)
        syscontrol=get_chunks(systemcontrolbase,16,256,s) 
        print "Got SysControlBase : total length 0x{0:x} = {1}kb".format(len(syscontrol),len(syscontrol)/1024)
 
	s.close()

	filename = 'test'	
	if(len(sys.argv) > 1):
		filename = sys.argv[1]
        flash_filename=filename+"FLASH.dat"
        sram_filename=filename+"SRAM.dat"
        sysctrl_filename=filename+"SYSCTRL.dat"

        write_file(flash_filename,flash)
        write_file(sram_filename,sram)
        write_file(sysctrl_filename,syscontrol)
        print "Files saved"


if __name__ == "__main__":
    main()
