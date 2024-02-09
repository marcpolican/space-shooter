#include "pch.h"
#include "gameobjects.h"
#include "gameManager.h"

using namespace PewPew;

EnemyShip::EnemyShip() :
    mMove(0),
    mParticle(0),
    mIsBoss(0)
{
}
//--------------------------------------------------------------------
EnemyShip::~EnemyShip()
{
    int i = 0;
    i++;
}
//--------------------------------------------------------------------
void EnemyShip::setMovement(IMovement* movement)
{
    assert(movement);

    if (mMove)
        delete mMove;

    mMove = movement;
}
//--------------------------------------------------------------------
bool EnemyShip::update(double timeElapsed)
{
    if (mMove) mMove->update(timeElapsed);
    if (!Ship::update(timeElapsed)) return false;

    Vector3 pos = mNode->getPosition();

    if (pos.x < -300) mIsActive = false;
    if (pos.x >  300) mIsActive = false;
    if (pos.z < -500) mIsActive = false;
    if (pos.z >  500) mIsActive = false;

    if (mWeapon.isReadyToFire())
        mWeapon.fire();

    return true;
}
//--------------------------------------------------------------------
int EnemyShip::getType() const
{
    return OBJ_ENEMY_SHIP;
}
//--------------------------------------------------------------------
void EnemyShip::doDeath()
{
    ParticleEffect* part1 = mMgr->createParticle("pewpew\\explosion2");
    ParticleEffect* part2 = mMgr->createParticle("pewpew\\explosion3");
    ParticleEffect* part3 = mMgr->createParticle("pewpew\\explosion4");

    part1->setDuration(0.6);
    part1->getNode()->setPosition (mNode->getWorldPosition());

    part2->setDuration(0.6);
    part2->getNode()->setPosition (mNode->getWorldPosition());

    part3->setDuration(0.6);
    part3->getNode()->setPosition (mNode->getWorldPosition());

    if (mIsBoss)
    {
        Vector3 offset (20,0,50);
        //add 3 more explosions
        for (int i=0; i<3; i++)
        {
            switch (i)
            {
            case 0: offset.x = 20; offset.z = 50; break;
            case 1: offset.x = -20; offset.z = 0; break;
            case 2: offset.x = 0; offset.z = -80; break;
            }
            part1 = mMgr->createParticle("pewpew\\explosion2");
            part1->setDuration(0.6);
            part1->getNode()->setPosition (mNode->getWorldPosition() + offset);

            part2 = mMgr->createParticle("pewpew\\explosion3");
            part2->setDuration(0.6);
            part2->getNode()->setPosition (mNode->getWorldPosition() + offset);

            part3 = mMgr->createParticle("pewpew\\explosion4");
            part3->setDuration(0.6);
            part3->getNode()->setPosition (mNode->getWorldPosition() + offset);
        }
    }
    mIsActive = false;
}