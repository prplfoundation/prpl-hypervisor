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


# Fake keyCode
binary_keyCode = base64.b16decode('0203030203030010200102000100000000000000000000')

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

