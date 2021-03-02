import serial
from time import sleep
import sys


COM = 'COM8'# /dev/ttyACM0 (Linux)
BAUD = 230400

ser = serial.Serial(COM, BAUD,rtscts=True, dsrdtr=True, timeout = .1)

print('Waiting for device');
sleep(3)
print(ser.name)

#check args
if("-m" in sys.argv or "--monitor" in sys.argv):
	monitor = True
else:
	monitor= False

while True:
	val = str(ser.readline().decode().strip('\r\n'))#Capture serial output as a decoded string
	valA = val.split("/")
	#print()
	if(monitor == True):
		print(val)
        