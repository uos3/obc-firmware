# linksprite_grab.py
# grabs photo from linksprite LS-Y201 camera
#
# Jon Klein, 8/26/11
# kleinjt@ieee.org
# MIT License
#
# usage: python linksprite_grab picture.jpg
# modified S Lucarotti to test with UART_passthrough

import serial, time, sys

def a2s(arr):
	return ''.join(chr(b) for b in arr)

# OLD: LK_POWERUP = a2s([0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a])
LK_POWERUP = a2s([0x0d, 0x0a, 0x00, 0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a])

LK_BAUDRATE_19200	= a2s([0x56, 0x00, 0x24, 0x03, 0x01, 0x56, 0xe4])
LK_BAUDRATE_38400	= a2s([0x56, 0x00, 0x24, 0x03, 0x01, 0x2a, 0xf2])
LK_BAUDRATE_RE	= a2s([0x76, 0x00, 0x24, 0x00, 0x00])
 
LK_RESOLUTION_VGA = a2s([0x56, 0x00, 0x54, 0x01, 0x00])
LK_RESOLUTION_800 = a2s([0x56, 0x00, 0x54, 0x01, 0x1D])
LK_RESOLUTION_1280 = a2s([0x56, 0x00, 0x54, 0x01, 0x1B])
LK_RESOLUTION_1600 = a2s([0x56, 0x00, 0x54, 0x01, 0x21])
LK_RESOLUTION_RE = a2s([0x76, 0x00, 0x54, 0x00, 0x00])

LK_COMPRESSION = [0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04]
LK_COMPRESSION_RE = a2s([0x76, 0x00, 0x31, 0x00, 0x00])

LK_RESET 		= a2s([0x56, 0x00, 0x26, 0x00])
# OLD: LK_RESET_RE 		= a2s([0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a])
# LEGIT: LK_RESET_RE 		= a2s([0x76,0x00,0x31,0x00,0x00])
LK_RESET_RE 		= a2s([0x76,0x00,0x54,0x00,0x00])
LK_RESET_RE2 		= a2s([0x76,0x00,0x31,0x00,0x00])

LK_PICTURE 		= a2s([0x56, 0x00, 0x36, 0x01, 0x00])
LK_PICTURE_RE		= a2s([0x76, 0x00, 0x36, 0x00, 0x00])
LK_JPEGSIZE 		= a2s([0x56, 0x00, 0x34, 0x01, 0x00])
LK_JPEGSIZE_RE		= a2s([0x76, 0x00, 0x34, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00]) # then XH XL
LK_STOP			= a2s([0x56, 0x00, 0x36, 0x01, 0x03])
LK_STOP_RE		= a2s([0x76, 0x00, 0x36, 0x00, 0x00])

LK_READPICTURE		= [0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00];
LK_PICTURE_TIME		= [0x00, 0x0A] # .1 ms
LK_READPICTURE_RE 	= a2s([0x76, 0x00, 0x32, 0x00, 0x00])
JPEG_START 		= a2s([0xFF, 0xD8])
JPEG_END 		= a2s([0xFF, 0xD9])

def init_serial(baudrate):
	return serial.Serial('/dev/ttyUSB0', baudrate, timeout=2)

def main():
	s = init_serial('115200')

#	while(1):
#		re = s.read(1)
#		print ':'.join(x.encode('hex') for x in re)

	print "Waiting for powerup"
	newdata = s.read(len(LK_POWERUP))
	while(newdata != LK_POWERUP): 
                debug="."
                print len(newdata)
                print tuple(newdata)
                for c in tuple(newdata):
                    debug+="[{}]".format(ord(c),'02x')
                print debug
		newdata = s.read(len(LK_POWERUP))

	print "Received Powerup!"

	if(not link_reset(s)):
		#print "Attempting 38400 baud.."
		#s.close()
		#s = init_serial('38400')
		while(not link_reset(s)):
			time.sleep(1)
	time.sleep(2)

	#while(not change_baudrate(s, LK_BAUDRATE_38400)):
	#	time.sleep(.1)

	#s.close()
	#s = init_serial('38400')

	s.flushInput()
	s.write(LK_RESET)

	while(not set_resolution(s, LK_RESOLUTION_1600)):
		time.sleep(.1)
	time.sleep(.2)

	while(not set_compression(s, 0x10)): #0x36)):
		time.sleep(.1)
	time.sleep(.2)

        print "taking picture..."
	take_picture(s)
        print "checking it..."
	time.sleep(.5)

	size = check_picturesize(s)
	hsize = size[0]*256 + size[1]
	if hsize==0:
		print "Picture size is Zero"
		return
	
	picture = grab_picture(s, size)

	s.close()

	filename = 'test.jpg'	
	if(len(sys.argv) > 1):
		filename = sys.argv[1]

	file = open(filename,'wb')
	file.write(picture)
	file.close()

def change_baudrate(s, cmd):
	s.flushInput()
	s.write(cmd)
	re = s.read(len(LK_BAUDRATE_RE))
	if(re == LK_BAUDRATE_RE):
		print "Baudrate response OK"
		print ':'.join(x.encode('hex') for x in re)
		return True
	else:
		print "Baudrate response failed (returned length: {})".format(len(re))
		print ':'.join(x.encode('hex') for x in re)
		return False
	
def link_reset(s):
        print "trying link reset"
	s.flushInput()
	s.write(LK_RESET)
	re = s.read(len(LK_RESET_RE))
	if(re == LK_RESET_RE or re == LK_RESET_RE2):
		print "Reset response OK"
		print ':'.join(x.encode('hex') for x in re)
		return True
	else:
		print "Reset response failed (returned length: {})".format(len(re))
		print ':'.join(x.encode('hex') for x in re)
		return False

def set_resolution(s, cmd):
	s.flushInput()
	s.write(cmd)
	re = s.read(len(cmd))
	if(re == LK_RESOLUTION_RE):
		print "Set resolution response OK"
		print ':'.join(x.encode('hex') for x in re)
		return True
	else:
		print "Set resolution failed (returned length: {})".format(len(re))
		print ':'.join(x.encode('hex') for x in re)
		return False

def set_compression(s, factor):
	s.flushInput()
	s.write(a2s(LK_COMPRESSION + [factor]))
	re = s.read(len(LK_COMPRESSION_RE))
	if(re == LK_COMPRESSION_RE):
		print "Set compression response OK"
		print ':'.join(x.encode('hex') for x in re)
		return True
	else:
		print "Set compression failed (returned length: {})".format(len(re))
		print ':'.join(x.encode('hex') for x in re)
		return False

def take_picture(s):
	s.flushInput();
	s.write(LK_PICTURE)
	while(1):
		re = s.read(len(LK_PICTURE_RE))
		if(re == LK_PICTURE_RE):
			print "Capture response OK"
			print ':'.join(x.encode('hex') for x in re)
			return True
		elif(len(re)>0):
			print "Capture response failed (returned length: {})".format(len(re))
			print ':'.join(x.encode('hex') for x in re)
		else:
			print "Waiting for capture response.."

def check_picturesize(s):
	s.flushInput()
	s.write(LK_JPEGSIZE)
	re = s.read(len(LK_JPEGSIZE_RE))
	if(len(re) == len(LK_JPEGSIZE_RE)):
		print "Picture size response OK"
		print ':'.join(x.encode('hex') for x in re)
	else:
		print "Picture size response failed (returned length: {})".format(len(re))
		print ':'.join(x.encode('hex') for x in re)
	return [ord(re[-2]), ord(re[-1])]

def grab_picture(s, size):
	s.flushInput()
	offset = [0x00, 0x00]
	size = [0xff, 0xff]
	s.write(a2s(LK_READPICTURE + offset + [0x00,0x00] + size + LK_PICTURE_TIME))
	re = s.read(len(LK_READPICTURE_RE))
	if(re != LK_READPICTURE_RE):
		print "read picture response failed (returned length: {})".format(len(re))
		print ':'.join(x.encode('hex') for x in re)
	
	picture = s.read(2)
	if(picture != JPEG_START):
		print 'picture start incorrect'
		print ':'.join(x.encode('hex') for x in picture)
 	
	print "Reading JPEG data.."
	while(picture[-2:] != JPEG_END):
		newdata = s.read(1)
		if len(newdata)==0:
			print "Image timed out at {} bytes".format(len(picture))
			print "Last bytes:"
			print ':'.join(x.encode('hex') for x in picture[-10:])
			return picture
		
		picture = picture + newdata

		if(len(picture) == 65541):
			print "Continuing from offset.."
			if picture[-5:] == LK_READPICTURE_RE:
				picture = picture[:-6]
			offset = size
			size = [0xff, 0xff]
			s.write(a2s(LK_READPICTURE + offset + [0x00,0x00] + size + LK_PICTURE_TIME))
			re = s.read(len(LK_READPICTURE_RE))
			if(re != LK_READPICTURE_RE):
				print "read picture response failed (returned length: {})".format(len(re))
				print ':'.join(x.encode('hex') for x in re)

			while(picture[-2:] != JPEG_END):
				newdata = s.read(1)
				if len(newdata)==0:
					print "Image totally timed out at {} bytes".format(len(picture))
					print "Last bytes:"
					print ':'.join(x.encode('hex') for x in picture[-10:])
					return picture
				picture = picture + newdata
			break
	
	print "Detected JPEG Footer"
	print "Received image of {} bytes".format(len(picture))
	return picture

if __name__ == "__main__":
    main()
