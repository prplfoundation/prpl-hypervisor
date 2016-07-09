import serial
from time import sleep
import base64
import sys

def readSerial():
	while True:
		response = ser.readline();
		return response


# main
ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=3)
ser.isOpen()

# Wait UART Listener VM to be done. 
while(1):
        message = readSerial()
        if 'Listener' in message:
                break

#Requires the keycode to the Litener VM
ser.write('\n'.encode())
ser.flush()


#Receive the keyCode
while(1):
    message = readSerial()
    if 'keyCode' in message:
        hex_keyCode = message[9:-1]
        break

print "KeyCode: ", hex_keyCode

binary_keyCode = base64.b16decode(hex_keyCode.upper())

while(1):
    print "ARM Commands: "
    print "1 - Start"
    print "2 - Stop"

    c = '0'
    while c!='1' and c!='2':
        c = raw_input('Input:')

    print 'Sending the arm command...'

    for i in range(0, len(binary_keyCode)):
        ser.write(binary_keyCode[i])
        ser.flush()

    ser.write(c.encode())
    ser.write('\n'.encode())
    ser.flush()
    
    print 'Board response: %s' % readSerial()

