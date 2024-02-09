#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include "gameobjects.h"

namespace PewPew
{
    class GameObject;
    class Projectile;
    class Ship;
    class PlayerShip;
    class EnemyShip;
    class Weapon;
    class ParticleEffect;
    class Asteroid;
    class PowerUp;

    /** Contains all game objects so they all can be updated every frame.
        Holds a collection of scene nodes. keeps track of which
        scene nodes are used an which are not. recycles the ones
        that are not used. creates new scene nodes when needed
    */
    class GameManager
    {
    public:
        typedef list <SceneNode*> LNodes;
        typedef list <GameObject*> LGameObjects;
        typedef list <ParticleEffect*> LParticleEffect;
        typedef map <string, Projectile*> MProjectile;
        typedef map <string, PlayerShip*> MPlayerShip;
        typedef map <string, EnemyShip*>  MEnemyShip;
        typedef map <string, ParticleEffect*>  MParticleEffect;
        typedef map <string, Asteroid*>  MAsteroid;
        typedef map <string, PowerUp*>  MPowerUp;

    protected:

        Root*           mRoot;
        SceneManager*   mSceneMgr;
        LGameObjects    mObjects;
        LParticleEffect mFreeParticles;

        MProjectile     mProjectiles;
        MPlayerShip     mPlayerShips;
        MEnemyShip      mEnemyShips;
        MParticleEffect mParticles;
        MAsteroid       mAsteroids;
        MPowerUp        mPowerUps;

        int mUnnamedProjectileIndex;
        int mUnnamedPlayerShipIndex;
        int mUnnamedEnemyShipIndex;
        int mUnnamedParticleIndex;
        int mUnnamedAsteroidIndex;
        int mUnnamedPowerUpIndex;

        template <class T>
        void deleteContents (T& myMap);

    public:
        GameManager();
        ~GameManager();

        bool init   (Root* r, SceneManager* sceneMgr);
        bool update (double timeElapsed);
        void addGameObject   (GameObject* obj);
        void removeGameObject(GameObject* obj);
        void releaseAll();
        SceneNode* acquireSceneNode(bool isAttachToRootNode = false);
        void releaseSceneNode(SceneNode* node);
        LGameObjects* getGameObjectsList();

        SceneManager* getSceneMgr() {return mSceneMgr;}
        Root* getRoot()             {return mRoot;}

        //pattern from scene manager----------------------------------------
        Projectile* getProjectile(const string& name);
        Projectile* createProjectile (const Projectile::Params& params);
        Projectile* createProjectile (const string& projName, const Projectile::Params& params);
        void destroyProjectile (Projectile* proj);
        void destroyProjectile (const string& projName);

        //player ship functions---------------------------------------------
        PlayerShip* getPlayerShip(const string& name);
        PlayerShip* createPlayerShip (const string& meshName);
        PlayerShip* createPlayerShip (const string& shipName, const string& meshName);
        void destroyPlayerShip (PlayerShip* ship);
        void destroyPlayerShip (const string& shipName);

        //enemy ship functions----------------------------------------------
        EnemyShip*  getEnemyShip (const string& name);
        EnemyShip*  createEnemyShip  (const string& meshName);
        EnemyShip*  createEnemyShip  (const string& shipName, const string& meshName);
        void destroyEnemyShip  (EnemyShip* ship);
        void destroyEnemyShip  (const string& shipName);

        //particle effect functions-----------------------------------------
        ParticleEffect* getParticle(const string& name);
        ParticleEffect* createParticle (const string& templateName);
        ParticleEffect* createParticle (const string& partName, const string& templateName);
        void destroyParticle (ParticleEffect* part);
        void destroyParticle (const string& partName);
        
        //weapon functions--------------------------------------------------
        Weapon* getWeapon(const string& name);
        Weapon* createWeapon (const string& meshName);
        Weapon* createWeapon (const string& weapName, const string& meshName);
        void destroyWeapon (Weapon* weapon);
        void destroyWeapon (const string& weapName);

        //asteroid functions------------------------------------------------
        Asteroid*  getAsteroid (const string& name);
        Asteroid*  createAsteroid  (const string& meshName);
        Asteroid*  createAsteroid  (const string& name, const string& meshName);
        void destroyAsteroid  (Asteroid* asteroid);
        void destroyAsteroid  (const string& name);

        //powerup functons--------------------------------------------------
        PowerUp*  getPowerUp (const string& name);
        PowerUp*  createPowerUp  (const string& meshName);
        PowerUp*  createPowerUp  (const string& name, const string& meshName);
        void destroyPowerUp  (PowerUp* ptr);
        void destroyPowerUp  (const string& name);
    };
}

#endif