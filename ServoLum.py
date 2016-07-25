import serial
import datetime
import time
import csv
ser = serial.Serial('/dev/ttyUSB0',9600, timeout=1)
lum=[0,1]
while True:
	t = time.gmtime();
	secs = t[5];
	min = t[4];
	hr = t[3];
	today = datetime.datetime.now()
	if secs == 28 or secs == 58:
	# 2 turns clockwise
		read_serial=ser.write('1')
		print(t);
		print(secs);
		time.sleep(1)
		#read_serial=ser.readline();
                #lum[0] = str(ser.readline())
                #with open('/home/pi/luminositylog.csv','a') as csvfile:
                #        csvwriter = csv.writer(csvfile,delimiter=',',quotechar=$
                #        csvwriter.writerow([lum,today]);

                #continue
	if secs == 44 or secs == 14:
		read_serial=ser.readline();
		lum[0] = str(ser.readline()) 
		with open('/home/pi/luminositylog.csv','a') as csvfile:
			csvwriter = csv.writer(csvfile,delimiter=',',quotechar='|', quoting=csv.QUOTE_ALL)
			csvwriter.writerow([lum,today]);
		
		continue		
#	if secs == 13 or secs == 43:
	# 1 turns counter clockwise	
#		read_serial=ser.write('1')
#		print(t);
#		print(secs);
#		time.sleep(1);
