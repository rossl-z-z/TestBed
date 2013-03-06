/* OGRE - 3d graphics */

// see http://www.ogre3d.org/ for more info on OGRE

#ifndef __OGRE_main_h_
#define __OGRE_main_h_

#undef USE_TERRAIN;

#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreFrameListener.h>
#include <OgreWindowEventUtilities.h>
#include <OgreMaterialManager.h>

#if defined(USE_TERRAIN)
  #include <Terrain/OgreTerrain.h>
  #include <Terrain/OgreTerrainGroup.h>
#endif

#include "MediaResources.h"
#include "INPUT_main.h"
#include "CEGUI_main.h"

class OGRE_main : public Ogre::FrameListener, public Ogre::WindowEventListener
{
public:
    OGRE_main(void);
    virtual ~OGRE_main(void);

    bool setup(void);
    bool go(void);
    bool exit(void);

    Ogre::RenderWindow* getRenderWindow() { return mWindow; }

    static bool mExiting;
protected:
    virtual void createScene(void);
    virtual void createCamera(void);
    virtual void createViewports(void);    

    virtual void createFrameListener(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
    virtual void windowResized(Ogre::RenderWindow *rw);
    virtual void windowClosed(Ogre::RenderWindow *rw);

    Ogre::Root *mRoot;
    Ogre::Camera *mCamera;
    Ogre::SceneManager *mSceneMgr;
    Ogre::RenderWindow *mWindow;

    CEGUI::OgreRenderer* mRenderer;
private:
    #if defined(USE_TERRAIN)
      Ogre::TerrainGlobalOptions *mTerrainGlobals;
      Ogre::TerrainGroup *mTerrainGroup;
      bool mTerrainsImported;
      void initBlendMaps(Ogre::Terrain *terrain);
      void defineTerrain(long x, long y);
      void configureTerrainDefaults(Ogre::Light *light);
    #endif
};

#endif // #ifndef __OGRE_main_h_
