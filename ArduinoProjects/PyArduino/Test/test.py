import serial
import time

arduinoSerial = serial.Serial('/dev/ttyACM0', 9600)

while True:
	data = arduinoSerial.readline();
	data = str(data, 'utf-8').strip('\r\n').split(' ');
	print(data);
