// ----------------------------------------------------------------------
// Visualization of Patient data (Dicoms and surface models) using the
// Oculus Rift.
// ---------------------------------------------------------------------
// August 2014, Micha Pfeiffer
// May 2015, Dario Facchini
// Couples Ogre3D and the Oculus Rift SDK to render Meshes in a VR setting
//

#include <iostream>
#include "Globals.h"
#include "App.h"
#include "OGRE/Ogre.h"

    int main(int argc, char *argv[])
    {
		std::cout << "Hello world." << std::endl;

		for ( int i = 0; i < argc; i++ )
		{
			std::string arg( argv[i] );

			if (arg == "-cbd" && i<argc-1)
			{
				CAMERA_BUFFERING_DELAY = atoi(argv[++i]);
			}

			// This flag triggers the DK1/DK2 view rotation (set this up as you need it)
			if( arg == "--rotate-view" )
			{
				ROTATE_VIEW = true;
			}
			// This flag enables the program to run even without a physical device connected
			/*	REMOVED: Rift class is now able to detect if there is a compatible headset connected!
			if( arg == "--no-rift" )
			{
				NO_RIFT = true;
			}
			*/
			// This flag disables the debug window (simply a window where stereo virtual cameras are showed before going through Rift class)
			if( arg == "--no-debug" )
			{
				DEBUG_WINDOW = false;
			}
			if( arg == "--help" || arg == "-h" )
			{
				std::cout << "Available Commands:" << std::endl
					<< "\t--rotate-view\tChanges the orientation of the main render window. Useful when your computer can't rotate the screen." << std::endl
					<< "\t--no-rift\tFor debugging: disable the Oculus Rift." << std::endl
					<< "\t--no-debug\tDisables the debug window." << std::endl
					<< "\t--help,-h\tShow this help message." << std::endl;
				exit(0);	// show help and then close app.
			}
		}

		// LOAD APP WITH MY CONFIGURATION VALUES
		// Creates the main program and starts rendering. When a framelistener
		// returns false, this will return.
		std::string configFilePathPrefix = "cfg/";			// configuration files default location when app is installed
		std::string paramsFileName = "parameters.cfg";		// parameters config file name
		App* app;

		// Try to load load up a valid config file (start the program with default values if none is found)
		try
		{
			//This will work ONLY when application is installed (only Release application)!
			app = new App(configFilePathPrefix + paramsFileName);
		}
		catch (Ogre::FileNotFoundException& e)
		{
			try
			{
				// if no existing config, or could not restore it, try to load from a different location
				configFilePathPrefix = "../cfg/";

				// This will work ONLY when application is in development (Debug/Release configuration)
				app = new App(configFilePathPrefix + paramsFileName);
			}
			catch (Ogre::FileNotFoundException& e)
			{
				// critical failure
				throw e;
			}
		}

		// App construction has finished = App has finished its job :)
		delete app;

        return 0;
    }
