#include "pch.h"
#include "utils.h"
#include "gameobjects.h"

using namespace PewPew;

MoveStraight::MoveStraight(SceneNode* node, const Vector3& velocity) :
    mNode (node),
    mVelocity(velocity)
{
    assert (mNode);
}
//--------------------------------------------------------------------
bool MoveStraight::update (double timeElapsed)
{
    mNode->setPosition(mNode->getPosition() + (mVelocity * timeElapsed));

    return true;
}
//--------------------------------------------------------------------
MoveStraightSpin::MoveStraightSpin(SceneNode* node, const Vector3& velocity, const Radian& spin, const Vector3& axis) :
    mNode (node),
    mVelocity (velocity),
    mAxis (axis),
    mSpin (spin)
{
}
//--------------------------------------------------------------------
bool MoveStraightSpin::update (double timeElapsed)
{
    mNode->translate(mVelocity * timeElapsed, Node::TS_WORLD);
    //mNode-setPosition(mNode->getPosition() + (mVelocity * timeElapsed));
    mNode->setOrientation (mNode->getOrientation() * Quaternion(mSpin * timeElapsed, mAxis));

    return true;
}
//--------------------------------------------------------------------
MoveToDest::MoveToDest(SceneNode* node, const Vector3& dest, double speed, bool isTurning) :
    mNode(node),
    mDest(dest),
    mIsTurning(isTurning),
    mTime(0.0),
    mDuration(0.0)
{
    mStart = mNode->getPosition();
    mDuration = mDest.distance(node->getPosition()) / speed;
}
//--------------------------------------------------------------------
bool MoveToDest::update (double timeElapsed)
{
    if (mTime > mDuration) return true;

    mTime += timeElapsed;
    double t = mTime / mDuration;
    if (t > 1.0) t = 1.0;
    
    Vector3 newPos = lerpV3(t, mStart, mDest);
    mNode->setPosition(newPos);
    return true;
}