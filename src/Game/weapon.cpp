#include "pch.h"
#include "gamemanager.h"
#include "gameobjects.h"

using namespace PewPew;

Weapon::Weapon() :
    mNode(0),
    mFireDelay(0.1),
    mNumAmmo(-1),
    mFireTime(0.0),
    mMgr(0)
{
}
//--------------------------------------------------------------------
Weapon::~Weapon()
{
}
//--------------------------------------------------------------------
bool Weapon::init(SceneNode* node, GameManager* mgr)
{
    assert (mgr);
    assert (node);

    mNode = node;
    mMgr = mgr;
    return true;
}
//--------------------------------------------------------------------
bool Weapon::setParams(Params params)
{
    mFireDelay = params.fireDelay;
    mNumAmmo   = params.numAmmo;

    mProjParams.clear();
    mProjParams = params.projectiles;
    return true;
}
//--------------------------------------------------------------------
void Weapon::fire()
{
    assert (mMgr);
    assert (mNode);

    if (isReadyToFire())
    {
        //spawn all projectiles
        for (VProjParams::iterator iter = mProjParams.begin();
            iter != mProjParams.end();
            iter++)
        {
            Projectile* proj = mMgr->createProjectile( (*iter) );
            proj->getNode()->setPosition ((*iter).origin + mNode->getWorldPosition());
        }
       
        mFireTime = 0;
    }
}
//--------------------------------------------------------------------
bool Weapon::update(double timeElapsed)
{
    if (mFireTime < mFireDelay)
        mFireTime += timeElapsed;

    return true;
}
//--------------------------------------------------------------------
bool Weapon::isReadyToFire()
{
    return mFireTime >= mFireDelay;
}
//--------------------------------------------------------------------
int  Weapon::getType() const
{
    return OBJ_WEAPON;
}
//--------------------------------------------------------------------
SceneNode* Weapon::getNode() const
{
    return mNode;
}