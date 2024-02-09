#include "pch.h"
#include "gamemanager.h"
#include "stages.h"
#include "GlobalVars.h"
#include "utils.h"

using namespace PewPew;



bool PlayStage::initData()
{
    mBounds.setExtents(GLOBALS->getDouble("globals/dScreenBoundsX1"),
                       -500,
                       GLOBALS->getDouble("globals/dScreenBoundsZ1"),
                       GLOBALS->getDouble("globals/dScreenBoundsX2"),
                       500,
                       GLOBALS->getDouble("globals/dScreenBoundsZ2"));

    //initialize player ship
    mPlayer = mGameManager->createPlayerShip(GLOBALS->getString("player/szModel"));
    mPlayer->initInput  (mMouse, mKeyboard);
    mPlayer->initAudio  (mAudio);
    mPlayer->initBounds (mBounds);
    mPlayer->setLives   (GLOBALS->getInt("player/nLives"));
    mPlayer->setInitHP  (GLOBALS->getInt("player/nHP"));
    mPlayer->setHP      (GLOBALS->getInt("player/nHP"));
    mPlayer->setShields (GLOBALS->getInt("player/nShield"));
    mPlayer->setPosZ    (GLOBALS->getDouble("player/dPosZ"));
    mPlayer->setSpeed   (Vector3(GLOBALS->getDouble("player/dSpeedX"), 0, GLOBALS->getDouble("player/dSpeedZ")));
    mPlayer->setTiltValues(
        Math::DegreesToRadians(GLOBALS->getDouble("player/dTiltLimit")),
        GLOBALS->getDouble("player/dTiltSpd"),
        GLOBALS->getDouble("player/dTiltRetSpd"));

    mPlayer->getNode()->setPosition(0,0, GLOBALS->getDouble("player/dPosZ"));
    
    mSpawnPtZ = GLOBALS->getDouble("globals/dSpawnZ");
    mScore = 0;
    
    Weapon::Params paramsWeapon;
    Weapon* weapon = mPlayer->getWeapon();

    getWeaponParams(GLOBALS->getString("player/szWeapon"), paramsWeapon);
    weapon->setParams(paramsWeapon);

    mPlayer->setFirstWeapon(paramsWeapon);

    return true;
}
//--------------------------------------------------------------------
bool PlayStage::initScene()
{
    mGameManager->init (mRoot,mSceneMgr);

    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
    mSceneMgr->setShadowTextureSize(512);
    mSceneMgr->setShadowColour(ColourValue(0.6, 0.6, 0.6));
    mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

    mCamera = mSceneMgr->getCamera("MainCamera");
    mCamera->setPosition(Vector3(0,0,0));
    mCamera->lookAt(Vector3(0,-1,0));
    mCamera->setNearClipDistance(5);

    SceneNode* nodeCamera = mSceneMgr->getRootSceneNode()->createChildSceneNode("MainCamera");
    nodeCamera->setPosition(0,200,-350);
    //nodeCamera->setPosition(0,500,-0.001);
    nodeCamera->attachObject(mCamera);

    // Give it a little ambience with lights
    Light* l;
    l = mSceneMgr->createLight("Main");
    l->setType(Light::LT_DIRECTIONAL);
    l->setDirection(-1, -1, 0);
    l->setDiffuseColour(1.0, 1.0, 0.5);

    l = mSceneMgr->createLight("Fill");
    l->setType(Light::LT_DIRECTIONAL);
    l->setDirection(1, -1, 1);
    l->setDiffuseColour(0.5, 0.5, 0.1);

    mStarfieldParticle = mGameManager->createParticle("pewpew\\starfield");
    mStarfieldParticle->setDuration(-1);

    mSceneMgr->setSkyDome(true, "pewpew\\starfield",
        10,20,4000,true,Quaternion(Radian(Math::PI/2.0), Vector3::UNIT_X));

    return true;
}
//--------------------------------------------------------------------
void PlayStage::getWeaponParams(const string& weaponName, Weapon::Params& p)
{
    Projectile::Params projParams;
    string projName;
    string temp;
    double val1, val2, val3;

    p.model     = GLOBALS->getString(weaponName + "/szModel");
    p.fireDelay = GLOBALS->getDouble(weaponName + "/dFireDelay");
    p.numAmmo   = GLOBALS->getInt(weaponName + "/nNumAmmo");

    int numProj = GLOBALS->getInt(weaponName + "/nNumProj");
    for (int i=0; i<numProj; i++)
    {
        projName = GLOBALS->getString(weaponName + "/szProj" + StringConverter::toString(i));

        projParams.model       = GLOBALS->getString(projName + "/szModel");
        projParams.damage      = GLOBALS->getInt   (projName + "/nDamage");

        temp = GLOBALS->getString(projName + "/szVelocity");
        if (parseDouble3( temp, val1, val2, val3 ))
        {
            projParams.velocity.x = val1;
            projParams.velocity.y = val2;
            projParams.velocity.z = val3;
        }
        else
        {
            projParams.velocity.x = 0;
            projParams.velocity.y = 0;
            projParams.velocity.z = 1.0;
        }

        temp = GLOBALS->getString(projName + "/szOrigin");
        if (parseDouble3( temp, val1, val2, val3 ))
        {
            projParams.origin.x = val1;
            projParams.origin.y = val2;
            projParams.origin.z = val3;
        }
        else
        {
            projParams.origin = Vector3::ZERO;
        }


        //check for defaults
        if (projParams.damage == 0) projParams.damage = 50;
            
        p.projectiles.push_back(projParams);
    }

    //check for defeault values
    if (p.numAmmo == 0) p.numAmmo = -1;
    if (p.fireDelay == 0) p.fireDelay = 0.1;
}