#! /usr/bin/python

# The code below is used to reduce the noise visible in the images taken by the GONetwork.
# Dark frames are subtracted from each image in the specified file location once this program is run
# Be sure that your file location and working directory are correct for YOUR COMPUTER

import os, sys
from PIL import Image, ImageChops

def Cimg_images():
	img1 = Image.open('/Users/enochbyers/Desktop/GONetwork/DarkFrames/darkframe14.png')
	i = 0
	for filename in os.listdir('/Users/enochbyers/Desktop/GONetwork/RawImages'):
		i += 1
		if filename.endswith(".png"):
			img2 = Image.open('/Users/enochbyers/Desktop/GONetwork/RawImages/%s' %filename)
			Cimg = ImageChops.subtract(img2, img1)
			Cimg.save("CorrectedImage%s.png" %i)
	
Cimg_images()

