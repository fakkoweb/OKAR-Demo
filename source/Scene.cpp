#include "Scene.h"

Scene::Scene( Ogre::Root* root, OIS::Mouse* mouse, OIS::Keyboard* keyboard )
{
	mRoot = root;
	mMouse = mouse;
	mKeyboard = keyboard;
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC); 

	// Set up main Lighting and Shadows in Scene:
	mSceneMgr->setAmbientLight( Ogre::ColourValue(0.1f,0.1f,0.1f) );
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	mSceneMgr->setShadowFarDistance( 30 );

	createRoom();
	createCameras();
	createVideos();
}

Scene::~Scene()
{
	if (mSceneMgr) delete mSceneMgr;
}

void Scene::createRoom()
{
	mRoomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RoomNode");

	Ogre::SceneNode* cubeNode = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt->getSubEntity(0)->setMaterialName( "CubeMaterialRed" );
	cubeNode->attachObject( cubeEnt );
	cubeNode->setPosition( 1.0, 0.0, 0.0 );
	Ogre::SceneNode* cubeNode2 = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt2 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt2->getSubEntity(0)->setMaterialName( "CubeMaterialGreen" );
	cubeNode2->attachObject( cubeEnt2 );
	cubeNode2->setPosition( 3.0, 0.0, 0.0 );
	cubeNode->setScale( 0.5, 0.5, 0.5 );
	cubeNode2->setScale( 0.5, 0.5, 0.5 );
	
	Ogre::SceneNode* cubeNode3 = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt3 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt3->getSubEntity(0)->setMaterialName( "CubeMaterialWhite" );
	cubeNode3->attachObject( cubeEnt3 );
	cubeNode3->setPosition( -1.0, 0.0, 0.0 );
	cubeNode3->setScale( 0.5, 0.5, 0.5 );

	Ogre::Entity* roomEnt = mSceneMgr->createEntity( "Room.mesh" );
	roomEnt->setCastShadows( false );
	mRoomNode->attachObject( roomEnt );

	Ogre::Light* roomLight = mSceneMgr->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows( false );
	roomLight->setShadowFarDistance( 30 );
	roomLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	roomLight->setSpecularColour( .25, .25, .25 );
	roomLight->setDiffuseColour( 0.85, 0.76, 0.7 );

	roomLight->setPosition( 5, 5, 5 );

	mRoomNode->attachObject( roomLight );
}

void Scene::createCameras()
{
	mCamLeft = mSceneMgr->createCamera("LeftCamera");
	mCamRight = mSceneMgr->createCamera("RightCamera");
	mCamGod = mSceneMgr->createCamera("GodCamera");

	// Create a scene nodes which the virtual stereo cams will be attached to:
	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BodyNode");
	mBodyTiltNode = mBodyNode->createChildSceneNode();
	mHeadNode = mBodyTiltNode->createChildSceneNode("HeadNode"); 
	mBodyNode->setFixedYawAxis( true );	// don't roll!  

	mHeadNode->attachObject(mCamLeft);
	mHeadNode->attachObject(mCamRight);
	
	// Position cameras to a STANDARD INITIAL interpupillary distance
	// It should be set later by calling setIPD() method
	float dist = 0.064f;
	/*if (mRift->isAttached())
	{
		dist = mRift->getStereoConfig().GetIPD();
	}*/
	mCamLeft->setPosition( -dist/2.0f, 0.0f, 0.0f );
	mCamRight->setPosition( dist/2.0f, 0.0f, 0.0f );

	// If possible, get the camera projection matricies given by the rift:	
	/*if (mRift->isAttached())
	{
		mCamLeft->setCustomProjectionMatrix( true, mRift->getProjectionMatrix_Left() );
		mCamRight->setCustomProjectionMatrix( true, mRift->getProjectionMatrix_Right() );
	}*/
	mCamLeft->setFarClipDistance( 50 );
	mCamRight->setFarClipDistance( 50 );

	mCamLeft->setNearClipDistance( 0.001 );
	mCamRight->setNearClipDistance( 0.001 );

	/*mHeadLight = mSceneMgr->createLight();
	mHeadLight->setType(Ogre::Light::LT_POINT);
	mHeadLight->setCastShadows( false );
	mHeadLight->setShadowFarDistance( 30 );
	mHeadLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	mHeadLight->setSpecularColour( 1.0, 1.0, 1.0 );
	mHeadLight->setDiffuseColour( 1.0, 1.0, 1.0 );
	mHeadNode->attachObject( mHeadLight );*/

	mBodyNode->setPosition( 4.0, 1.5, 4.0 );
	//mBodyNode->lookAt( Ogre::Vector3::ZERO, Ogre::SceneNode::TS_WORLD );


	// Position God camera according to the room
	mRoomNode->attachObject(mCamGod);
	mCamGod->setFarClipDistance(50);
	mCamGod->setNearClipDistance(0.001);
	mCamGod->setPosition(5, 5, 5);
	mCamGod->lookAt(mBodyNode->getPosition());


	// Position a light on the body
	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65, 1.0, 0.07, 0.017 );
	light->setSpecularColour( .25, .25, .25 );
	light->setDiffuseColour( 0.35, 0.27, 0.23 );
	mBodyNode->attachObject(light);
}

void Scene::createVideos()
{

	//Create an Plane class instance that describes our plane (no position or orientation, just mathematical description)
	Ogre::Plane videoPlane(Ogre::Vector3::UNIT_Z, 0);

	//Create a static mesh out of the plane (as a REUSABLE "resource")
	Ogre::MeshManager::getSingleton().createPlane(
		"videoMesh",										// this is the name that our resource will have for the whole application!
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		videoPlane,											// this is the instance from which we build the mesh
		1, 1, 20, 20,
		true,
		1, 5, 5,
		Ogre::Vector3::UNIT_Y);								// this is the vector that will be used as mesh UP direction

	//Create an ogre Entity out of the resource we created (more Entities can be created out of a resource!)
	Ogre::Entity* videoPlaneEntity = mSceneMgr->createEntity("videoMesh");

	//Prepare an Ogre SceneNode where we will attach the newly created Entity (as child of mHeadNode)
	mVideoLeft = mHeadNode->createChildSceneNode("LeftVideo");

	//Attach videoPlaneEntity to mVideoLeft SceneNode (now it will have a Position/Scale/Orientation
	mVideoLeft->attachObject(videoPlaneEntity);

	//Last two operations could have also been done in one step, but we would not get the SceneNode pointer to save in mVideoLeft
	// mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(videoPlaneEntity);
	
	//Setup videoPlaneEntity rendering proprieties (INDEPENDENT FROM mVideoLeft SceneNode!!)
	videoPlaneEntity->setCastShadows(false);
	videoPlaneEntity->setMaterialName("CubeMaterialWhite");
	
	//Setup mVideoLeft SceneNode position/scale/orientation
	// X-axis:	will be set up the same as virtual cameras IPD -> see setIPD()
	// Y-axis:	0 = solidal with mHeadNode
	// Z-axis:	will be set up depending on the physical camera FOV -> see setFOV()
	//			toghether with its scale (scale and z position are proportional)
	mVideoLeft->setPosition(0, 0, -1.0);

	//Set camera listeners to this class (so that I can do stuff before and after each renders)
	mCamLeft->addListener(this);
	
}

void Scene::update( float dt )
{
	float forward = (mKeyboard->isKeyDown( OIS::KC_W ) ? 0 : 1) + (mKeyboard->isKeyDown( OIS::KC_S ) ? 0 : -1);
	float leftRight = (mKeyboard->isKeyDown( OIS::KC_A ) ? 0 : 1) + (mKeyboard->isKeyDown( OIS::KC_D ) ? 0 : -1);

	if( mKeyboard->isKeyDown( OIS::KC_LSHIFT ) )
	{
		forward *= 3;
		leftRight *= 3;
	}
	
	Ogre::Vector3 dirX = mBodyTiltNode->_getDerivedOrientation()*Ogre::Vector3::UNIT_X;
	Ogre::Vector3 dirZ = mBodyTiltNode->_getDerivedOrientation()*Ogre::Vector3::UNIT_Z;

	mBodyNode->setPosition( mBodyNode->getPosition() + dirZ*forward*dt + dirX*leftRight*dt );
}

//////////////////////////////////////////////////////////////
// Handle Rift Input:
//////////////////////////////////////////////////////////////

void Scene::setRiftPose( Ogre::Quaternion orientation, Ogre::Vector3 pos )
{
	mHeadNode->setOrientation( orientation );
	mHeadNode->setPosition( pos );
}

void Scene::setIPD( float IPD )
{
	mCamLeft->setPosition( -IPD/2.0f, 0.0f, 0.0f );
	mCamRight->setPosition( IPD/2.0f, 0.0f, 0.0f );

	//mVideoLeft->setPosition(-IPD / 2.0f, mVideoLeft->getPosition().y, mVideoLeft->getPosition().z);
	//mVideoRight->setPosition(IPD / 2.0f, mVideoLeft->getPosition().y, mVideoLeft->getPosition().z);
}

//////////////////////////////////////////////////////////////
// Handle Camera Input:
//////////////////////////////////////////////////////////////
void Scene::setCameraTextureLeft(const cv::Mat &image, Ogre::Quaternion pose)
{}

//////////////////////////////////////////////////////////////
// Handle Input:
//////////////////////////////////////////////////////////////

bool Scene::keyPressed( const OIS::KeyEvent& e )
{
	return true;
}
bool Scene::keyReleased( const OIS::KeyEvent& e )
{
	return true;
}
bool Scene::mouseMoved( const OIS::MouseEvent& e )
{
	if( mMouse->getMouseState().buttonDown( OIS::MB_Left ) )
	{
		mBodyNode->yaw( Ogre::Degree( -0.3*e.state.X.rel ) );
		mBodyTiltNode->pitch( Ogre::Degree( -0.3*e.state.Y.rel ) );
	}
	return true;
}
bool Scene::mousePressed( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}
bool Scene::mouseReleased( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}

//////////////////////////////////////////////////////////////
// Handle Virtual Camera Events:
//////////////////////////////////////////////////////////////
void Scene::cameraPreRenderScene(Ogre::Camera* cam)
{
	if (cam == mCamLeft)
	{
		mVideoLeft->setVisible(true, false);
	}
}

void Scene::cameraPostRenderScene(Ogre::Camera* cam)
{
	if (cam == mCamLeft)
	{
		mVideoLeft->setVisible(false, false);
	}
}
