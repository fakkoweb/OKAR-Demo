
// Main Application for Ogre integration.
// Responsible for comminication with Ogre (rendering) and OIS (mouse and keyboard input)

#ifndef APP_H
#define APP_H

//class Rift;

#include "ConfigDB.h"
#include "Camera.h"
#include "Rift.h"
#include <sstream>
#include <string.h>
#include "OGRE/Ogre.h"
#include "OIS/OIS.h"
#include <SdkTrays.h>
#include "Scene.h"
#include "Globals.h"

// The Debug window's size is the Oculus Rift Resolution times this factor.
#define debugWindowSize 0.5

// Creates the Ogre application
// Constructor: initializes Ogre (initOgre) and OIS (initOIS), allocates the Scene and viewports (createViewports), inizializes Oculus (initRift) and STARTS RENDERING
// Destructor: deallocates all above
class App : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener
{
	public:
		App();
		~App();

		void loadConfig();
		void initOgre();
		void quitOgre();
		void initOIS();
		void quitOIS();
		void initTray();
		void quitTray();
		//TODO: separate Ogre initialization from windows creation (Oculus NEEDS a window or to create a window to start!)
		//void createWindows();
		void createViewports();

		void quit();

		void initRift();
		void quitRift();

		void initCameras();
		void quitCameras();

		bool keyPressed(const OIS::KeyEvent& e );
		bool keyReleased(const OIS::KeyEvent& e );
		bool mouseMoved(const OIS::MouseEvent& e );
		bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id );
		bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id );

		bool frameRenderingQueued(const Ogre::FrameEvent& evt);

		bool update();

	private:
		ConfigDB* mConfig;

		OIS::Keyboard* mKeyboard;
		OIS::Mouse* mMouse;

		Ogre::Root* mRoot;

		Ogre::RenderWindow* mWindow = nullptr;
		
		// interface display
		Ogre::OverlaySystem* mOverlaySystem;
		OgreBites::SdkTrayManager*	mTrayMgr;
		OgreBites::InputContext     mInputContext;
		OgreBites::ParamsPanel*     mDetailsPanel;   	// Sample details panel
		bool                        mCursorWasVisible;	// Was cursor visible before dialog appeared?
		bool                        mShutDown;

		Ogre::RenderWindow* mSmallWindow = nullptr;
		Ogre::RenderWindow* mGodWindow = nullptr;
		Ogre::Viewport* mViewportL;
		Ogre::Viewport* mViewportR;

		// If this is set to true, the app will close during the next frame:
		bool mShutdown;

		Scene* mScene;

		Rift* mRift;

		FrameCaptureHandler* mCameraLeft;
		FrameCaptureHandler* mCameraRight;
		Ogre::PixelBox mOgrePixelBoxLeft;	//Ogre containers for opencv Mat image raw data
};

#endif
