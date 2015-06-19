//
// Created by shybovycha on 19.06.15.
//

#ifndef TANKS_SCRIPTMANAGER_H
#define TANKS_SCRIPTMANAGER_H

#include <Ogre.h>
#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

class ScriptManager {
public:
    ScriptManager(Ogre::Root *root, Ogre::Camera *camera, Ogre::SceneManager *sceneMgr);

    void runScript(Ogre::String scriptName);

protected:
    void log(Ogre::String message);
    void loadEntity(Ogre::String entityName, Ogre::String meshFilename);
    void setEntityMaterial(Ogre::String entityName, Ogre::String materialName);
    void enableShadowCasting(Ogre::String entityName);
    void disableShadowCasting(Ogre::String entityName);
    void createSceneNode(Ogre::String sceneNodeName, Ogre::String parentNodeName);
    void attachEntityToSceneNode(Ogre::String entityName, Ogre::String sceneNodeName);
    void scaleSceneNode(Ogre::String entityName, Ogre::Real dx, Ogre::Real dy, Ogre::Real dz);
    void translateSceneNode(Ogre::String entityName, Ogre::Real dx, Ogre::Real dy, Ogre::Real dz);
    void rotateSceneNode(Ogre::String entityName, Ogre::Real dx, Ogre::Real dy, Ogre::Real dz);

    Ogre::String findScriptFile(Ogre::String scriptName);

private:
    Ogre::Root *mRoot;
    Ogre::Camera* mCamera;
    Ogre::SceneManager* mSceneMgr;
    chaiscript::ChaiScript* mChai;
};


#endif //TANKS_SCRIPTMANAGER_H
