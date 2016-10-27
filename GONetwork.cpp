//=============================================================================
// Copyright © 2008 Point Grey Research, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with PGR.
//
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: FlyCapture2Test.cpp,v 1.19 2010-03-11 22:58:37 soowei Exp $
//=============================================================================

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "FlyCapture2.h"
#include <time.h>
#include <unistd.h>
#include <bitset>
#include <iomanip>
#include <fstream>



using namespace FlyCapture2;
using namespace std;

enum ExtendedShutterType
{
	NO_EXTENDED_SHUTTER,
	DRAGONFLY_EXTENDED_SHUTTER,
	GENERAL_EXTENDED_SHUTTER
};

void PrintBuildInfo()
{
	FC2Version fc2Version;
	Utilities::GetLibraryVersion( &fc2Version );

	ostringstream version;
	version << "FlyCapture2 library version: " << fc2Version.major << "." << fc2Version.minor << "." << fc2Version.type << "." << fc2Version.build;
	cout << version.str() << endl;

	ostringstream timeStamp;
	timeStamp <<"Application build date: " << __DATE__ << " " << __TIME__;
	cout << timeStamp.str() << endl << endl;
}

void PrintCameraInfo( CameraInfo* pCamInfo )
{
	cout << endl;
	/* Below code has been removed because camera info need not be printed to the console
	   During operation.
	cout << "*** CAMERA INFORMATION ***" << endl;
	cout << "Serial number -" << pCamInfo->serialNumber << endl;
	cout << "Camera model - " << pCamInfo->modelName << endl;
	cout << "Camera vendor - " << pCamInfo->vendorName << endl;
	cout << "Sensor - " << pCamInfo->sensorInfo << endl;
	cout << "Resolution - " << pCamInfo->sensorResolution << endl;
	cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
	cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl << endl;
	*/
}

void PrintError( Error error )
{
	error.PrintErrorTrace();
}

int RunSingleCamera( PGRGuid guid , int k_numImages , float k_shutterVal)
{

	Error error;
	Camera cam;

	// Connect to a camera
	error = cam.Connect(&guid);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}

	// Get the camera information
	CameraInfo camInfo;
	error = cam.GetCameraInfo(&camInfo);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}

	// Check if the camera supports the FRAME_RATE property
	PropertyInfo propInfo;
	propInfo.type = FRAME_RATE;
	error = cam.GetPropertyInfo( &propInfo );
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}

	ExtendedShutterType shutterType = NO_EXTENDED_SHUTTER;

	if ( propInfo.present == true )
	{
		// Turn off frame rate

		Property prop;
		prop.type = FRAME_RATE;
		error = cam.GetProperty( &prop );
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return -1;
		}

		prop.autoManualMode = false;
		prop.onOff = false;

		error = cam.SetProperty( &prop );
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return -1;
		}

		shutterType = GENERAL_EXTENDED_SHUTTER;
	}
	else
	{
		// Frame rate property does not appear to be supported.
		// Disable the extended shutter register instead.
		// This is only applicable for Dragonfly.

		const unsigned int k_extendedShutter = 0x1028;
		unsigned int extendedShutterRegVal = 0;

		error = cam.ReadRegister( k_extendedShutter, &extendedShutterRegVal );
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return -1;
		}

		std::bitset<32> extendedShutterBS((int) extendedShutterRegVal );
		if ( extendedShutterBS[31] == true )
		{
			// Set the camera into extended shutter mode
			error = cam.WriteRegister( k_extendedShutter, 0x80020000 );
			if (error != PGRERROR_OK)
			{
				PrintError( error );
				return -1;
			}
		}
		else
		{
			cout << "Frame rate and extended shutter are not supported... exiting" << endl;
			return -1;
		}

		shutterType = DRAGONFLY_EXTENDED_SHUTTER;
	}

	// Set the shutter property of the camera
	Property prop;
	prop.type = SHUTTER;
	error = cam.GetProperty( &prop );
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}

	prop.autoManualMode = false;
	prop.absControl = true;


	prop.absValue = k_shutterVal;

	error = cam.SetProperty( &prop );
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}

	cout << "Shutter time set to " << fixed << setprecision(2) << k_shutterVal << "ms" << endl;



	// Enable timestamping
	EmbeddedImageInfo embeddedInfo;

	error = cam.GetEmbeddedImageInfo( &embeddedInfo );
	if ( error != PGRERROR_OK )
	{
		PrintError( error );
		return -1;
	}

	if ( embeddedInfo.timestamp.available != 0 )
	{
		embeddedInfo.timestamp.onOff = true;
	}

	error = cam.SetEmbeddedImageInfo( &embeddedInfo );
	if ( error != PGRERROR_OK )
	{
		PrintError( error );
		return -1;
	}

	// Start capturing images
	error = cam.StartCapture();
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}
	

	for ( int imageCnt=0; imageCnt < k_numImages; imageCnt++ ) 
	{
		Image rawImage;
		// Retrieve an image
		error = cam.RetrieveBuffer( &rawImage );
		
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			continue;
		}
		
		cout << "Grabbed image " << imageCnt << endl;

		// Create a converted image
		Image convertedImage;

		// Convert the raw image
		error = rawImage.Convert( PIXEL_FORMAT_RGB16, &convertedImage );
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return -1;
		}
		
		// Create a unique filename
		time_t rawtime;
		struct tm * ptm;
		time ( &rawtime );
		ptm = gmtime ( &rawtime );
		ostringstream filename;
		filename << "GONetwork-" << (ptm->tm_mday) << "D" << (ptm->tm_mon+1) << "M @"<< (ptm->tm_hour)%24 << "-" << (ptm->tm_min) << "-" << (ptm->tm_sec) << ".bmp";

		// Save the image. If a file format is not passed in, then the file
		// extension is parsed to attempt to determine the file format.
		error = convertedImage.Save( filename.str().c_str() );
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return -1;
		}
	}

	// Stop capturing images
	error = cam.StopCapture();
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}

	// Disconnect the camera
	error = cam.Disconnect();
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}

	return 0;
}

int main(int /*argc*/, char** /*argv*/)
{
	PrintBuildInfo();
	std::ofstream outfile;
	outfile.open("CommandLog.txt", std::ios_base::app);
	Error error;
 	repeat:

	time_t rawtime;
	struct tm * ptm;
	time ( &rawtime );
	ptm = gmtime ( &rawtime );
	
	if (int(ptm->tm_sec)==0 || ptm->tm_sec==30) {
	outfile << "Current time: " << (ptm->tm_hour)%24 << ":" << (ptm->tm_min) << ":" << (ptm->tm_sec) << endl;
		// Since this application saves images in the current folder
		// we must ensure that we have permission to write to this folder.
		// If we do not have permission, fail right away.
		FILE* tempFile = fopen("test.txt", "w+");
		if (tempFile == NULL)
		{
			outfile << "Failed to create file in current folder.  Please check permissions." << endl;
			return -1;
		}
		fclose(tempFile);
		remove("test.txt");

		BusManager busMgr;
		unsigned int numCameras;
		error = busMgr.GetNumOfCameras(&numCameras);
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return -1;
		}

		for (unsigned int i=0; i < numCameras; i++)
		{
			float w = 3500.0; // This line is where the shutter value has been set (3.5 seconds in this case)
			float L;          // The camera's first image at boot will use the previous shutter value from last use.  This variable is used to remind us that the first image cannot be trusted
			if (L < 1.0)
			{
				outfile << "Ignore This Image!!" << w << endl;
				L = L+1.0;
				PGRGuid guid;
				error = busMgr.GetCameraFromIndex(i, &guid);
				if (error != PGRERROR_OK)
				{
					PrintError( error );
					return -1;
				}

				RunSingleCamera( guid , 4 , w);
			}
			else
			{
				L = L+1.0;
				outfile << "Shutter time set to: " << w << endl;
				PGRGuid guid;
				error = busMgr.GetCameraFromIndex(i, &guid);
				if (error != PGRERROR_OK)
				{
					PrintError( error );
					return -1;
				}

				RunSingleCamera( guid , 4 , w);
			}
	
		outfile << "Done!" << endl;

		}	
	}
	goto repeat;
	return 0;
}
