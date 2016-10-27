#!/usr/bin/python

import os, sys, csv
from PIL import Image

path = '/Users/enochbyers/Desktop/GONetwork/CorrectedImages'
s = "CorrectedImage"
end = ".png"

star = raw_input("What is the name of the stars? ")
M = input("What is the stars magnitude? ")
pic = raw_input("Which image number would you like to analyze? ")
file = s.join(pic)
im = Image.open(os.path.join(path, s + file + end))

F = 3640.0*(10.0**(-1.0*float(M)/2.5))

xpixrange = range(input("Enter X location of starting pixel "),1+input("Enter X value of the pixel after the ending pixel "))
ypixrange = range(input("Enter Y location of starting pixel "),1+input("Enter Y value of the pixel after the ending pixel "))

dxpixrange = range(input("Enter X location of starting dark pixel "),1+input("Enter X value of the pixel after the ending dark pixel "))
dypixrange = range(input("Enter X location of starting dark pixel "),1+input("Enter X value of the pixel after the ending dark pixel "))

numpixd = len(dxpixrange) * len(dypixrange)
numpix = len(xpixrange) * len(ypixrange)

dy = 0
y = 0

for x in dxpixrange:
	for y in dypixrange:
		dpix = im.load()
		dcpixel = dpix[x,y]
		dy = dy + ((.2126*dcpixel[0])+(.7152*dcpixel[1])+(.0722*dcpixel[2]))
		

background = dy/numpixd

for x in xpixrange:
	for y in ypixrange:
		pix = im.load()
		cpixel = pix[x,y]
		y = y + ((.2126*cpixel[0])+(.7152*cpixel[1])+(.0722*cpixel[2]))
		

Dn = y - (background * numpix) 
StarData = open('StarData.csv', 'a')

spamwriter = csv.writer(StarData)
spamwriter.writerow([star,F,Dn,M,pic,xpixrange,ypixrange,dxpixrange,dypixrange])
