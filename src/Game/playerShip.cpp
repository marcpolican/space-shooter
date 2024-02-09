#include "pch.h"
#include "gameobjects.h"
#include "gamemanager.h"
#include "audio.h"

using namespace PewPew;


PlayerShip::PlayerShip() :
    mMouse(0),
    mKeyboard(0),
    mMoveTotalX(0),
    mAudio(0),
    mTiltLimit(Math::PI * 0.25),
    mTiltSpeed(10.0),
    mTiltReturnSpeed(5.0),
    mDamagedTimer(0),
    mDamagedTimerInitial(1.0),
    mLives(3),
    mRespawnWait(0.0)
{
    mHP = 1000;
}
//--------------------------------------------------------------------
PlayerShip::~PlayerShip()
{
}
//--------------------------------------------------------------------
bool PlayerShip::initInput(OIS::Mouse* mouse, OIS::Keyboard* keyboard)
{
    assert (mouse);
    assert (keyboard);

    mMouse    = mouse;
    mKeyboard = keyboard;
    mSpeed = Vector3(1,0,1);
    return true;
}
//--------------------------------------------------------------------
void PlayerShip::initAudio(Audio* audio)
{
    mAudio = audio;
}
//--------------------------------------------------------------------
void PlayerShip::initBounds(const AxisAlignedBox& box)
{
    mBounds = box;
}
//--------------------------------------------------------------------
void PlayerShip::setSpeed(const Vector3& moveSpeed)
{
    mSpeed = moveSpeed;
}
//--------------------------------------------------------------------
bool PlayerShip::update(double timeElapsed)
{
    if (!Ship::update(timeElapsed)) return false;
    if (!mMouse) return true;

    const double limit        = Math::PI * 0.25;
    const OIS::MouseState &ms = mMouse->getMouseState();
    const Quaternion& orient  = mNode->getOrientation();
    static double blink = 0.3;
    static bool isOn = true;

    if (mDamagedTimer > 0.0)
    {
        mDamagedTimer -= timeElapsed;
        blink -= timeElapsed;
        if (blink <= 0)
        {
            mNode->setVisible( !isOn );
            isOn = !isOn;
            blink -= 0.3;
        }

        if (mDamagedTimer <= 0.0)
            mNode->setVisible(true);
    }

    if (mRespawnWait > 0.0)
    {
        mRespawnWait -= timeElapsed;
        if (mLives > 0)
        {
            if (mRespawnWait <= 0)
            {
                //respawn
                mHP = mInitHP;
                mNode->setPosition(0,0,mPosZ);
                mNode->setVisible(true);
                mDamagedTimer = 2.0;
            }
        }
        else
        {
            mIsActive = false;
        }
        return true;
    }

    if (ms.X.rel != 0.0)
        mPosChange.x = -ms.X.rel * mSpeed.x;
    else
        mPosChange.x = 0.0;

    mPosChange.y = 0;
    mPosChange.z = 0;

    Vector3 tempPos = mNode->getPosition() + mPosChange;

    //check if player will go outside the playing field
    Matrix4 mat;
    mat.makeTransform(tempPos, mNode->getScale(), Quaternion::IDENTITY);
    AxisAlignedBox box = mNode->getAttachedObject(0)->getBoundingBox();
    box.transform(mat);

    Vector3 min1(box.getMinimum());
    Vector3 max1(box.getMaximum());
    Vector3 min2(mBounds.getMinimum());
    Vector3 max2(mBounds.getMaximum());
    Vector3 diff(0,0,0);

    if (min1.x < min2.x) diff.x = min2.x - min1.x;
    if (min1.z < min2.z) diff.z = min2.z - min1.z;
    if (max1.x > max2.x) diff.x = max2.x - max1.x;
    if (max1.z > max2.z) diff.z = max2.z - max1.z;

    tempPos = tempPos + diff;
    mPosChange += diff;
    mNode->setPosition(tempPos);

    //camera follows player movement
    SceneNode* camNode = mMgr->getSceneMgr()->getSceneNode("MainCamera");
    Vector3 camPos = camNode->getPosition();
    camPos.x = tempPos.x;
    camNode->setPosition (camPos);

    //do the tilting thing when the player moves left and right
    if (mPosChange.x != 0)
    {
        double dir = (mPosChange.x < 0) ? -1 : 1;

        Quaternion rotateMore (Radian(mTiltSpeed * timeElapsed * -dir) , Vector3::UNIT_Z);
        mNode->setOrientation (orient * rotateMore);

        const Quaternion& newOrient = mNode->getOrientation();
        double roll = newOrient.getRoll().valueRadians();

        if (roll < -mTiltLimit )
            mNode->setOrientation(Quaternion( Radian(-mTiltLimit), Vector3::UNIT_Z ));

        else if (roll > mTiltLimit)
            mNode->setOrientation(Quaternion( Radian(mTiltLimit),  Vector3::UNIT_Z ));
    }
    else
    {
        //gradually return to neutral position
        double roll = orient.getRoll().valueRadians();

        if (-0.05 <= roll && roll <= 0.05)
            mNode->setOrientation(Quaternion::IDENTITY);

        else if (roll < 0)
            mNode->setOrientation (orient * Quaternion( Radian(mTiltReturnSpeed * timeElapsed) , Vector3::UNIT_Z ));

        else
            mNode->setOrientation (orient * Quaternion( Radian(-mTiltReturnSpeed * timeElapsed) , Vector3::UNIT_Z ));

        mMoveTotalX = 0.0;
    }


    if (ms.buttonDown(OIS::MB_Left))
    {
        if (mWeapon.isReadyToFire())
        {
            if (mAudio) mAudio->play("zap");
            mWeapon.fire();
        }
    }

    return true;
}
//--------------------------------------------------------------------
void PlayerShip::setTiltValues (double tiltLimit, double tiltSpeed, double tiltReturnSpeed)
{
    mTiltLimit       = tiltLimit;
    mTiltSpeed       = tiltSpeed;
    mTiltReturnSpeed = tiltReturnSpeed;
}
//--------------------------------------------------------------------
int PlayerShip::getType() const
{
    return OBJ_PLAYER_SHIP;
}
//--------------------------------------------------------------------
bool PlayerShip::isCollidable() const
{
    return true;
}
//--------------------------------------------------------------------
bool PlayerShip::canBeHit() const
{
    return (mDamagedTimer <= 0.0 && mRespawnWait <= 0.0);
}
//--------------------------------------------------------------------
void PlayerShip::damage(long dmg)
{
    ParticleEffect* part = mMgr->createParticle("pewpew\\playerHit");
    part->setDuration(0.2);
    part->getNode()->setPosition (mNode->getWorldPosition());

    Ship::damage(dmg);

    if (mHP <= 0)
    {
        mLives--;
        mNode->setVisible(false);
        mRespawnWait = 1.0;
        mDamagedTimer = 0.0;

        //revert to first weapon
        mWeapon.setParams(mFirstWeapon);
    }
    else
    {
        mDamagedTimer = mDamagedTimerInitial;
    }
}
//--------------------------------------------------------------------
void PlayerShip::doDeath()
{
    ParticleEffect* part = mMgr->createParticle("pewpew\\explosion1");
    part->setDuration(0.2);
    part->getNode()->setPosition (mNode->getWorldPosition());
}
//--------------------------------------------------------------------
void PlayerShip::setFirstWeapon(const Weapon::Params& w)
{
    mFirstWeapon = w;
}