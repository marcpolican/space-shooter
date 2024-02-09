#include "pch.h"
#include "gameobjects.h"

using namespace PewPew;

PowerUp::PowerUp() :
    mNode(0),
    mMgr(0),
    mMove(0)
{
}
//--------------------------------------------------------------------
PowerUp::~PowerUp()
{
}
//--------------------------------------------------------------------
bool PowerUp::init (SceneNode* node, GameManager* mgr)
{
    assert (node);
    assert (mgr);

    mNode = node;
    mMgr = mgr;
    return true;
}
//--------------------------------------------------------------------
bool PowerUp::update (double timeElapsed)
{
    if (mMove) mMove->update(timeElapsed);

    Vector3 pos = mNode->getPosition();

    if (pos.x < -300) mIsActive = false;
    if (pos.x >  300) mIsActive = false;
    if (pos.z < -500) mIsActive = false;
    if (pos.z >  500) mIsActive = false;

    return true;
}
//--------------------------------------------------------------------
void PowerUp::setMovement(IMovement* movement)
{    
    if (mMove)
        delete mMove;

    mMove = movement;
}
//--------------------------------------------------------------------
void PowerUp::acquired()
{
    mIsActive = false;
}

