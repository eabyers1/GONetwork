#! /usr/bin/python
import os, sys
from PIL import Image, ImageChops

def Cimg_images():
	img1 = Image.open('/Users/enochbyers/Desktop/GONetwork/DarkFrames/darkframe14.png')
	i = 0
	for filename in os.listdir('/Users/enochbyers/Desktop/GONetwork/RawImages'):
		i += 1
		if filename.endswith(".png"):
			img2 = Image.open('/Users/enochbyers/Desktop/GONetwork/RawImages/%s' %filename)
# The code commented out was an attempt at combining multiple dark frames to see if I could get rid of all the bad pixels.
# The result was not great but I left it in anyway.
#			Cimg2 = Image.open('GONetwork-11D9M @1-4-46.png')
#	for i in range(1,4):
#	frame1 = Image.open('darkframe%i.png' % i)
#	Cimg2 = ImageChops.subtract(Cimg2, frame1)
#	Cimg2.save("CorrectedImage.png")

			Cimg = ImageChops.subtract(img2, img1)
			Cimg.save("CorrectedImage%s.png" %i)
	
Cimg_images()

