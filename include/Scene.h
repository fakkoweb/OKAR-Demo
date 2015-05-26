#ifndef SCENE_H
#define SCENE_H

#include "OGRE/Ogre.h"
#include "OIS/OIS.h"
#include <opencv2\opencv.hpp>
#include "Globals.h"

class Scene : public Ogre::Camera::Listener
{
	public:
		Scene( Ogre::Root* root, OIS::Mouse* mouse, OIS::Keyboard* keyboard );
		~Scene();

		Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }

		// Initialising/Loading the scene
		void createRoom();
		void createCameras();
		void createVideos();

		void update( float dt );

		Ogre::Camera* getLeftCamera() { return mCamLeft; }
		Ogre::Camera* getRightCamera() { return mCamRight; }
		Ogre::Camera* getGodCamera() { return mCamGod; }
		void setIPD( float IPD );

		void setRiftPose( Ogre::Quaternion orientation, Ogre::Vector3 pos );
		void setCameraTextureLeft(const Ogre::PixelBox &image, Ogre::Quaternion pose);
		//void setCameraTextureRight();

		// Keyboard and mouse events (forwarded by App)
		bool keyPressed(const OIS::KeyEvent&);
		bool keyReleased(const OIS::KeyEvent&);
		bool mouseMoved(const OIS::MouseEvent&);
		bool mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID);
		bool mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID);

		// Virtual Camera events (deriving from Ogre::Camera::Listener)
		void cameraPostRenderScene(Ogre::Camera* cam);
		void cameraPreRenderScene(Ogre::Camera* cam);

	private:
		Ogre::Root* mRoot;
		OIS::Mouse* mMouse;
		OIS::Keyboard* mKeyboard;
		Ogre::SceneManager* mSceneMgr;

		Ogre::Camera* mCamLeft;
		
		Ogre::Image mLeftCameraRenderImage;

		Ogre::TexturePtr mLeftCameraRenderTexture;
		Ogre::MaterialPtr mLeftCameraRenderMaterial;
		Ogre::Camera* mCamRight;
		Ogre::Camera* mCamGod;

		Ogre::SceneNode* mVideoLeft;
		Ogre::SceneNode* mHeadNode;
		Ogre::SceneNode* mBodyNode;
		Ogre::SceneNode* mBodyTiltNode;

		Ogre::SceneNode* mRoomNode;
};

#endif
