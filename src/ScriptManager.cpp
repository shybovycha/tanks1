#include "ScriptManager.h"

ScriptManager::ScriptManager(Ogre::Root *root, Ogre::Camera *camera, Ogre::SceneManager *sceneMgr) :
        mRoot(root),
        mCamera(camera),
        mSceneMgr(sceneMgr),
        mChai(new chaiscript::ChaiScript(chaiscript::Std_Lib::library()))
{
    mChai->add(chaiscript::fun(&ScriptManager::log, this), "log");
    mChai->add(chaiscript::fun(&ScriptManager::loadEntity, this), "loadEntity");
    mChai->add(chaiscript::fun(&ScriptManager::setEntityMaterial, this), "setEntityMaterial");
    mChai->add(chaiscript::fun(&ScriptManager::enableShadowCasting, this), "enableShadowCasting");
    mChai->add(chaiscript::fun(&ScriptManager::disableShadowCasting, this), "disableShadowCasting");
    mChai->add(chaiscript::fun(&ScriptManager::createSceneNode, this), "createSceneNode");
    mChai->add(chaiscript::fun(&ScriptManager::attachEntityToSceneNode, this), "attachEntityToSceneNode");
    mChai->add(chaiscript::fun(&ScriptManager::scaleSceneNode, this), "scaleSceneNode");
    mChai->add(chaiscript::fun(&ScriptManager::translateSceneNode, this), "translateSceneNode");
    mChai->add(chaiscript::fun(&ScriptManager::rotateSceneNode, this), "rotateSceneNode");
}

Ogre::String ScriptManager::findScriptFile(Ogre::String scriptFileName) {
    std::string resourceGroup(Ogre::ResourceGroupManager::getSingletonPtr()->findGroupContainingResource(scriptFileName));
    Ogre::FileInfoListPtr candidates = Ogre::ResourceGroupManager::getSingletonPtr()->listResourceFileInfo(resourceGroup);

    if (candidates->size() < 1) {
        throw new Ogre::Exception(1, "No file found for script " + scriptFileName, "ScriptManager");
    }

    std::string filename = candidates->at(0).archive->getName() + "/" + candidates->at(0).filename;

    return filename;
}

void ScriptManager::runScript(Ogre::String scriptName) {
    Ogre::String filename = findScriptFile("tank1_loader.chai");

    log("Evaluating script file " + filename);

    mChai->eval_file(filename);
}

void ScriptManager::setEntityMaterial(Ogre::String entityName, Ogre::String materialName) {
    Ogre::Entity *entity = mSceneMgr->getEntity(entityName);
    entity->setMaterialName(materialName);
}

void ScriptManager::enableShadowCasting(Ogre::String entityName) {
    Ogre::Entity *entity = mSceneMgr->getEntity(entityName);
    entity->setCastShadows(true);
}

void ScriptManager::disableShadowCasting(Ogre::String entityName) {
    Ogre::Entity *entity = mSceneMgr->getEntity(entityName);
    entity->setCastShadows(false);
}

void ScriptManager::loadEntity(Ogre::String entityName, Ogre::String meshFilename) {
    log("Script requested to create entity " + entityName + " from " + meshFilename);
    mSceneMgr->createEntity(entityName, meshFilename);
}

void ScriptManager::createSceneNode(Ogre::String sceneNodeName, Ogre::String parentNodeName) {
    Ogre::SceneNode *parent;

    if (parentNodeName.empty() || parentNodeName == "root") {
        parent = mSceneMgr->getRootSceneNode() ; //->createChildSceneNode(Ogre::Vector3(0, 5, 0));
    } else {
        parent = mSceneMgr->getSceneNode(parentNodeName);
    }

    parent->createChildSceneNode(sceneNodeName);
}

void ScriptManager::attachEntityToSceneNode(Ogre::String entityName, Ogre::String sceneNodeName) {
    Ogre::SceneNode *parent = mSceneMgr->getSceneNode(sceneNodeName);
    Ogre::Entity *entity = mSceneMgr->getEntity(entityName);
    parent->attachObject(entity);
}

void ScriptManager::scaleSceneNode(Ogre::String nodeName, Ogre::Real dx, Ogre::Real dy, Ogre::Real dz) {
    Ogre::SceneNode *node = mSceneMgr->getSceneNode(nodeName);
    node->scale(dx, dy, dz);
}

void ScriptManager::translateSceneNode(Ogre::String nodeName, Ogre::Real dx, Ogre::Real dy, Ogre::Real dz) {
    Ogre::SceneNode *node = mSceneMgr->getSceneNode(nodeName);
    node->translate(dx, dy, dz);
}

void ScriptManager::rotateSceneNode(Ogre::String nodeName, Ogre::Real dx, Ogre::Real dy, Ogre::Real dz) {
    Ogre::SceneNode *node = mSceneMgr->getSceneNode(nodeName);
    Ogre::Matrix3 mat;
    mat.FromEulerAnglesXYZ(Ogre::Radian(Ogre::Degree(dx)), Ogre::Radian(Ogre::Degree(dy)), Ogre::Radian(Ogre::Degree(dz)));
    node->rotate(Ogre::Quaternion(mat));
}

void ScriptManager::log(Ogre::String message) {
    Ogre::LogManager::getSingletonPtr()->logMessage(message);
}