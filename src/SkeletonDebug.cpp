#include "SkeletonDebug.h"

using namespace Ogre;

SkeletonDebug::SkeletonDebug(Ogre::Entity* entity, Ogre::SceneManager *man, Ogre::Camera *cam, float boneSize)
{
    mEntity = entity;
    mSceneMan = man;
    mCamera = cam;

    mScaleAxes = 1.0;

    mBoneSize = boneSize;

    createAxesMaterial();
    createBoneMaterial();
    createAxesMesh();
    createBoneMesh();

    mShowAxes = true;
    mShowBones = true;
    mShowNames = true;

    int numBones = mEntity->getSkeleton()->getNumBones();

    std::cerr << "Bones found: " << numBones << std::endl;

    for(unsigned short int iBone = 0; iBone < numBones; ++iBone)
    {
        Ogre::Bone* pBone = mEntity->getSkeleton()->getBone(iBone);
        if ( !pBone )
        {
            assert(false);
            continue;
        }

        Ogre::Entity *ent;
        Ogre::TagPoint *tp;

        // Absolutely HAVE to create bone representations first. Otherwise we would get the wrong child count
        // because an attached object counts as a child
        // Would be nice to have a function that only gets the children that are bones...
        unsigned short numChildren = pBone->numChildren();
        if(numChildren == 0)
        {
            // There are no children, but we should still represent the bone
            // Creates a bone of length 1 for leaf bones (bones without children)
            ent = mSceneMan->createEntity("SkeletonDebug/BoneMesh");
            tp = mEntity->attachObjectToBone(pBone->getName(), (Ogre::MovableObject*)ent);
            mBoneEntities.push_back(ent);
        }
        else
        {
            for(int i = 0; i < numChildren; ++i)
            {
                Vector3 v = pBone->getChild(i)->getPosition();
                // If the length is zero, no point in creating the bone representation
                float length = v.length();
                if(length < 0.00001f)
                    continue;

                ent = mSceneMan->createEntity("SkeletonDebug/BoneMesh");
                tp = mEntity->attachObjectToBone(pBone->getName(), (Ogre::MovableObject*)ent);
                mBoneEntities.push_back(ent);

                tp->setScale(length, length, length);
            }
        }

        ent = mSceneMan->createEntity("SkeletonDebug/AxesMesh");
        tp = mEntity->attachObjectToBone(pBone->getName(), (Ogre::MovableObject*)ent);
        // Make sure we don't wind up with tiny/giant axes and that one axis doesnt get squashed
        tp->setScale((mScaleAxes/mEntity->getParentSceneNode()->getScale().x), (mScaleAxes/mEntity->getParentSceneNode()->getScale().y), (mScaleAxes/mEntity->getParentSceneNode()->getScale().z));
        mAxisEntities.push_back(ent);
        // std::cerr << "Found bone:" << name << " at " << pBone->getPostition() << std::endl;

        Ogre::String name = mEntity->getName() + "SkeletonDebug/BoneText/Bone_";
        name += Ogre::StringConverter::toString(iBone);
        ObjectTextDisplay *overlay = new ObjectTextDisplay(name, pBone, mCamera, mEntity);
        overlay->enable(true);
        overlay->setText(pBone->getName());
        mTextOverlays.push_back(overlay);
    }

    showAxes(false);
    showBones(false);
    showNames(false);
}

SkeletonDebug::~SkeletonDebug()
{
}

void SkeletonDebug::update()
{
    std::vector<ObjectTextDisplay*>::iterator it;
    for(it = mTextOverlays.begin(); it < mTextOverlays.end(); it++)
    {
        ((ObjectTextDisplay*)*it)->update();
    }
}

void SkeletonDebug::showAxes(bool show)
{
    // Don't change anything if we are already in the proper state
    if(mShowAxes == show)
        return;

    mShowAxes = show;

    std::vector<Ogre::Entity*>::iterator it;
    for(it = mAxisEntities.begin(); it < mAxisEntities.end(); ++it)
    {
        ((Ogre::Entity*)*it)->setVisible(show);
    }
}

void SkeletonDebug::showBones(bool show)
{
    // Don't change anything if we are already in the proper state
    if(mShowBones == show)
        return;

    mShowBones = show;

    std::vector<Ogre::Entity*>::iterator it;
    for(it = mBoneEntities.begin(); it < mBoneEntities.end(); ++it)
    {
        ((Ogre::Entity*)*it)->setVisible(show);
    }

}

void SkeletonDebug::showNames(bool show)
{
    // Don't change anything if we are already in the proper state
    if(mShowNames == show)
        return;

    mShowNames = show;

    std::vector<ObjectTextDisplay*>::iterator it;
    for(it = mTextOverlays.begin(); it < mTextOverlays.end(); it++)
    {
        ((ObjectTextDisplay*)*it)->enable(show);
    }
}

void SkeletonDebug::createAxesMaterial()
{
    Ogre::String matName = "SkeletonDebug/AxesMat";

    mAxisMatPtr = MaterialManager::getSingleton().getByName(matName);
    if (mAxisMatPtr.isNull())
    {
        mAxisMatPtr = MaterialManager::getSingleton().create(matName, ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);

        // First pass for axes that are partially within the model (shows transparency)
        Pass* p = mAxisMatPtr->getTechnique(0)->getPass(0);
        p->setLightingEnabled(false);
        p->setPolygonModeOverrideable(false);
        p->setVertexColourTracking(TVC_AMBIENT);
        p->setSceneBlending(SBT_TRANSPARENT_ALPHA);
        p->setCullingMode(CULL_NONE);
        p->setDepthWriteEnabled(false);
        p->setDepthCheckEnabled(false);

        // Second pass for the portion of the axis that is outside the model (solid colour)
        Pass* p2 = mAxisMatPtr->getTechnique(0)->createPass();
        p2->setLightingEnabled(false);
        p2->setPolygonModeOverrideable(false);
        p2->setVertexColourTracking(TVC_AMBIENT);
        p2->setCullingMode(CULL_NONE);
        p2->setDepthWriteEnabled(false);
    }
}

void SkeletonDebug::createBoneMaterial()
{
    Ogre::String matName = "SkeletonDebug/BoneMat";

    mBoneMatPtr = MaterialManager::getSingleton().getByName(matName);
    if (mBoneMatPtr.isNull())
    {
        mBoneMatPtr = MaterialManager::getSingleton().create(matName, ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);

        Pass* p = mBoneMatPtr->getTechnique(0)->getPass(0);
        p->setLightingEnabled(false);
        p->setPolygonModeOverrideable(false);
        p->setVertexColourTracking(TVC_AMBIENT);
        p->setSceneBlending(SBT_TRANSPARENT_ALPHA);
        p->setCullingMode(CULL_ANTICLOCKWISE);
        p->setDepthWriteEnabled(false);
        p->setDepthCheckEnabled(false);
    }
}

void SkeletonDebug::createBoneMesh()
{
    String meshName = "SkeletonDebug/BoneMesh";
    mBoneMeshPtr = MeshManager::getSingleton().getByName(meshName);
    if(mBoneMeshPtr.isNull())
    {
        ManualObject mo("tmp");
        mo.begin(mBoneMatPtr->getName());

        Vector3 basepos[6] =
        {
            Vector3(0,0,0),
            Vector3(mBoneSize, mBoneSize*2, mBoneSize),
            Vector3(-mBoneSize, mBoneSize*2, mBoneSize),
            Vector3(-mBoneSize, mBoneSize*2, -mBoneSize),
            Vector3(mBoneSize, mBoneSize*2, -mBoneSize),
            Vector3(0, 1, 0),
        };

        // Two colours so that we can distinguish the sides of the bones (we don't use any lighting on the material)
        ColourValue col = ColourValue(0.5, 0.5, 0.5, 1);
        ColourValue col1 = ColourValue(0.6, 0.6, 0.6, 1);

        mo.position(basepos[0]);
        mo.colour(col);
        mo.position(basepos[2]);
        mo.colour(col);
        mo.position(basepos[1]);
        mo.colour(col);

        mo.position(basepos[0]);
        mo.colour(col1);
        mo.position(basepos[3]);
        mo.colour(col1);
        mo.position(basepos[2]);
        mo.colour(col1);

        mo.position(basepos[0]);
        mo.colour(col);
        mo.position(basepos[4]);
        mo.colour(col);
        mo.position(basepos[3]);
        mo.colour(col);

        mo.position(basepos[0]);
        mo.colour(col1);
        mo.position(basepos[1]);
        mo.colour(col1);
        mo.position(basepos[4]);
        mo.colour(col1);

        mo.position(basepos[1]);
        mo.colour(col1);
        mo.position(basepos[2]);
        mo.colour(col1);
        mo.position(basepos[5]);
        mo.colour(col1);

        mo.position(basepos[2]);
        mo.colour(col);
        mo.position(basepos[3]);
        mo.colour(col);
        mo.position(basepos[5]);
        mo.colour(col);

        mo.position(basepos[3]);
        mo.colour(col1);
        mo.position(basepos[4]);
        mo.colour(col1);
        mo.position(basepos[5]);
        mo.colour(col1);

        mo.position(basepos[4]);
        mo.colour(col);
        mo.position(basepos[1]);
        mo.colour(col);
        mo.position(basepos[5]);
        mo.colour(col);

        // indices
        mo.triangle(0, 1, 2);
        mo.triangle(3, 4, 5);
        mo.triangle(6, 7, 8);
        mo.triangle(9, 10, 11);
        mo.triangle(12, 13, 14);
        mo.triangle(15, 16, 17);
        mo.triangle(18, 19, 20);
        mo.triangle(21, 22, 23);

        mo.end();

        mBoneMeshPtr = mo.convertToMesh(meshName, ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    }
}

void SkeletonDebug::createAxesMesh()
{
    String meshName = "SkeletonDebug/AxesMesh";
    mAxesMeshPtr = MeshManager::getSingleton().getByName(meshName);
    if (mAxesMeshPtr.isNull())
    {
        ManualObject mo("tmp");
        mo.begin(mAxisMatPtr->getName());
        /* 3 axes, each made up of 2 of these (base plane = XY)
        *   .------------|\
        *   '------------|/
        */
        mo.estimateVertexCount(7 * 2 * 3);
        mo.estimateIndexCount(3 * 2 * 3);
        Quaternion quat[6];
        ColourValue col[3];

        // x-axis
        quat[0] = Quaternion::IDENTITY;
        quat[1].FromAxes(Vector3::UNIT_X, Vector3::NEGATIVE_UNIT_Z, Vector3::UNIT_Y);
        col[0] = ColourValue::Red;
        col[0].a = 0.3;
        // y-axis
        quat[2].FromAxes(Vector3::UNIT_Y, Vector3::NEGATIVE_UNIT_X, Vector3::UNIT_Z);
        quat[3].FromAxes(Vector3::UNIT_Y, Vector3::UNIT_Z, Vector3::UNIT_X);
        col[1] = ColourValue::Green;
        col[1].a = 0.3;
        // z-axis
        quat[4].FromAxes(Vector3::UNIT_Z, Vector3::UNIT_Y, Vector3::NEGATIVE_UNIT_X);
        quat[5].FromAxes(Vector3::UNIT_Z, Vector3::UNIT_X, Vector3::UNIT_Y);
        col[2] = ColourValue::Blue;
        col[2].a = 0.3;

        Vector3 basepos[7] =
        {
            // stalk
            Vector3(0, 0.05, 0),
            Vector3(0, -0.05, 0),
            Vector3(0.7, -0.05, 0),
            Vector3(0.7, 0.05, 0),
            // head
            Vector3(0.7, -0.15, 0),
            Vector3(1, 0, 0),
            Vector3(0.7, 0.15, 0)
        };


        // vertices
        // 6 arrows
        for (size_t i = 0; i < 6; ++i)
        {
            // 7 points
            for (size_t p = 0; p < 7; ++p)
            {
                Vector3 pos = quat[i] * basepos[p];
                mo.position(pos);
                mo.colour(col[i / 2]);
            }
        }

        // indices
        // 6 arrows
        for (size_t i = 0; i < 6; ++i)
        {
            size_t base = i * 7;
            mo.triangle(base + 0, base + 1, base + 2);
            mo.triangle(base + 0, base + 2, base + 3);
            mo.triangle(base + 4, base + 5, base + 6);
        }

        mo.end();

        mAxesMeshPtr = mo.convertToMesh(meshName, ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    }
}