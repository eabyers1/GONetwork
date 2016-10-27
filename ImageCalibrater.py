#!/usr/bin/python

import os, sys, csv
from PIL import Image

path = '/Users/enochbyers/Desktop/GONetwork/CorrectedImages'
s = "CorrectedImage"
end = ".png"

star = raw_input("What is the name of the star? ")
M = input("What is the stars magnitude? ")
pic = raw_input("Which image number would you like to analyze? ")
file = s.join(pic)
im = Image.open(os.path.join(path, s + file + end))

# Here we calculae the Flux of the star of interest
F = 3640.0*(10.0**(-1.0*float(M)/2.5))

# We will gather the pixel range we want to analyze for the star and then for some background pixels near it to compare.
xpixrange = range(input("Enter X location of starting pixel "),1+input("Enter X value of the pixel after the ending pixel "))
ypixrange = range(input("Enter Y location of starting pixel "),1+input("Enter Y value of the pixel after the ending pixel "))

dxpixrange = range(input("Enter X location of starting dark pixel "),1+input("Enter X value of the pixel after the ending dark pixel "))
dypixrange = range(input("Enter X location of starting dark pixel "),1+input("Enter X value of the pixel after the ending dark pixel "))

numpixd = len(dxpixrange) * len(dypixrange)
numpix = len(xpixrange) * len(ypixrange)

dy = 0
y = 0

# We now need to find the relative luminance of the dark pixels and the pixels containing the star data. 
# Subtracting these two values lets us know how bright the star appears in the sky.   
# This information can be used later to determine how bright the light pollution ring visible in the images is.
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
# All the information accumulated to analyze the star is saved in a csv file to compare at a later date.
StarData = open('StarData.csv', 'a')

spamwriter = csv.writer(StarData)
spamwriter.writerow([star,F,Dn,M,pic,xpixrange,ypixrange,dxpixrange,dypixrange])
