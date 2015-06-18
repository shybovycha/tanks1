#include "TanksApplication.h"

TanksApplication::TanksApplication(void)
        : mRoot(0),
          mCamera(0),
          mSceneMgr(0),
          mWindow(0),
          mResourcesCfg(Ogre::StringUtil::BLANK),
          mPluginsCfg(Ogre::StringUtil::BLANK),
          mTrayMgr(0),
          mCameraMan(0),
          mDetailsPanel(0),
          mCursorWasVisible(false),
          mShutDown(false),
          mInputManager(0),
          mMouse(0),
          mOverlaySystem(0),
          mSkeletonDebugger(0),
          mKeyboard(0),
          mChai(new chaiscript::ChaiScript(chaiscript::Std_Lib::library()))
          { }

TanksApplication::~TanksApplication(void) {
    if (mTrayMgr) delete mTrayMgr;
    if (mCameraMan) delete mCameraMan;
    if (mSkeletonDebugger) delete mSkeletonDebugger;
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

void TanksApplication::loadEntity(Ogre::String entityName, Ogre::String meshFilename) {
    Ogre::LogManager::getSingletonPtr()->logMessage("--- Loading entity:" + entityName + " from file: " + meshFilename);
    mSceneMgr->createEntity(entityName, meshFilename);
}

void TanksApplication::createSceneNode(Ogre::String sceneNodeName, Ogre::String parentNodeName) {
    Ogre::SceneNode *parent;

    if (parentNodeName.empty() || parentNodeName == "root") {
        parent = mSceneMgr->getRootSceneNode() ; //->createChildSceneNode(Ogre::Vector3(0, 5, 0));
    } else {
        parent = mSceneMgr->getSceneNode(parentNodeName);
    }

    parent->createChildSceneNode(sceneNodeName);
}

void TanksApplication::attachEntityToSceneNode(Ogre::String entityName, Ogre::String sceneNodeName) {
    Ogre::SceneNode *parent = mSceneMgr->getSceneNode(sceneNodeName);
    Ogre::Entity *entity = mSceneMgr->getEntity(entityName);
    parent->attachObject(entity);
}

void TanksApplication::scaleSceneNode(Ogre::String nodeName, Ogre::Real dx, Ogre::Real dy, Ogre::Real dz) {
    Ogre::SceneNode *node = mSceneMgr->getSceneNode(nodeName);
    node->scale(dx, dy, dz);
}

void TanksApplication::translateSceneNode(Ogre::String nodeName, Ogre::Real dx, Ogre::Real dy, Ogre::Real dz) {
    Ogre::SceneNode *node = mSceneMgr->getSceneNode(nodeName);
    node->translate(dx, dy, dz);
}

void TanksApplication::rotateSceneNode(Ogre::String nodeName, Ogre::Real dx, Ogre::Real dy, Ogre::Real dz) {
    Ogre::SceneNode *node = mSceneMgr->getSceneNode(nodeName);
    Ogre::Matrix3 mat;
    mat.FromEulerAnglesXYZ(Ogre::Radian(Ogre::Degree(dx)), Ogre::Radian(Ogre::Degree(dy)), Ogre::Radian(Ogre::Degree(dz)));
    node->rotate(Ogre::Quaternion(mat));
}

void TanksApplication::log(Ogre::String message) {
    Ogre::LogManager::getSingletonPtr()->logMessage(message);
}

//СОЗДАНИЕ РУТА И ЗАГРУЗКА ФАЙЛОВ КОНФИГУРАЦИИ
//====================================================
bool TanksApplication::go(void) {
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    mRoot = new Ogre::Root(mPluginsCfg);
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    Ogre::String secName, typeName, archName;

    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();

        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;

        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName,
                                                                           secName);
        }
    }

    if (mRoot->restoreConfig() || mRoot->showConfigDialog()) {

        //СОЗДАЕМ ОКНО И ЕГО ЗАГОЛОВОК
        //====================================================
        mWindow = mRoot->initialise(true, "test1");
    }
    else {
        return false;
    }

    //СОЗДАЕМ СЦЕНУ
    //====================================================
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

    //СОЗДАЕМ ОВЕРЛЕЙ
    mOverlaySystem = new Ogre::OverlaySystem();
    mSceneMgr->addRenderQueueListener(mOverlaySystem);

    //СОЗДАЕМ КАМЕРУ
    //====================================================
    mCamera = mSceneMgr->createCamera("Main camera");
    mCamera->setPosition(Ogre::Vector3(0, 100, -200));//позиция камеры
    mCamera->lookAt(Ogre::Vector3(0, 50, 0));//позиция точки взгляда камеры
    mCamera->setNearClipDistance(1.5f);//обрезание видимости
    mCamera->setFarClipDistance(3000.0f);//обрезание видимости
    mCameraMan = new OgreBites::SdkCameraMan(mCamera);
    Ogre::Viewport *vp = mWindow->addViewport(mCamera);

    //Цвет фона, не особо нужен так как все равно использовать будем скайбокс
    vp->setBackgroundColour(Ogre::ColourValue(0.2, 0.2, 0.2));
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) /
                            Ogre::Real(vp->getActualHeight()));
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    //ЗАГРУЖАЕМ МОДЕЛИ
    //====================================================
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    //ПОЛ
    //====================================================
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::
                                                  DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 20, 20, true, 1, 32,
                                                  32,
                                                  Ogre::Vector3::UNIT_Z);
    Ogre::Entity *entGround = mSceneMgr->createEntity("GroundEntity", "ground");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);
    entGround->setMaterialName("Examples/BumpyMetal");//назначаем материал
    entGround->setCastShadows(false);//выключаем отброс тени

    std::cerr << "--- Hello, Floor!\n";

    //Tank1
    mChai->add(chaiscript::fun(&TanksApplication::log, this), "log");
    mChai->add(chaiscript::fun(&TanksApplication::loadEntity, this), "loadEntity");
    mChai->add(chaiscript::fun(&TanksApplication::createSceneNode, this), "createSceneNode");
    mChai->add(chaiscript::fun(&TanksApplication::attachEntityToSceneNode, this), "attachEntityToSceneNode");
    mChai->add(chaiscript::fun(&TanksApplication::scaleSceneNode, this), "scaleSceneNode");
    mChai->add(chaiscript::fun(&TanksApplication::translateSceneNode, this), "translateSceneNode");
    mChai->add(chaiscript::fun(&TanksApplication::rotateSceneNode, this), "rotateSceneNode");

    // eval script
    std::string scriptFileName = "tank1_loader.chai";
    std::string resourceGroup(Ogre::ResourceGroupManager::getSingletonPtr()->findGroupContainingResource(scriptFileName));
    Ogre::FileInfoListPtr FInfoList = Ogre::ResourceGroupManager::getSingletonPtr()->listResourceFileInfo("Scripts");

    Ogre::LogManager::getSingletonPtr()->logMessage("--- Loading script from file:");
    Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(FInfoList->size()));
    Ogre::LogManager::getSingletonPtr()->logMessage(FInfoList->at(0).archive->getName() + "/" + FInfoList->at(0).filename);

    mChai->eval_file(FInfoList->at(0).archive->getName() + "/" + FInfoList->at(0).filename);
    // std::string result = mChai.eval_file<std::string>("myfile.chai");

    //====================================================
    /*Ogre::Entity *tank1Entity = mSceneMgr->createEntity("tank1", "Body1.mesh");
    Ogre::SceneNode *tank1 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 5, 0));
    tank1->attachObject(tank1Entity);
    //tank1->scale(40, 40, 40);//увеличиваем
    //Ogre::SkeletonInstance *tank1Skeleton = tank1Entity->getSkeleton();
    //Ogre::Bone *tank1TowerBone = tank1Skeleton->getBone("TowerBone");

    Ogre::Entity *tower1Entity = mSceneMgr->createEntity("tower1", "Tower1.mesh");
    //Ogre::SkeletonInstance *tower1Skeleton = tower1Entity->getSkeleton();
    // Ogre::Bone *tower1GunBone = tower1Skeleton->getBone("GunBone");

    Ogre::Matrix3 tower1Orientation;
    //tower1Orientation.FromEulerAnglesXYZ(Ogre::Degree(0), Ogre::Degree(-90), Ogre::Degree(0));
    //Ogre::Vector3 tower1Offset(0, 0, 0);
    Ogre::TagPoint *tower1 = tank1Entity->attachObjectToBone("TowerBone", tower1Entity);
    //tower1->scale(0.4, 0.4, 0.4);

    Ogre::Entity *gun1Entity = mSceneMgr->createEntity("gun1", "Gun1.mesh");
    Ogre::Matrix3 gun1Orientation;
    //gun1Orientation.FromEulerAnglesXYZ(Ogre::Degree(0), Ogre::Degree(90), Ogre::Degree(0));
    //Ogre::Vector3 gun1Offset(0, 0, 0);
    Ogre::TagPoint *gun1 = tower1Entity->attachObjectToBone("GunBone", gun1Entity);
    //gun1->scale(0.1, 0.1, 0.1);

    mSkeletonDebugger = new SkeletonDebug(tank1Entity, mSceneMgr, mCamera);

    std::vector<Ogre::Entity *> wheelEntities;
    std::vector<Ogre::TagPoint *> wheels;

    for (int i = 1; i <= 3; i++) {
        Ogre::String tagName = (boost::format("left_wheel%1%") % i).str();
        Ogre::String boneName = (boost::format("LeftWheel%1%Bone") % i).str();
        Ogre::Entity *entity = mSceneMgr->createEntity(tagName, "Wheel.mesh");
        Ogre::TagPoint *wheel = tank1Entity->attachObjectToBone(boneName, entity);
        //wheel->scale(0.25, 0.25, 0.25);

        wheelEntities.push_back(entity);
        wheels.push_back(wheel);
    }

    for (int i = 1; i <= 3; i++) {
        Ogre::String tagName = (boost::format("right_wheel%1%") % i).str();
        Ogre::String boneName = (boost::format("RightWheel%1%Bone") % i).str();
        Ogre::Entity *entity = mSceneMgr->createEntity(tagName, "Wheel.mesh");
        Ogre::TagPoint *wheel = tank1Entity->attachObjectToBone(boneName, entity);
        //wheel->scale(0.25, 0.25, 0.25);

        wheelEntities.push_back(entity);
        wheels.push_back(wheel);
    }

    //tank1TowerBone->addChild(tower1);*/

    //ГОТОВИМ АНИМАЦИЮ МОДЕЛИ JAIQUA
    //====================================================
    /*Ogre::Animation::setDefaultInterpolationMode(Ogre::Animation::IM_LINEAR);
    Ogre::Animation::setDefaultRotationInterpolationMode(Ogre::Animation::RIM_LINEAR);
    mAnimation = Jaiqua->getAnimationState("Sneak");*/
    //mCharSceneNode->pitch(Ogre::Degree(90));
    //mCharSceneNode->yaw(Ogre::Degree(-128));

    //ВКЛЮЧАЕМ СВЕТ, СКАЙБОКС, ТЕНИ И ПРОЧЕЕ
    //====================================================
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox", 5, 8);
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
    Ogre::Light *l = mSceneMgr->createLight("Main light");
    l->setPosition(0, 200, -200);
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    mInputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard *>(mInputManager->createInputObject(
            OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse *>(mInputManager->createInputObject(
            OIS::OISMouse, true));
    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
    windowResized(mWindow);
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
    //mTrayMgr = new OgreBites::SdkTrayManager("Interface name", mWindow, mMouse, this);
    OgreBites::InputContext inputContext;
    inputContext.mMouse = mMouse;
    inputContext.mKeyboard = mKeyboard;
    mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, inputContext, this);
    mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    mTrayMgr->hideCursor();
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
    mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "Details panel",
                                                200, items);
    mDetailsPanel->setParamValue(9, "Bilinear");
    mDetailsPanel->setParamValue(10, "Solid");
    mDetailsPanel->hide();
    mRoot->addFrameListener(this);
    mRoot->startRendering();
    return true;
}

bool TanksApplication::frameRenderingQueued(const Ogre::FrameEvent &evt) {
    if (mWindow->isClosed()) return false;
    if (mShutDown) return false;
    mKeyboard->capture();
    mMouse->capture();
    mTrayMgr->frameRenderingQueued(evt);
    if (!mTrayMgr->isDialogVisible()) {
        mCameraMan->frameRenderingQueued(evt);
        if (mDetailsPanel->isVisible()) {
            mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->
                    getDerivedPosition().x));
            mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->
                    getDerivedPosition().y));
            mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->
                    getDerivedPosition().z));
            mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mCamera->
                    getDerivedOrientation().w));
            mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mCamera->
                    getDerivedOrientation().x));
            mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mCamera->
                    getDerivedOrientation().y));
            mDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(mCamera->
                    getDerivedOrientation().z));
        }

        //ВКЛЮЧАЕМ АНИМАЦИЮ МОДЕЛИ JAIQUA
        //====================================================
        /*mAnimation->setLoop(true);
        mAnimation->addTime(evt.timeSinceLastFrame);
        mAnimation->setEnabled(true);*/
    }
    return true;
}

//ОБРАБОТКА КЛАВИШ, ПОКАЗА\СКРЫТИЯ ВИДЖЕТОВ
//====================================================
bool TanksApplication::keyPressed(const OIS::KeyEvent &arg) {
    if (mTrayMgr->isDialogVisible()) return true;
    if (arg.key == OIS::KC_F) {
        mTrayMgr->toggleAdvancedFrameStats();
    }
    else if (arg.key == OIS::KC_G) {
        if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE) {
            mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
            mDetailsPanel->show();
        }
        else {
            mTrayMgr->removeWidgetFromTray(mDetailsPanel);
            mDetailsPanel->hide();
        }
    }
    else if (arg.key == OIS::KC_T) {
        Ogre::String newVal;
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;
        switch (mDetailsPanel->getParamValue(9).asUTF8()[0]) {
            case 'B':
                newVal = "Trilinear";
                tfo = Ogre::TFO_TRILINEAR;
                aniso = 1;
                break;
            case 'T':
                newVal = "Anisotropic";
                tfo = Ogre::TFO_ANISOTROPIC;
                aniso = 8;
                break;
            case 'A':
                newVal = "None";
                tfo = Ogre::TFO_NONE;
                aniso = 1;
                break;
            default:
                newVal = "Bilinear";
                tfo = Ogre::TFO_BILINEAR;
                aniso = 1;
        }
        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
        mDetailsPanel->setParamValue(9, newVal);
    }
    else if (arg.key == OIS::KC_R) {
        Ogre::String newVal;
        Ogre::PolygonMode pm;
        switch (mCamera->getPolygonMode()) {
            case Ogre::PM_SOLID:
                newVal = "Wireframe";
                pm = Ogre::PM_WIREFRAME;
                break;
            case Ogre::PM_WIREFRAME:
                newVal = "Points";
                pm = Ogre::PM_POINTS;
                break;
            default:
                newVal = "Solid";
                pm = Ogre::PM_SOLID;
        }
        mCamera->setPolygonMode(pm);
        mDetailsPanel->setParamValue(10, newVal);
    }
    else if (arg.key == OIS::KC_F5) {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ) {
        mWindow->writeContentsToTimestampedFile("Screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE) {
        mShutDown = true;
    }
    mCameraMan->injectKeyDown(arg);
    return true;
}

bool TanksApplication::keyReleased(const OIS::KeyEvent &arg) {
    mCameraMan->injectKeyUp(arg);
    return true;
}

bool TanksApplication::mouseMoved(const OIS::MouseEvent &arg) {
    if (mTrayMgr->injectMouseMove(arg)) return true;
    mCameraMan->injectMouseMove(arg);
    return true;
}

bool TanksApplication::mousePressed(const OIS::MouseEvent &arg,
                                    OIS::MouseButtonID id) {
    if (mTrayMgr->injectMouseDown(arg, id)) return true;
    mCameraMan->injectMouseDown(arg, id);
    return true;
}

bool TanksApplication::mouseReleased(const OIS::MouseEvent &arg,
                                     OIS::MouseButtonID id) {
    if (mTrayMgr->injectMouseUp(arg, id)) return true;
    mCameraMan->injectMouseUp(arg, id);
    return true;
}

void TanksApplication::windowResized(Ogre::RenderWindow * rw) {
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

void TanksApplication::windowClosed(Ogre::RenderWindow * rw) {
    if (rw == mWindow) {
        if (mInputManager) {
            mInputManager->destroyInputObject(mMouse);
            mInputManager->destroyInputObject(mKeyboard);
            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN

#include "windows.h"

#endif
#ifdef __cplusplus
extern "C" {
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI
WinMain( HINSTANCE
hInst , HINSTANCE , LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
{
TanksApplication app;
try {
app . go();
} catch(
Ogre::Exception &e
) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
MessageBox( NULL, e.

getFullDescription()

.

c_str(),

"An exception has occured!",
MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " << e.getFullDescription().c_str()
                << std::endl;
#endif
}
return 0;
}
#ifdef __cplusplus
}
#endif