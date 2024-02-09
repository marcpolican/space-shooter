#include "pch.h"
#include "gameobjects.h"
#include "gamemanager.h"

using namespace PewPew;

//--------------------------------------------------------------------
Asteroid::Asteroid() :
    mHP(0),
    mNode(0),
    mMgr(0),
    mMove(0),
    mEntity(0)
{
}
//--------------------------------------------------------------------
Asteroid::~Asteroid()
{
}
//--------------------------------------------------------------------
bool Asteroid::init (SceneNode* node, GameManager* mgr)
{
    assert(node);
    assert(mgr);

    mNode = node;
    mMgr = mgr;

    return true;
}
//--------------------------------------------------------------------
bool Asteroid::update (double timeElapsed)
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
void Asteroid::setMovement(IMovement* movement)
{
    if (mMove)
    {
        delete mMove;
    }

    mMove = movement;
}
//--------------------------------------------------------------------
SceneNode* Asteroid::getNode() const
{
    return mNode;
}
//--------------------------------------------------------------------
void Asteroid::setHP(long hp)
{
    mHP = hp;
}
//--------------------------------------------------------------------
long Asteroid::getHP() const
{
    return mHP;
}
//--------------------------------------------------------------------
void Asteroid::damage(long damage)
{
    mHP -= damage;
}
//--------------------------------------------------------------------
void Asteroid::kill()
{
    ParticleEffect* part = mMgr->createParticle("pewpew\\explosion1");
    part->setDuration(0.6);
    part->getNode()->setPosition (mNode->getWorldPosition());

    mIsActive = false;
}