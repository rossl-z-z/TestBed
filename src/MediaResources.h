/* Location setup of all resources used */

#ifndef __MediaResources_h_
#define __MediaResources_h_

class MediaResources
{
public:
  static void setup(void)
  {
    /* all resource locations... put all in media */
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/materials/programs", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/materials/scripts", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/materials/textures", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/materials/textures/nvidia", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/models", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./resource", "FileSystem", "GUI");
  }
};

#endif // #ifndef __MediaResources_h_
