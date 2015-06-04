#include "App.h"
#include <chrono>

void frames_per_second(int delay)
{
	static int num_frame = 0;
	static int time = 0;

	num_frame++;
	time += delay;
	if (time >= 1000)
	{
		std::cout << "\tFPS:" << num_frame << std::endl;
		num_frame = 0;
		time = 0;
	}

}

//Globals used only in App.cpp
std::chrono::steady_clock::time_point ogre_last_frame_displayed_time = std::chrono::system_clock::now();
std::chrono::duration< int, std::milli > ogre_last_frame_delay;


////////////////////////////////////////////////////////////
// Init application
////////////////////////////////////////////////////////////

App::App()
{
	std::cout << "Creating Ogre application:" << std::endl;

	mRoot = NULL;
	mKeyboard = NULL;
	mMouse = NULL;	
	mScene = NULL;
	mShutdown = false;
	mWindow = NULL;
	mSmallWindow = NULL;
	mRift = NULL;

	//Load custom configuration for the App (parameters.cfg)
	loadConfig();

	//Ogre engine setup (loads plugins.cfg/resources.cfg and creates Ogre main rendering window NOT THE SCENE)
	initOgre();

	//Rift Setup (creates Oculus rendering window and Oculus inner scene - user shouldn't care about it)
	initRift();

	//Stereo camera rig setup ()
	initCameras();

	//Input/Output setup (associate I/O to Oculus window)
	initOIS();

	// Create Ogre main scene (setup and populate main scene)
	// This class implements App logic!!
	mScene = new Scene(mRoot, mMouse, mKeyboard);
	mScene->setIPD(mRift->getIPD());
	if (mOverlaySystem)	mScene->getSceneMgr()->addRenderQueueListener(mOverlaySystem);	//Only Ogre main scene will render overlays!
	try
	{
		// try first to load HFOV/VFOV values (higher priority)
		mScene->setupVideo(mConfig->getValueAsReal("Camera/HFOV"), mConfig->getValueAsReal("Camera/VFOV"));
	}
	catch (Ogre::Exception &e)
	{
		// in case one of these parameters is not found in configuration file or is invalid...
		if (e.getNumber() == Ogre::Exception::ERR_ITEM_NOT_FOUND || e.getNumber() == Ogre::Exception::ERR_INVALIDPARAMS)
		{
			try
			{
				// ..try to load other parameters (these are preferred, but lower priority since not everyone know these)
				mScene->setupVideo(mConfig->getValueAsReal("Camera/SensorWidth"), mConfig->getValueAsReal("Camera/SensorHeight"), mConfig->getValueAsReal("Camera/FocalLenght"));
			}
			catch (Ogre::Exception &e)
			{
				// if another exception is thrown (any), forward
				throw e;
			}
		}
		// if another exception is thrown, forward
		else
		{
			throw e;
		}
	}
	// when setup has finished successfully, enable Video into scene
	// mScene->enableVideo();	USER CAN ACTIVATE IT, SEE INPUT KEYS LISTENERS!

	//Start info display interface
	//initTray();

	//Viewport setup (link scene cameras to Ogre/Oculus windows)
	createViewports();

	// Then setup THIS CLASS INSTANCE as a frame listener
	// This means that Ogre will call frameStarted(), frameRenderingQueued() and frameEnded()
	// automatically and periodically if defined in this class
	mRoot->addFrameListener(this);

	//Ogre::WindowEventUtilities::messagePump();

	// START RENDERING!
	mRoot->startRendering();
}

App::~App()
{
	std::cout << "Deleting Ogre application." << std::endl;

	quitCameras();
	quitRift();

	std::cout << "Deleting Scene:" << std::endl;
	if( mScene ) delete mScene;
	//quitTray();
	std::cout << "Closing OIS:" << std::endl;
	quitOIS();
	std::cout << "Closing Ogre:" << std::endl;
	quitOgre();
}

/////////////////////////////////////////////////////////////////
// Load User parameters and App configuration
/////////////////////////////////////////////////////////////////
void App::loadConfig()
{
	std::string configFilePathPrefix = "cfg/";			// configuration files default location when app is installed
	std::string paramsFileName = "parameters.cfg";		// parameters config file name

	// LOAD APP CONFIGURATION VALUES
	// Try to load load up a valid config file (start the program with default values if none is found)
	try
	{
		//This will work ONLY when application is installed (only Release application)!
		mConfig = new ConfigDB(configFilePathPrefix + paramsFileName);
	}
	catch (Ogre::FileNotFoundException& e)
	{
		try
		{
			// if no existing config, or could not restore it, try to load from a different location
			configFilePathPrefix = "../cfg/";

			// This will work ONLY when application is in development (Debug/Release configuration)
			mConfig = new ConfigDB(configFilePathPrefix + paramsFileName);
		}
		catch (Ogre::FileNotFoundException& e)
		{
			// return silently from loadConfig() -- hardcoded default values or main arguments will be used
			return;
		}
	}

	// Overwrite parameters values
	CAMERA_BUFFERING_DELAY = mConfig->getValueAsInt("Camera/BufferingDelay");
	ROTATE_VIEW = mConfig->getValueAsBool("Oculus/RotateView");


}


/////////////////////////////////////////////////////////////////
// Init Ogre (construction, setup and destruction of ogre root):
/////////////////////////////////////////////////////////////////

void App::initOgre()
{

	// Config file class is an utility that parses and stores values from a .cfg file
	Ogre::ConfigFile cf;
	std::string configFilePathPrefix = "cfg/";			// configuration files default location when app is installed
#ifdef _DEBUG
	std::string pluginsFileName = "plugins_d.cfg";		// plugins config file name (Debug mode)
#else
	std::string pluginsFileName = "plugins.cfg";		// plugins config file name (Release mode)
#endif
	std::string resourcesFileName = "resources.cfg";	// resources config file name (Debug/Release mode)



	// LOAD OGRE PLUGINS
	// Try to load load up a valid config file (and don't start the program if none is found)
	try
	{
		//This will work ONLY when application is installed (only Release application)!
		cf.load(configFilePathPrefix + pluginsFileName);
	}
	catch (Ogre::FileNotFoundException &e)
	{
		try
		{
			// if no existing config, or could not restore it, try to load from a different location
			configFilePathPrefix = "../cfg/";

			//This will work ONLY when application is in development (Debug/Release configuration)
			cf.load(configFilePathPrefix + pluginsFileName);			
		}
		catch (Ogre::FileNotFoundException &e)
		{
			// launch exception if no valid config file is found! - PROGRAM WON'T START!
			throw e;
		}
	}


	// INSTANCIATE OGRE ROOT (IT INSTANCIATES ALSO ALL OTHER OGRE COMPONENTS)
	// In Ogre, the singletons are instanciated explicitly (with new) the first time,
	// then it can be accessed with Ogre::Root::getSingleton()
	// Plugins are passed as argument to the "Root" constructor
	mRoot = new Ogre::Root(configFilePathPrefix + pluginsFileName, configFilePathPrefix + "ogre.cfg", "ogre.log");
	// No Ogre::FileNotFoundException is thrown by this, that's why we tried to open it first with ConfigFile::load()


	
	// LOAD OGRE RESOURCES
	// Load up resources according to resources.cfg ("cf" variable is reused)
	try
	{
		//This will work ONLY when application is installed!
		cf.load("cfg/resources.cfg");
	}
	catch (Ogre::FileNotFoundException &e)	// It works, no need to change anything
	{
		try
		{
			//This will work ONLY when application is in development (Debug/Release configuration)
			cf.load("../cfg/resources.cfg");
		}
		catch (Ogre::FileNotFoundException &e)
		{
			// launch exception if no valid config file is found! - PROGRAM WON'T START!
			throw e;
		}
	}

    // Go through all sections & settings in the file and ADD them to Resource Manager
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
			//For each section/key-value, add a resource to ResourceGroupManager
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
	}


	// SELECT AND CUSTOMIZE OGRE RENDERING (OpenGL)
	// Get a reference of the RenderSystem in Ogre that I want to customize
	Ogre::RenderSystem* pRS = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	// Get current config RenderSystem options in a ConfigOptionMap
	Ogre::ConfigOptionMap cfgMap = pRS->getConfigOptions();
	// Modify them
	cfgMap["Full Screen"].currentValue = "No";
	cfgMap["VSync"].currentValue = "Yes";
	#ifdef _DEBUG
		cfgMap["FSAA"].currentValue = "0";
	#else
		cfgMap["FSAA"].currentValue = "8";
	#endif
	cfgMap["Video Mode"].currentValue = "1200 x 800";
	// Set them back into the RenderSystem
	for(Ogre::ConfigOptionMap::iterator iter = cfgMap.begin(); iter != cfgMap.end(); iter++) pRS->setConfigOption(iter->first, iter->second.currentValue);
	// Set this RenderSystem as the one I want to use
	mRoot->setRenderSystem(pRS);
	// Initialize it: "false" is DO NOT CREATE A WINDOW FOR ME
	mRoot->initialise(false, "Oculus Rift Visualization");


	// CREATE WINDOWS
	/* REMOVED: Rift class creates the window if no null is passed to its constructor
	// Options for Window 1 (rendering window)
	Ogre::NameValuePairList miscParams;
	if( NO_RIFT )
		miscParams["monitorIndex"] = Ogre::StringConverter::toString(0);
	else
		miscParams["monitorIndex"] = Ogre::StringConverter::toString(1);
	miscParams["border "] = "none";
	*/

	/*
	// Create Window 1
	if( !ROTATE_VIEW )
	mWindow = mRoot->createRenderWindow("Oculus Rift Liver Visualization", 1280, 800, true, &miscParams);
	//mWindow = mRoot->createRenderWindow("Oculus Rift Liver Visualization", 1920*0.5, 1080*0.5, false, &miscParams);
	else
	mWindow = mRoot->createRenderWindow("Oculus Rift Liver Visualization", 1080, 1920, true, &miscParams);
	*/

#ifdef _DEBUG
	// Options for Window 2 (debug window)
	// This window will simply show what the two cameras see in two different viewports
	Ogre::NameValuePairList miscParamsSmall;
	miscParamsSmall["monitorIndex"] = Ogre::StringConverter::toString(0);

	// Create Window 2
	if( DEBUG_WINDOW )
		mSmallWindow = mRoot->createRenderWindow("DEBUG Oculus Rift Liver Visualization", 1920*debugWindowSize, 1080*debugWindowSize, false, &miscParamsSmall);   

	// Options for Window 3 (god window)
	// This debug window will show the whole scene from a top view perspective
	Ogre::NameValuePairList miscParamsGod;
	miscParamsSmall["monitorIndex"] = Ogre::StringConverter::toString(0);
	
	// Create Window 3
	if (DEBUG_WINDOW)
		mGodWindow = mRoot->createRenderWindow("DEBUG God Visualization", 1920 * debugWindowSize, 1080 * debugWindowSize, false, &miscParamsSmall);
#endif



	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	mOverlaySystem = new Ogre::OverlaySystem();

}

void App::initTray()
{
	// Register as a Window listener
	//Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	if (mWindow)
	{
		mInputContext.mKeyboard = mKeyboard;
		mInputContext.mMouse = mMouse;

		mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mInputContext, this);
		mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
		//mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
		mTrayMgr->hideCursor();

		// Create a params panel for displaying sample details
		Ogre::StringVector items;
		items.push_back("cam.pX");
		items.push_back("cam.pY");
		items.push_back("cam.pZ");
		items.push_back("");
		items.push_back("cam.oW");
		items.push_back("cam.oX");
		items.push_back("cam.oY");
		items.push_back("cam.oZ");
		items.push_back("");
		items.push_back("Filtering");
		items.push_back("Poly Mode");

		mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
		mDetailsPanel->setParamValue(9, "Bilinear");
		mDetailsPanel->setParamValue(10, "Solid");
		mDetailsPanel->hide();
	}

	
}

void App::createViewports()
{
	// Create two viewports, one for each eye (i.e. one for each camera):
	// Each viewport is assigned to a RenderTarget (a RenderWindow in this case) and spans half of the screen
	// A pointer to a Viewport is returned, so we can access it directly.
	// CAMERA -> render into -> VIEWPORT (rectangle area) -> displayed into -> WINDOW
	/*
	if (mWindow)		//check if Ogre rendering window has been created
	{
		if (NO_RIFT)
		{
			mViewportL = mWindow->addViewport(mScene->getLeftCamera(), 0, 0.0, 0.0, 0.5, 1.0);
			mViewportL->setBackgroundColour(Ogre::ColourValue(0.15, 0.15, 0.15));
			mViewportR = mWindow->addViewport(mScene->getRightCamera(), 1, 0.5, 0.0, 0.5, 1.0);
			mViewportR->setBackgroundColour(Ogre::ColourValue(0.15, 0.15, 0.15));
		}

		if( !ROTATE_VIEW )
		{
		mScene->getLeftCamera()->setAspectRatio( 0.5*mWindow->getWidth()/mWindow->getHeight() );
		mScene->getRightCamera()->setAspectRatio( 0.5*mWindow->getWidth()/mWindow->getHeight() );
		} else {
		mScene->getLeftCamera()->setAspectRatio( 0.5*mWindow->getHeight()/mWindow->getWidth() );
		mScene->getRightCamera()->setAspectRatio( 0.5*mWindow->getHeight()/mWindow->getWidth() );
		/
	}
	*/

	// Plug the virtual stereo camera rig to Rift class (they will be rendered on Oculus screen)
	mRift->setCameras(mScene->getLeftCamera(), mScene->getRightCamera());

	// Create similar viewports to be displayed into PC window
	if (mSmallWindow)
	{
		Ogre::Viewport* debugL = mSmallWindow->addViewport(mScene->getLeftCamera(), 0, 0.0, 0.0, 0.5, 1.0);
		debugL->setBackgroundColour(Ogre::ColourValue(0.15, 0.15, 0.15));

		Ogre::Viewport* debugR = mSmallWindow->addViewport(mScene->getRightCamera(), 1, 0.5, 0.0, 0.5, 1.0);
		debugR->setBackgroundColour(Ogre::ColourValue(0.15, 0.15, 0.15));
	}

	// Create a God view window from a third view camera
	if (mGodWindow)
	{
		Ogre::Viewport* god = mGodWindow->addViewport(mScene->getGodCamera(), 0, 0.0, 0.0, 1.0, 1.0);
	}
}

void App::quitTray()
{
	delete mTrayMgr;
	mTrayMgr = nullptr;
	delete mOverlaySystem;
	mOverlaySystem = nullptr;
}

void App::quitOgre()
{
	// This should really not be commented out, but it crashes on my system... ?!
	// if( mRoot ) delete mRoot;
}

////////////////////////////////////////////////////////////
// Init OIS (construction, setup and destruction of input):
////////////////////////////////////////////////////////////

void App::initOIS()
{
	OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
 
    // Tell OIS about the Ogre Rendering window (give its id)
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	// Setup the manager, keyboard and mouse to handle input
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
 
    // Tell OIS about the window's dimensions
    unsigned int width, height, depth;
    int top, left;
    mWindow->getMetrics(width, height, depth, left, top);
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

	// Setup THIS CLASS INSTANCE as a OIS mouse listener AND key listener
	// This means that OIS will call keyPressed(), mouseMoved(), etc.
	// automatically and whenever needed
	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);
}

void App::quitOIS()
{
	if( mKeyboard ) delete mKeyboard;
	if( mMouse ) delete mMouse;
}

////////////////////////////////////////////////////////////////
// Init Rift (Device and API initialization, setup and close):
////////////////////////////////////////////////////////////////

void App::initRift()
{
	// Try to initialize the Oculus Rift (ID 0):
	try {
		// This class implements a custom C++ Class version of RIFT C API
		//Rift::init();		//OPTIONAL: automatically called by Rift constructor, if necessary
		mRift = new Rift( 0, mRoot, mWindow /*if null, Rift creates the window*/, ROTATE_VIEW );
	}
	catch (const std::ios_base::failure& e) {
		std::cout << ">> " << e.what() << std::endl;
		//NO_RIFT = true;
		mRift = NULL;
		mShutdown = true;
	}
}

void App::quitRift()
{
	std::cout << "Shutting down Oculus Rifts:" << std::endl;
	if( mRift ) delete mRift;
	//Rift::shutdown();
}

////////////////////////////////////////////////////////////////
// Init Cameras (Device and API initialization, setup and close):
////////////////////////////////////////////////////////////////

void App::initCameras()
{
	mCameraLeft = new FrameCaptureHandler(0, mRift);
	//mCameraRight = new FrameCaptureHandler(1, mRift);

	mCameraLeft->startCapture();
	//mCameraRight->startCapture();

	cv::namedWindow("CameraDebug", cv::WINDOW_NORMAL);
	cv::resizeWindow("CameraDebug", 1920 / 4, 1080 / 4);
}

void App::quitCameras()
{
	mCameraLeft->stopCapture();
	//mCameraRight->stopCapture();
	if (mCameraLeft) delete mCameraLeft;
	//if (mCameraRight) delete mCameraRight;
}

////////////////////////////////////////////////////////////
// Handle Rendering (Ogre::FrameListener)
////////////////////////////////////////////////////////////

// This gets called while rendering frame data is loading into GPU
// Good time to update measurements and physics before rendering next frame!
bool App::frameRenderingQueued(const Ogre::FrameEvent& evt) 
{
	// [TIME] FRAME RATE DISPLAY
	//calculate delay from last frame and show
	ogre_last_frame_delay = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - ogre_last_frame_displayed_time);
	frames_per_second(ogre_last_frame_delay.count());

	if (mShutdown) return false;

	// [RIFT] UPDATE
	// update Oculus information and sends it to Scene (Position/Orientation of character's head)
	if(mRift)
	{
		if ( mRift->update( evt.timeSinceLastFrame ) )		// saves new orientation/position information
		{
			mScene->setRiftPose( mRift->getOrientation(), mRift->getPosition() );	// sets orientation/position to a SceneNode
		} else {
			delete mRift;
			mRift = NULL;
		}
	}

	// [CAMERA] UPDATE
	// update cameras information and sends it to Scene (Texture of pictures planes/shapes)
	FrameCaptureData uno;
	if (mCameraLeft && mCameraLeft->get(uno))	// if camera is initialized AND there is a new frame
	{
		std::cout << "Drawing the frame in debug window..." << std::endl;

		//cv::imshow("sideleft", left);
		//cv::waitKey(1);
		cv::imshow("CameraDebug", uno.image);
		cv::waitKey(1);
		
		
		std::cout << "converting from cv::Mat to Ogre::PixelBox..." << std::endl;
		mOgrePixelBoxLeft = Ogre::PixelBox(1920, 1080, 1, Ogre::PF_R8G8B8, uno.image.ptr<uchar>(0));
		std::cout << "sending new image to the scene..." << std::endl;
		mScene->setVideoImagePoseLeft(mOgrePixelBoxLeft,uno.pose);
		std::cout << "image sent!\nImage plane updated!" << std::endl;

	}

	// [OIS] UPDATE
	// update standard input devices state
	mKeyboard->capture();
	mMouse->capture();

	// [OGRE] UPDATE
	// perform any other update that regards the Scene only (ex. Physics calculations)
	mScene->update( evt.timeSinceLastFrame );
	//mTrayMgr->frameRenderingQueued(evt);

	// [TIME] UPDATE
	// save time point for this frame (for frame rate calculation)
	ogre_last_frame_displayed_time = std::chrono::system_clock::now();

	//exit if key ESCAPE pressed 
	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) 
		return false;

	return true; 
}

//////////////////////////////////////////////////////////////////////////////////
// Handle Keyboard and Mouse input (OIS::KeyListener, public OIS::MouseListener)
//////////////////////////////////////////////////////////////////////////////////

bool App::keyPressed( const OIS::KeyEvent& e )
{
	mScene->keyPressed( e );

	if( e.key == OIS::KC_P )
		mWindow->writeContentsToFile("Screenshot.png");

	return true;
}
bool App::keyReleased( const OIS::KeyEvent& e )
{
	mScene->keyReleased( e );

	static float distance = 1;
	if (e.key == OIS::KC_ADD)
	{
		//CAMERA_BUFFERING_DELAY += 5;
		distance += 0.1f;
		mScene->setVideoDistance(distance);
	}
	if (e.key == OIS::KC_SUBTRACT)
	{
		//CAMERA_BUFFERING_DELAY -= 5;
		distance -= 0.1f;
		mScene->setVideoDistance(distance);
	}

	static bool seethroughEnabled = false;
	if (e.key == OIS::KC_C)
	{
		// toggle VIDEO in the scene with "c"
		if (seethroughEnabled)
		{
			mScene->disableVideo();
			seethroughEnabled = false;
		}
		else
		{
			mScene->enableVideo();
			seethroughEnabled = true;
		}
	}
		
	return true;
}
bool App::mouseMoved( const OIS::MouseEvent& e )
{
	mScene->mouseMoved( e );
	return true;
}
bool App::mousePressed( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	mScene->mouseReleased( e, id );
	return true;
}
bool App::mouseReleased( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	mScene->mouseReleased( e, id );
	return true;
}

// Interrupts App loop
// frameRenderingQueued() will return false, thus interrupting Ogre loop
// App could run again by calling "mRoot->startRendering()"
// Destructor still needs to be called by user to deallocate application!
void App::quit()
{
	std::cout << "QUIT." << std::endl;
	mShutdown = true;
}
