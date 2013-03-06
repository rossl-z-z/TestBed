#include "Log.h"
#include "OGRE_main.h"

#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgrePlugin.h>

#define USE_DIRECTX
#undef  USE_OPENGL

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
WNDPROC g_ogreWinProc = NULL; // Call Ogre's WinProc after Ours
LRESULT APIENTRY SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

bool OGRE_main::mExiting = false;

OGRE_main::OGRE_main(void)
  : mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0)
{
}
OGRE_main::~OGRE_main(void)
{
#if defined(USE_TERRAIN)
  OGRE_DELETE mTerrainGroup;
  OGRE_DELETE mTerrainGlobals;
#endif

  delete mRoot;
}

void OGRE_main::createFrameListener(void)
{
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing (createFrameListener) ***");

  //Set initial mouse clipping size
  windowResized(mWindow);
 
  //Register as a Window listener
  Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
  mRoot->addFrameListener(this);
}

bool OGRE_main::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  if(mWindow->isClosed())
    return false;

  // poll input if needed
  INPUT_main *im = INPUT_main::getSingletonPtr();
  im->capture();

  //Need to inject timestamps to CEGUI System.
  CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
 
  return true;
}

void OGRE_main::windowResized(Ogre::RenderWindow* rw)
{
  unsigned int width, height, depth;
  int left, top;
  rw->getMetrics(width, height, depth, left, top);

  INPUT_main::getSingletonPtr()->setWindowExtents(width, height);
}
 
void OGRE_main::windowClosed(Ogre::RenderWindow* rw)
{
  if( rw == mWindow )
  {
    delete INPUT_main::getSingletonPtr();
  }
}

#if defined(USE_TERRAIN)
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
  img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  if (flipX)
    img.flipAroundY();
  if (flipY)
    img.flipAroundX();
}
void OGRE_main::defineTerrain(long x, long y)
{
  Ogre::String filename = mTerrainGroup->generateFilename(x, y);
  if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
  {
    mTerrainGroup->defineTerrain(x, y);
  }
  else
  {
    Ogre::Image img;
    getTerrainImage(x % 2 != 0, y % 2 != 0, img);
    mTerrainGroup->defineTerrain(x, y, &img);
    mTerrainsImported = true;
  }
}
void OGRE_main::initBlendMaps(Ogre::Terrain* terrain)
{
  Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
  Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
  Ogre::Real minHeight0 = 70;
  Ogre::Real fadeDist0 = 40;
  Ogre::Real minHeight1 = 70;
  Ogre::Real fadeDist1 = 15;
  float* pBlend0 = blendMap0->getBlendPointer();
  float* pBlend1 = blendMap1->getBlendPointer();
  for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
  {
    for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
    {
      Ogre::Real tx, ty;
      
      blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
      Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
      Ogre::Real val = (height - minHeight0) / fadeDist0;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend0++ = val;
      
      val = (height - minHeight1) / fadeDist1;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend1++ = val;
    }
  }
  blendMap0->dirty();
  blendMap1->dirty();
  blendMap0->update();
  blendMap1->update();
}
void OGRE_main::configureTerrainDefaults(Ogre::Light* light)
{
  // Configure global
  mTerrainGlobals->setMaxPixelError(8);
  // testing composite map
  mTerrainGlobals->setCompositeMapDistance(3000);

  // Important to set these so that the terrain knows what to use for derived (non-realtime) data
  mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
  mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
  mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

  // Configure default import settings for if we use imported image
  Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
  defaultimp.terrainSize = 513;
  defaultimp.worldSize = 12000.0f;
  defaultimp.inputScale = 600; // due terrain.png is 8 bpp
  defaultimp.minBatchSize = 33;
  defaultimp.maxBatchSize = 65;

  // textures
  defaultimp.layerList.resize(3);
  defaultimp.layerList[0].worldSize = 100;
  defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
  defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
  defaultimp.layerList[1].worldSize = 30;
  defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
  defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
  defaultimp.layerList[2].worldSize = 200;
  defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
  defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}
#endif



void OGRE_main::createCamera(void)
{
  // Create the camera
  mCamera = mSceneMgr->createCamera("PlayerCam");

  // mCamera->setPosition(Ogre::Vector3(175,180,-328));
  // mCamera->lookAt(Ogre::Vector3(0,40,0));
  // mCamera->setNearClipDistance(5);

  mCamera->setPosition(Ogre::Vector3(1683, 50, 2116));
  mCamera->lookAt(Ogre::Vector3(1963, 50, 1660));
  mCamera->setNearClipDistance(0.1);
  mCamera->setFarClipDistance(50000);
  
  if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
  {
    mCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
  }
}
 
//-------------------------------------------------------------------------------------
void OGRE_main::createViewports(void)
{
  // create viewports
  // Create one viewport, entire window
  Ogre::Viewport* vp = mWindow->addViewport(mCamera);
  vp->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));

  // Alter the camera aspect ratio to match the viewport
  mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
 
//-------------------------------------------------------------------------------------
void OGRE_main::createScene(void)
{
  // Create the scene
  //Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");
  //Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
  //headNode->attachObject(ogreHead);

  // Ogre::Entity* ogreHead2 = mSceneMgr->createEntity( "Head2", "ogrehead.mesh" );
  // Ogre::SceneNode* headNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode( "HeadNode2", Ogre::Vector3( 0, 20, -40 ) );
  // headNode2->attachObject( ogreHead2 );

  // // Ogre::Entity* entNinja = mSceneMgr->createEntity("Ninja", "ninja.mesh");
  // // entNinja->setCastShadows(true);
  // // mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entNinja);

  // Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
  // Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
  // 						plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

  // Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
  // mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);

  // entGround->setMaterialName("Examples/Rockwall");
  // entGround->setCastShadows(false);

  
  // // Set ambient light
  // mSceneMgr->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));
  // mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
  //                               //Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
  //                               //Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

  // Create a light

  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
  Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);

  Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
  lightdir.normalise();
  
  Ogre::Light* light = mSceneMgr->createLight("tstLight");
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  light->setDirection(lightdir);
  light->setDiffuseColour(Ogre::ColourValue::White);
  light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
  
  mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));


#if defined (USE_TERRAIN)
  // Terrain
  mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
 
  mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
  mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
  mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
  
  configureTerrainDefaults(light);
  
  for (long x = 0; x <= 0; ++x)
    for (long y = 0; y <= 0; ++y)
      defineTerrain(x, y);
  
  // sync load since we want everything in place when we start
  mTerrainGroup->loadAllTerrains(true);
  
  if (mTerrainsImported)
  {
    Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
    while(ti.hasMoreElements())
    {
      Ogre::Terrain* t = ti.getNext()->instance;
      initBlendMaps(t);
    }
  }
  
  mTerrainGroup->freeTemporaryResources();
#endif


  // Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
  // pointLight->setType(Ogre::Light::LT_POINT);
  // pointLight->setPosition(Ogre::Vector3(0, 150, 250));
  
  // pointLight->setDiffuseColour(1.0, 0.0, 0.0);
  // pointLight->setSpecularColour(1.0, 0.0, 0.0);
  
  // Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight");
  // directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
  // directionalLight->setDiffuseColour(Ogre::ColourValue(.25, .25, 0));
  // directionalLight->setSpecularColour(Ogre::ColourValue(.25, .25, 0));
 
  // directionalLight->setDirection(Ogre::Vector3( 0, -1, 1 )); 
  
  // Ogre::Light* spotLight = mSceneMgr->createLight("spotLight");
  // spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
  // spotLight->setDiffuseColour(0, 0, 1.0);
  // spotLight->setSpecularColour(0, 0, 1.0);
  
  // spotLight->setDirection(-1, -1, 0);
  // spotLight->setPosition(Ogre::Vector3(300, 300, 0));
  
  // spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));


//  Ogre::Plane plane;
//  plane.d = 10;
//  plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
//
//  mSceneMgr->setSkyPlane(true, plane, "Examples/CloudySky", 500, 20, true, 1.5f, 150, 150);
//

  Ogre::ColourValue fadeColour(0.1, 0.1, 0.1);
  mWindow->getViewport(0)->setBackgroundColour(fadeColour);

  mSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0, 10, 1200);
}


bool OGRE_main::setup(void)
{
  // construct Ogre::Root : no plugins filename, no config filename, using a custom log filename
  mRoot = new Ogre::Root("", "", "OGRE.log");

  // A list of required plugins and plugins to load
  Ogre::StringVector required_plugins;
  Ogre::StringVector plugins_toLoad;
#if defined(USE_DIRECTX)
  required_plugins.push_back("D3D9 RenderSystem");
  plugins_toLoad.push_back("RenderSystem_Direct3D9");
#elif defined(USE_OPENGL)
  required_plugins.push_back("GL RenderSystem");
  plugins_toLoad.push_back("RenderSystem_GL");
#endif
  required_plugins.push_back("Octree Scene Manager");
  plugins_toLoad.push_back("Plugin_OctreeSceneManager");
   
  for (Ogre::StringVector::iterator j = plugins_toLoad.begin(); j != plugins_toLoad.end(); j++)
  {
#ifdef _DEBUG
    mRoot->loadPlugin(*j + Ogre::String("_d"));
#else
    mRoot->loadPlugin(*j);
#endif
  }

  // Check if the required plugins are installed and ready for use
  // If not: exit the application
  Ogre::Root::PluginInstanceList ip = mRoot->getInstalledPlugins();
  for (Ogre::StringVector::iterator j = required_plugins.begin(); j != required_plugins.end(); j++)
  {
    bool found = false;
    // try to find the required plugin in the current installed plugins
    for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); k++)
    {
      if ((*k)->getName() == *j)
      {
        found = true;
        break;
      }
    }
    if (!found)  // return false because a required plugin is not available
    {
      FILE_LOG(logERROR) << "Error Loading plugin " << *j;
      return false;
    }
  }

  // configure
  // Grab the DirectX or OpenGL RenderSystem, or exit
#if defined(USE_DIRECTX)
  const char *rendersys = "Direct3D9 Rendering Subsystem";
#elif defined(USE_OPENGL)
  const char *rendersys = "OpenGL Rendering Subsystem";
#endif
  Ogre::RenderSystem* rs = mRoot->getRenderSystemByName(rendersys);
  if(!(rs->getName() == rendersys))
  {
	FILE_LOG(logERROR) << "No RenderSystem found!!!";
    return false; // No RenderSystem found
  }

  // configure our RenderSystem
  rs->setConfigOption("Full Screen", "No");
  //rs->setConfigOption("VSync", "No");
  //rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit");

  mRoot->setRenderSystem(rs);

  mWindow = mRoot->initialise(true, "OGRE Render Window");

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  // call both custom wndproc and orges
  HWND windowHnd;
  mWindow->getCustomAttribute("WINDOW", &windowHnd);
  g_ogreWinProc = (WNDPROC) SetWindowLong(windowHnd, GWL_WNDPROC, (LONG) SubclassProc);

  // move the mouse to the middle of the window (where CEGUI expects it is)
  RECT rcWnd;
  GetWindowRect(windowHnd, &rcWnd);
  SetCursorPos(rcWnd.left + ((rcWnd.right-rcWnd.left)/2), 
    rcWnd.top + ((rcWnd.bottom-rcWnd.top)/2));
#endif

  // Get the SceneManager, in this case the OctreeSceneManager
  mSceneMgr = mRoot->createSceneManager("OctreeSceneManager", "DefaultSceneManager");

  // setup resources
  MediaResources::setup();

  return true;
}

bool OGRE_main::go(void)
{
  if (!setup())
    return false;

  createCamera();
  createViewports();

#if defined(USE_TERRAIN)
  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
#endif

  // load resources
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  createScene();
  createFrameListener();

  CEGUI_main::getSingletonPtr()->create(this);
  INPUT_main::getSingletonPtr()->initialise(mWindow);

  while(true)
  {
    Ogre::WindowEventUtilities::messagePump();
    if (!mExiting)
      mRoot->renderOneFrame();

    if(mWindow->isClosed())
    {
      return false;
    }
  }

  return true;
}

bool OGRE_main::exit(void)
{
  mExiting = true;
  ::exit(0);
  return true;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
// winproc code, which calls ogre's one when finished.
LRESULT APIENTRY SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {
   // handle WM_**** stuff here
   case WM_CLOSE:
     FILE_LOG(logERROR) << "Exiting via WM_CLOSE";
     OGRE_main::mExiting = true;
     break;
   case WM_INPUT:
     // pass input events to INPUT_main
     INPUT_main::getSingletonPtr()->inputEvent(uMsg, wParam, lParam);
     break;
   case WM_MOUSELEAVE:
     INPUT_main::getSingletonPtr()->inputEvent(uMsg, wParam, lParam);
     break;
   }
   return CallWindowProc(g_ogreWinProc, hwnd, uMsg, wParam, lParam);
}
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
  int main(int argc, char *argv[])
#endif
  {
	// Setup log file
	  FILE* pFile = fopen("errors.log", "a");
    Output2FILE::Stream() = pFile;

    // Create application object
    OGRE_main app;

    try {
      app.go();
    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      FILE_LOG(logERROR) << "An exception has occured: " << e.getFullDescription().c_str();
      MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
  	  FILE_LOG(logERROR) << "An exception has occured: " << e.getFullDescription().c_str();
#endif
    }
    return 0;
  }

#ifdef __cplusplus
}
#endif
