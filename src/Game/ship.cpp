#include "pch.h"
#include "gameobjects.h"
#include "gamemanager.h"

using namespace PewPew;

Ship::Ship() :
    mHP (500),
    mShields (0),
    mNode (0),
    mMgr(0)
{
}
//--------------------------------------------------------------------
Ship::~Ship()
{
}
//--------------------------------------------------------------------
bool Ship::init(SceneNode* node, GameManager* mgr)
{
    assert (node);
    assert (mgr);

    mNode = node;
    mMgr = mgr;
    SceneNode* weaponNode = mgr->acquireSceneNode();
    mNode->addChild(weaponNode);
    mWeapon.init(weaponNode, mgr);
    
    return true;
}
//--------------------------------------------------------------------
bool Ship::update(double timeElapsed)
{
    mWeapon.update(timeElapsed);
    return true;
}
//--------------------------------------------------------------------
bool Ship::isHit(const AxisAlignedBox& bbox, const Matrix4& mat) const
{
    return false;
}
//--------------------------------------------------------------------
void Ship::doHit(long damage)
{
    //do hit animation, etc.
}
//--------------------------------------------------------------------
void Ship::doDeath()
{
    //do death animation, etc
}
//--------------------------------------------------------------------
bool Ship::isDead() const
{
    return (mHP <= 0);
}
//--------------------------------------------------------------------
void Ship::setHP(long hp)
{
    mHP = hp;
}
//--------------------------------------------------------------------
void Ship::addHP(long hp)
{
    mHP += hp;
}
//--------------------------------------------------------------------
void Ship::subHP(long hp)
{
    mHP -= hp;
}
//--------------------------------------------------------------------
long Ship::getHP() const
{
    return mHP;
}
//--------------------------------------------------------------------
void Ship::setShields(long shields)
{
    mShields = shields;
}
//--------------------------------------------------------------------
void Ship::addShields(long shields)
{
    mShields += shields;
}
//--------------------------------------------------------------------
void Ship::subShields(long shields)
{
    mShields -= shields;
}
//--------------------------------------------------------------------
long Ship::getShields() const
{
    return mShields;
}
//--------------------------------------------------------------------
Weapon* Ship::getWeapon()
{
    return &mWeapon;
}
//--------------------------------------------------------------------
SceneNode* Ship::getNode() const
{
    return mNode;
}
//--------------------------------------------------------------------
int Ship::getType() const
{
    return OBJ_SHIP;
}
//--------------------------------------------------------------------
void Ship::damage(long dmg)
{
    if (mShields > 0)
    {
        mShields -= dmg;
        if (mShields < 0)
            mHP += mShields;
    }
    else
    {
        mHP -= dmg;
    }

    if (mHP < 0) mHP = 0;
    if (mShields < 0) mShields = 0;
}
//--------------------------------------------------------------------
void Ship::kill()
{
    doDeath();
}