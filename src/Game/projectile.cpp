#include "pch.h"
#include "gamemanager.h"
#include "gameobjects.h"

using namespace PewPew;

Projectile::Projectile() :
    mMove(0),
    mMgr(0),
    mDamage(50),
    mFromPlayer(true)
{}
//--------------------------------------------------------------------
Projectile::~Projectile() 
{
    assert (mMove);
    delete mMove;
}
//--------------------------------------------------------------------
bool Projectile::init (GameManager* mgr, SceneNode* node, const Params& p)
{
    assert (node);
    assert (mgr);
    mMgr  = mgr;
    mMove = new MoveStraight(node, p.velocity);
    
    Vector3 unit = p.velocity.normalisedCopy();
    double angle = asin (unit.x);
    node->setOrientation (Quaternion(Radian(angle), Vector3::UNIT_Y));
    mDamage = p.damage;
    mFromPlayer = p.fromPlayer;

    return true;
}
//--------------------------------------------------------------------
bool Projectile::update (double timeElapsed)
{
    if (!mIsActive) return true;

    Vector3 pos = mMove->getNode()->getPosition();
    
    if (pos.z < -300) mIsActive = false;
    if (pos.z >  500) mIsActive = false;

    return mMove->update(timeElapsed);
}
//--------------------------------------------------------------------
SceneNode* Projectile::getNode() const
{
    assert (mMove);
    return mMove->getNode();
}
//--------------------------------------------------------------------
int Projectile::getType() const
{
    return OBJ_PROJECTILE;
}
//--------------------------------------------------------------------
void Projectile::kill()
{
    mIsActive = false;
}