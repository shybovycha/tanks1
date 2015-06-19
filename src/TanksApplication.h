#ifndef __TANKS_APPLICATION_H__
#define __TANKS_APPLICATION_H__

#include <Ogre.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreTagPoint.h>
#include <OgreConfigFile.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <boost/format.hpp>
#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>
#include "SdkTrays.h"
#include "SdkCameraMan.h"
#include "SkeletonDebug.h"
#include "ScriptManager.h"

class TanksApplication : public Ogre::FrameListener, public Ogre::WindowEventListener,
    public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener {
public:
    TanksApplication(void);
    virtual ~TanksApplication(void);
    bool go(void);

protected:
    Ogre::Root *mRoot;
    Ogre::Camera* mCamera;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;
    Ogre::AnimationState* mAnimation;
    Ogre::OverlaySystem *mOverlaySystem;
    SkeletonDebug* mSkeletonDebugger;
    ScriptManager* mScriptMgr;

    //OGREBITES
    //====================================================
    OgreBites::SdkTrayManager* mTrayMgr;
    OgreBites::SdkCameraMan* mCameraMan;//Basic camera controller
    OgreBites::ParamsPanel* mDetailsPanel;//Sample details panel
    bool mCursorWasVisible;//Was cursor visible before dialog appeared
    bool mShutDown;

    //OIS
    //====================================================
    OIS::InputManager* mInputManager;
    OIS::Mouse* mMouse;
    OIS::Keyboard* mKeyboard;

    //FRAME LISTENER
    //====================================================
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    //KEY LISTENER
    //====================================================
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );

    //MOUSE LISTENER
    //====================================================
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    //WINDOW EVENT LISTENER
    //====================================================
    virtual void windowResized(Ogre::RenderWindow* rw);
    virtual void windowClosed(Ogre::RenderWindow* rw);
};
#endif