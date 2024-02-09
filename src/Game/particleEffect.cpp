#include "pch.h"
#include "gamemanager.h"
#include "gameobjects.h"

using namespace PewPew;


ParticleEffect::ParticleEffect() :
    mDuration(1.0),
    mTimeElapsed(0),
    mNode(0),
    mMgr(0),
    mParticleSystem(0)
{
}
//--------------------------------------------------------------------
bool ParticleEffect::init (SceneNode* node, ParticleSystem* part, GameManager* mgr)
{
    mMgr = mgr;
    mNode = node;
    mParticleSystem = part;
    return true;
}
//--------------------------------------------------------------------
bool ParticleEffect::update (double timeElapsed)
{
    if (mDuration < 0) return true;

    mTimeElapsed += timeElapsed;

    if (mTimeElapsed > mDuration)
    {
        mIsActive = false;
    }
    return true;
}
//--------------------------------------------------------------------
void ParticleEffect::reset()
{
    mIsActive=true;
    mTimeElapsed = 0;
    if (mNode)
    {
        mNode->setPosition(Vector3::ZERO);
        mNode->setScale(Vector3::UNIT_SCALE);
        mNode->setOrientation(Quaternion::IDENTITY);
    }
}