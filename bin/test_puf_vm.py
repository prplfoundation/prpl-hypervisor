import serial
from time import sleep
import base64

def readSerial():
	while True:
		response = ser.readline();
		return response


# main
ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=3)
ser.isOpen()

while(1):
	message = readSerial()
	
	if 'keyCode' in message:
		hex_keyCode = message[9:-1]
		
#		print hex_keyCode
		break

binary_keyCode = base64.b16decode(hex_keyCode.upper())

binary_keyCode_manipulated = binary_keyCode
temp = list(binary_keyCode_manipulated)
temp[0] = '01'
temp[1] = '02'
temp[2] = '03'
binary_keyCode_manipulated = "".join(temp)


print 'Sending the wrong keyCode to the board (%d bytes)' % len(binary_keyCode_manipulated)

for i in range(0, len(binary_keyCode_manipulated)):
	ser.write(binary_keyCode_manipulated[i])
	ser.flush()

ser.write('\n'.encode())
ser.flush()

print 'Board response: %s' % readSerial()

sleep(2)

print 'Sending the correct keyCode to the board (%d bytes)' % len(binary_keyCode)

for i in range(0, len(binary_keyCode)):
	ser.write(binary_keyCode[i])
	ser.flush()

ser.write('\n'.encode())
ser.flush()

print 'Board response: %s' % readSerial()

