#include "pch.h"
#include "gamemanager.h"
#include "gameobjects.h"
#include "audio.h"

using namespace PewPew;

GameManager::GameManager() :
    mSceneMgr(0),
    mRoot(0),
    mUnnamedProjectileIndex(0),
    mUnnamedPlayerShipIndex(0),
    mUnnamedEnemyShipIndex(0),
    mUnnamedParticleIndex(0),
    mUnnamedAsteroidIndex(0),
    mUnnamedPowerUpIndex(0)
{}
//--------------------------------------------------------------------
GameManager::~GameManager()
{
    releaseAll();
}
//--------------------------------------------------------------------
void GameManager::releaseAll()
{
    deleteContents (mProjectiles);
    deleteContents (mPlayerShips);
    deleteContents (mEnemyShips);
    deleteContents (mParticles);
    
    LParticleEffect::iterator iter;
    for (iter = mFreeParticles.begin(); iter != mFreeParticles.end(); iter++)
    {
        delete (*iter);
    }
    mFreeParticles.clear();

    mObjects.clear();
}
//--------------------------------------------------------------------
//WOW! it actually works!
template <class T>
void GameManager::deleteContents (T& myMap)
{
    T::iterator iter;
    for (iter = myMap.begin(); iter != myMap.end(); iter++)
        delete (*iter).second;

    myMap.clear();
}
//--------------------------------------------------------------------
bool GameManager::init (Root* root, SceneManager* sceneMgr)
{
    assert (sceneMgr);
    assert (root);

    mRoot = root;
    mSceneMgr = sceneMgr;

    return true;
}
//--------------------------------------------------------------------
bool GameManager::update (double timeElapsed)
{
    LGameObjects::iterator iter;

    //remove inactive objects;
    for (iter = mObjects.begin(); iter != mObjects.end(); iter++)
    {
        if (!(*iter)->isActive())
        {
            GameObject* ptr = *iter;

            mObjects.remove(ptr);

            switch (ptr->getType())
            {
            case OBJ_PROJECTILE : destroyProjectile(ptr->getName()); break;
            case OBJ_PLAYER_SHIP: destroyPlayerShip(ptr->getName()); break;
            case OBJ_ENEMY_SHIP : destroyEnemyShip (ptr->getName()); break;
            case OBJ_PARTICLE_EFFECT: destroyParticle (ptr->getName()); break;
            case OBJ_ASTEROID   : destroyAsteroid  (ptr->getName()); break;
            case OBJ_POWERUP    : destroyPowerUp   (ptr->getName()); break;
            }

            if (mObjects.size() == 0)
                break;

            iter = mObjects.begin(); //TODO: NOT GOOD! :P
        }
    }

    for (iter = mObjects.begin(); iter != mObjects.end(); iter++)
    {
        if (!(*iter)->update(timeElapsed))
            return false;
    }

    return true;
}
//--------------------------------------------------------------------
void GameManager::addGameObject(GameObject* obj)
{
    mObjects.push_back (obj);
}
//--------------------------------------------------------------------
void GameManager::removeGameObject(GameObject* obj)
{
    mObjects.remove(obj);
}
//--------------------------------------------------------------------
SceneNode* GameManager::acquireSceneNode(bool isAttachToRootNode)
{
    assert (mSceneMgr);
    SceneNode* node = mSceneMgr->createSceneNode();

    if (isAttachToRootNode)
        mSceneMgr->getRootSceneNode()->addChild(node);

    return node;
}
//--------------------------------------------------------------------
void GameManager::releaseSceneNode(SceneNode* node)
{
    assert (mSceneMgr);
    assert (node);

    //remove all node children too! oh noesss!!!
    SceneNode::ObjectIterator iter = node->getAttachedObjectIterator();
    string type;

    while (iter.hasMoreElements())
    {
        MovableObject* obj = iter.getNext();
        type = obj->getMovableType();

        if (type == "Entity")
        {
            mSceneMgr->destroyEntity(static_cast <Entity*> (obj));
        }
        else if (type == "ParticleSystem")
        {
            //mSceneMgr->destroyParticleSystem(static_cast <ParticleSystem*> (obj));
        }
    }

    node->removeAndDestroyAllChildren();
    node->detachAllObjects();
    mSceneMgr->destroySceneNode(node->getName());
}
//--------------------------------------------------------------------
/// Returns a pointer to the list of GameOjbect pointers
/// @returns
///     The pointer to the LGameObjects list
GameManager::LGameObjects* GameManager::getGameObjectsList()
{
    return &mObjects;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
Projectile* GameManager::getProjectile(const string& name)
{
    MProjectile::iterator iter = mProjectiles.find(name);
    if (iter != mProjectiles.end())
        return (*iter).second;

    return 0;
}
//--------------------------------------------------------------------
Projectile* GameManager::createProjectile (const Projectile::Params& params)
{
    string projName = "projectile" + StringConverter::toString(mUnnamedProjectileIndex++);
    return createProjectile(projName, params);
}
//--------------------------------------------------------------------
Projectile* GameManager::createProjectile (const string& projName, const Projectile::Params& params)
{
    //check if it exists already
    if (getProjectile(projName) != 0)
        return getProjectile(projName);

    //it doesnt exist yet. create a new one
    Projectile* proj = new Projectile;
    Entity*     ent  = mSceneMgr->createEntity(projName, params.model);
    SceneNode*  node = acquireSceneNode(true);

    node->attachObject (ent);
    proj->init(this, node, params);
    proj->setName(projName);
    addGameObject(proj);

    mProjectiles.insert( make_pair (projName, proj) );
    
    return proj;
}
//--------------------------------------------------------------------
void GameManager::destroyProjectile (Projectile* proj)
{
    destroyProjectile(proj->getName());    
}
//--------------------------------------------------------------------
void GameManager::destroyProjectile (const string& projName)
{
    MProjectile::iterator iter = mProjectiles.find(projName);
    if (iter != mProjectiles.end())
    {
        Projectile* proj = (*iter).second;
        releaseSceneNode(proj->getNode());
        mProjectiles.erase(iter);
        delete proj;
    }
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
PlayerShip* GameManager::getPlayerShip(const string& name)
{
    MPlayerShip::iterator iter = mPlayerShips.find(name);
    if (iter != mPlayerShips.end())
        return (*iter).second;

    return 0;
}
//--------------------------------------------------------------------
PlayerShip* GameManager::createPlayerShip (const string& meshName)
{
    string name = "PlayerShip" + StringConverter::toString(mUnnamedPlayerShipIndex++);
    return createPlayerShip(name, meshName);
}
//--------------------------------------------------------------------
PlayerShip* GameManager::createPlayerShip (const string& shipName, const string& meshName)
{
    //check if it exists already
    if (getPlayerShip(shipName) != 0)
        return getPlayerShip(shipName);

    //it doesnt exist yet. create a new one
    PlayerShip* ship = new PlayerShip;
    Entity*     ent  = mSceneMgr->createEntity(shipName, meshName);
    SceneNode*  node = acquireSceneNode(true);

    node->attachObject (ent);
    ship->init(node, this);
    ship->setName(shipName);
    addGameObject(ship);

    mPlayerShips.insert( make_pair (shipName, ship) );
    return ship;
}
//--------------------------------------------------------------------
void GameManager::destroyPlayerShip (PlayerShip* ship)
{
    destroyPlayerShip(ship->getName());
}
//--------------------------------------------------------------------
void GameManager::destroyPlayerShip (const string& shipName)
{
    MPlayerShip::iterator iter = mPlayerShips.find(shipName);
    if (iter != mPlayerShips.end())
    {
        PlayerShip* ship = (*iter).second;
        releaseSceneNode(ship->getNode());
        mPlayerShips.erase(iter);
        delete ship;
    }
}
//--------------------------------------------------------------------
//----------------------------------------------------------------------------
EnemyShip*  GameManager::getEnemyShip (const string& name)
{
    MEnemyShip::iterator iter = mEnemyShips.find(name);
    if (iter != mEnemyShips.end())
        return (*iter).second;

    return 0;
}
//--------------------------------------------------------------------
EnemyShip*  GameManager::createEnemyShip  (const string& meshName)
{
    string name = "EnemyShip" + StringConverter::toString(mUnnamedEnemyShipIndex++);
    return createEnemyShip(name, meshName);
}
//--------------------------------------------------------------------
EnemyShip*  GameManager::createEnemyShip  (const string& shipName, const string& meshName)
{
    //check if it exists already
    if (getEnemyShip(shipName) != 0)
        return getEnemyShip(shipName);

    //it doesnt exist yet. create a new one
    EnemyShip* ship = new EnemyShip;
    Entity*    ent  = mSceneMgr->createEntity(shipName, meshName);
    SceneNode* node = acquireSceneNode(true);

    node->attachObject (ent);
    ship->init(node, this);
    ship->setName(shipName);
    addGameObject(ship);

    mEnemyShips.insert( make_pair (shipName, ship) );
    return ship;
}
//--------------------------------------------------------------------
void GameManager::destroyEnemyShip  (EnemyShip* ship)
{
    destroyEnemyShip(ship->getName());
}
//--------------------------------------------------------------------
void GameManager::destroyEnemyShip  (const string& shipName)
{
    MEnemyShip::iterator iter = mEnemyShips.find(shipName);
    if (iter != mEnemyShips.end())
    {
        EnemyShip* ship = (*iter).second;

        releaseSceneNode(ship->getNode());
        mEnemyShips.erase(iter);

        delete ship;
    }
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
ParticleEffect* GameManager::getParticle(const string& name)
{
    MParticleEffect::iterator iter = mParticles.find(name);
    if (iter != mParticles.end())
        return (*iter).second;

    return 0;
}
//--------------------------------------------------------------------
ParticleEffect* GameManager::createParticle (const string& templateName)
{
    //check first if particles from the free list can be reused
    //if not just create a new one

    LParticleEffect::iterator iter;
    for (iter = mFreeParticles.begin();
        iter != mFreeParticles.end();
        iter++)
    {
        if ((*iter)->getTemplateName() == templateName)
            break; //found it!
    }

    if (iter != mFreeParticles.end())
    {
        //found one! reuse it
        ParticleEffect* part = (*iter);

        //mSceneMgr->getRootSceneNode()->addChild(part->getNode());
        part->reset();
        part->getParticleSystem()->setVisible(true);
        ParticleEmitter* em = part->getParticleSystem()->getEmitter(0);
        em->setEnabled(false);
        em->setEnabled(true);
        addGameObject(part);
        mParticles.insert (make_pair (part->getName(), part));

        mFreeParticles.erase(iter);
        return part;
    }
    else
    {
        //just create a new one
        string name = "Particles" + StringConverter::toString(mUnnamedParticleIndex++);
        return createParticle(name, templateName);
    }
    return 0;
}
//--------------------------------------------------------------------
ParticleEffect* GameManager::createParticle (const string& partName, const string& templateName)
{
    //check if it exists already
    if (getParticle(partName) != 0)
        return getParticle(partName);

    ParticleEffect* part = new ParticleEffect;
    ParticleSystem* sys = mSceneMgr->createParticleSystem(partName, templateName);
    SceneNode*      node = acquireSceneNode(true);

    node->attachObject(sys);
    part->init(node, sys, this);
    part->setName(partName);
    part->setTemplatename(templateName);

    addGameObject(part);
    mParticles.insert ( make_pair (partName, part) );

    return part;
}
//--------------------------------------------------------------------
void GameManager::destroyParticle (ParticleEffect* part)
{
    destroyParticle(part->getName());
}
//--------------------------------------------------------------------
void GameManager::destroyParticle (const string& partName)
{
    MParticleEffect::iterator iter = mParticles.find(partName);
    if (iter != mParticles.end())
    {
        //move to free particle list to be reused later
        //SceneNode* node = (*iter).second->getNode();
        //node->getParent()->removeChild(node);

        ParticleSystem* sys = (*iter).second->getParticleSystem();
        sys->getEmitter(0)->setEnabled(false);
        sys->setVisible(false);

        mFreeParticles.push_back ((*iter).second);
        
        mParticles.erase(iter);
    }
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
Asteroid*  GameManager::getAsteroid (const string& name)
{
    MAsteroid::iterator iter = mAsteroids.find(name);
    if (iter != mAsteroids.end())
        return (*iter).second;

    return 0;
}
//--------------------------------------------------------------------
Asteroid*  GameManager::createAsteroid  (const string& meshName)
{
    string name = "Asteroid" + StringConverter::toString(mUnnamedAsteroidIndex++);
    return createAsteroid(name, meshName);
}
//--------------------------------------------------------------------
Asteroid*  GameManager::createAsteroid  (const string& name, const string& meshName)
{
    //check if it exists already
    if (getAsteroid(name) != 0)
        return getAsteroid(name);

    //it doesnt exist yet. create a new one
    Asteroid*  asteroid = new Asteroid;
    Entity*    ent      = mSceneMgr->createEntity(name, meshName);
    SceneNode* node     = acquireSceneNode(true);

    node->attachObject (ent);
    asteroid->init(node, this);
    asteroid->setName(name);
    asteroid->setEntity(ent);
    addGameObject(asteroid);

    mAsteroids.insert( make_pair (name, asteroid) );
    return asteroid;
}
//--------------------------------------------------------------------
void GameManager::destroyAsteroid  (Asteroid* asteroid)
{
    destroyAsteroid(asteroid->getName());
}
//--------------------------------------------------------------------
void GameManager::destroyAsteroid  (const string& name)
{
    MAsteroid::iterator iter = mAsteroids.find(name);
    if (iter != mAsteroids.end())
    {
        Asteroid* asteroid = (*iter).second;

        releaseSceneNode(asteroid->getNode());
        mAsteroids.erase(iter);

        delete asteroid;
    }
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
PowerUp*  GameManager::getPowerUp (const string& name)
{
    MPowerUp::iterator iter = mPowerUps.find(name);
    if (iter != mPowerUps.end())
        return (*iter).second;

    return 0;
}
//--------------------------------------------------------------------
PowerUp*  GameManager::createPowerUp  (const string& meshName)
{
    string name = "PowerUp" + StringConverter::toString(mUnnamedPowerUpIndex++);
    return createPowerUp(name, meshName);
}
//--------------------------------------------------------------------
PowerUp*  GameManager::createPowerUp  (const string& name, const string& meshName)
{
    //check if it exists already
    if (getPowerUp(name) != 0)
        return getPowerUp(name);

    //it doesnt exist yet. create a new one
    PowerUp*   ptr  = new PowerUp;
    Entity*    ent  = mSceneMgr->createEntity(name, meshName);
    SceneNode* node = acquireSceneNode(true);

    node->attachObject (ent);
    ptr->init(node, this);
    ptr->setName(name);
    addGameObject(ptr);

    mPowerUps.insert( make_pair (name, ptr) );
    return ptr;
}
//--------------------------------------------------------------------
void GameManager::destroyPowerUp  (PowerUp* ptr)
{
    destroyPowerUp(ptr->getName());
}
//--------------------------------------------------------------------
void GameManager::destroyPowerUp  (const string& name)
{
    MPowerUp::iterator iter = mPowerUps.find(name);
    if (iter != mPowerUps.end())
    {
        PowerUp* ptr = (*iter).second;

        releaseSceneNode(ptr->getNode());
        mPowerUps.erase(iter);
        delete ptr;
    }
}