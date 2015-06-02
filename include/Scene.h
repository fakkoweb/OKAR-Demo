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
		Ogre::Camera* getLeftCamera() { return mCamLeft; }
		Ogre::Camera* getRightCamera() { return mCamRight; }
		Ogre::Camera* getGodCamera() { return mCamGod; }

		// One-call functions for the SeeThrough rig setup
		void setupVideo(const float HFov, const float VFov);
		void setupVideo(const float WSensor, const float HSensor, const float FL);
		void enableVideo();
		void disableVideo();
		void setVideoDistance(const float dist);

		// One-call functions for Oculus rig setup
		void setIPD(float IPD);

		// Update functions
		void update( float dt );
		void setRiftPose( Ogre::Quaternion orientation, Ogre::Vector3 pos );
		void setVideoImagePoseLeft(const Ogre::PixelBox &image, Ogre::Quaternion pose);
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

		bool videoIsEnabled = false;
		float videoScale = 1.0f;	// default value: plane scaled with factor 1, at distance 1 from the eye
									// Warning: real distance between real cameras and the eye is not yet considered/implemented!

		// Initialising/Loading the scene
		void createRoom();
		void createCameras();
		void createVideos(const float WPlane, const float HPlane);

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

		Ogre::SceneNode* mVideoLeft = nullptr;
		Ogre::SceneNode* mHeadNode;
		Ogre::SceneNode* mBodyNode;
		Ogre::SceneNode* mBodyTiltNode;

		Ogre::SceneNode* mRoomNode;
};

#endif
