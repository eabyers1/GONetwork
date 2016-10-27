import serial
import datetime
import time
import csv

# This code runs on a raspberry pi and communicates with an Arduino to control the light source for the GONetwork.
ser = serial.Serial('/dev/ttyUSB0',9600, timeout=1)
lum=[0,1]
while True:
	t = time.gmtime();
	secs = t[5];
	min = t[4];
	hr = t[3];
	today = datetime.datetime.now()
# Depending on the time, the Pi sends a signal to the Arduino to either rotate the servo clockwise or counterclockwise
	if secs == 28 or secs == 58:
		read_serial=ser.write('1')
		print(t);
		print(secs);
		time.sleep(1)

	if secs == 44 or secs == 14:
		read_serial=ser.readline();
		lum[0] = str(ser.readline()) 
		with open('/home/pi/luminositylog.csv','a') as csvfile:
			csvwriter = csv.writer(csvfile,delimiter=',',quotechar='|', quoting=csv.QUOTE_ALL)
			csvwriter.writerow([lum,today]);
		
		continue		

