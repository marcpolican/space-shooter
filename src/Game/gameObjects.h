#ifndef _GAMEOBJECTS_H_
#define _GAMEOBJECTS_H_

using namespace Ogre;

namespace PewPew
{
    //forward declarations
    class GameManager;
    class Audio;

    //--------------------------------------------------------------------
    ///enum for GameObject types so we can distinguish what type
    ///of object we can cast the GameObject pointer to.
    enum ObjectType
    {
        OBJ_NULL,
        OBJ_SHIP,
        OBJ_PLAYER_SHIP,
        OBJ_ENEMY_SHIP,
        OBJ_BOSS,
        OBJ_PROJECTILE,
        OBJ_WEAPON,
        OBJ_POWERUP,
        OBJ_PARTICLE_EFFECT,
        OBJ_ASTEROID,
    };

    //--------------------------------------------------------------------
    /// Super class of ships, projectiles, particles, etc
    class GameObject
    {
    protected:
        int  mType;
        bool mIsActive;
        string mName;
    public:
        GameObject() : mIsActive(true) {}
        virtual ~GameObject() {}

        virtual bool update (double timeElapsed) = 0;
        virtual int  getType() const = 0;
        virtual SceneNode* getNode() const = 0;

        bool isActive() const                         {return mIsActive;}
        virtual bool checkCollision (GameObject* obj) {return false;}
        virtual bool isCollidable() const             {return false;}
        virtual void kill() {}
        
        const string& getName() {return mName;}

        // to be called only by GameManager
        void setName(const string& name) {mName = name;}
    };

    //--------------------------------------------------------------------
    /// Super class of the movment objects
    class IMovement
    {
    public:
        virtual bool update (double timeElapsed) = 0;
        virtual SceneNode* getNode() const = 0;
    };

    //--------------------------------------------------------------------
    /// Straight line movement with a given velocity
    class MoveStraight : public IMovement
    {
    protected:
        SceneNode* mNode;
        Vector3 mVelocity;
    public:
        MoveStraight(SceneNode* node, const Vector3& velocity);
        bool update (double timeElapsed);
        SceneNode* getNode() const {return mNode;}
    };

    //--------------------------------------------------------------------
    /// Straight line movement while the object is spinning
    class MoveStraightSpin : public IMovement
    {
    protected:
        SceneNode* mNode;
        Vector3 mVelocity;
        Vector3 mAxis;
        Radian mSpin;
    public:
        MoveStraightSpin(SceneNode* node, const Vector3& velocity, const Radian& spin, const Vector3& axis = Vector3::UNIT_X);
        bool update (double timeElapsed);
        SceneNode* getNode() const {return mNode;}
    };

    //--------------------------------------------------------------------
    /// Movement object to go to a certain position
    class MoveToDest: public IMovement
    {
    protected:
        SceneNode* mNode;
        Vector3 mStart;
        Vector3 mDest;
        double  mDuration;
        double  mTime;
        bool mIsTurning;
    public:
        MoveToDest(SceneNode* node, const Vector3& dest, double speed, bool isTurning = false);
        bool update (double timeElapsed);
        SceneNode* getNode() const {return mNode;}
    };

    //--------------------------------------------------------------------
    /// The projectile class for bullets
    class Projectile : public GameObject
    {
    protected:
        IMovement*   mMove;
        GameManager* mMgr;
        long         mDamage;
        bool         mFromPlayer;

    public:
        typedef struct Params
        {
            bool    fromPlayer;
            string  model;
            long    damage;
            Vector3 velocity;
            Vector3 origin;

            Params() : fromPlayer(true), model(""), damage(100), velocity(0.0), origin(0.0)
            {}
        };

        Projectile();
        ~Projectile();
        bool init (GameManager* mgr, SceneNode* node, const Params& p);
        bool update (double timeElapsed);
        SceneNode* getNode() const;
        int  getType() const;
        void kill();

        bool isFromPlayer() const {return mFromPlayer;}
        long getDamage () const   {return mDamage;}
        bool isCollidable() const {return true;}
    };

    //--------------------------------------------------------------------
    /// The weapon class which handles firing of projectiles
    class Weapon : public GameObject
    {
    protected:
        typedef vector <Projectile::Params> VProjParams;
        SceneNode*      mNode;
        double          mFireDelay;
        double          mFireTime;
        long            mNumAmmo;
        GameManager*    mMgr;
        VProjParams     mProjParams;

    public:
        typedef struct Params
        {
            string model;
            double fireDelay;
            long   numAmmo;
            VProjParams projectiles;

            Params() : model(""), fireDelay(0.5), numAmmo(-1)
            {}
        };

        Weapon();
        ~Weapon();

        bool init(SceneNode* node, GameManager* mgr);
        bool setParams(Params params);
        void fire();
        bool update(double timeElapsed);
        bool isReadyToFire();
        int  getType() const;
        SceneNode* getNode() const;
        bool isCollidable() const {return false;}
    };

    //--------------------------------------------------------------------
    /// The ship class which is the parent of player and enemy ships
    class Ship : public GameObject
    {
    protected:
        long         mHP;
        long         mShields;
        SceneNode*   mNode;
        Weapon       mWeapon;
        GameManager* mMgr;

    public:
        Ship();
        ~Ship();
        bool init(SceneNode* node, GameManager* mgr);
        bool update(double timeElapsed);
        bool isHit(const AxisAlignedBox& bbox, const Matrix4& mat) const;
        void doHit(long damage); //do hit animation, etc.
        virtual void doDeath(); //do death animation, etc

        Weapon*    getWeapon();
        SceneNode* getNode() const;

        bool isDead() const;

        void setHP(long hp);
        void addHP(long hp);
        void subHP(long hp);
        long getHP() const;

        void setShields(long shields);
        void addShields(long shields);
        void subShields(long shields);
        long getShields() const;

        virtual void damage(long dmg);
        void kill();

        virtual int getType() const;
    };

    //--------------------------------------------------------------------
    /// The ship that the player controls
    class PlayerShip : public Ship
    {
    protected:
        Vector3        mSpeed;
        Vector3        mPosChange;
        double         mPosZ;
        double         mMoveTotalX;
        double         mTiltLimit;
        double         mTiltSpeed;
        double         mTiltReturnSpeed;
        double         mRespawnWait;
        double         mDamagedTimer;
        const double   mDamagedTimerInitial;
        int            mLives;
        long           mInitHP;

        OIS::Mouse*    mMouse;
        OIS::Keyboard* mKeyboard;
        Audio*         mAudio;
        AxisAlignedBox mBounds;
        Weapon::Params mFirstWeapon;

    public:
        PlayerShip();
        ~PlayerShip();

        bool initInput(OIS::Mouse* mouse, OIS::Keyboard* keyboard);
        void initAudio(Audio* audio);
        void initBounds(const AxisAlignedBox& box);
        void setSpeed(const Vector3& moveSpeed);
        bool update(double timeElapsed);

        void setTiltValues (double tiltLimit, double tiltSpeed, double tiltReturnSpeed);

        int getType() const;
        bool isCollidable() const;
        void damage(long dmg);

        void doDeath();
        void setFirstWeapon(const Weapon::Params& w);

        bool canBeHit() const;

        int  getLives() const { return mLives; }
        void setLives(int lives) {mLives = lives;}

        long getInitHP() const {return mInitHP;}
        void setInitHP(long hp) {mInitHP = hp;}
        
        double getPosZ() const {return mPosZ;}
        void   setPosZ(double z) {mPosZ = z;}
    };

    //--------------------------------------------------------------------
    /// The enemy ship class
    class EnemyShip : public Ship
    {
    protected:
        IMovement*      mMove;
        double          mDyingTimer;
        ParticleSystem* mParticle;
        bool            mIsBoss;

    public:
        EnemyShip();
        ~EnemyShip();

        void setMovement(IMovement* movement);
        bool update(double timeElapsed);

        int getType() const;
        bool isCollidable() const {return true;}

        void doDeath();

        void setIsBoss(bool b) {mIsBoss = b;}
        bool isBoss() const    {return mIsBoss;}
    };

    //--------------------------------------------------------------------
    /// A class for handling particle effects
    class ParticleEffect : public GameObject
    {
    protected:
        double          mDuration;
        double          mTimeElapsed;
        SceneNode*      mNode;
        GameManager*    mMgr;
        string          mTemplateName;
        ParticleSystem* mParticleSystem;

    public:
        ParticleEffect();
        ~ParticleEffect() {}

        bool init (SceneNode* node, ParticleSystem* part, GameManager* mgr);
        bool update (double timeElapsed);

        int  getType() const                {return OBJ_PARTICLE_EFFECT;}
        void setDuration(double duration)   {mDuration = duration;}

        SceneNode* getNode() const                {return mNode;}
        ParticleSystem* getParticleSystem() const {return mParticleSystem;}

        const string& getTemplateName() const     {return mTemplateName;}
        void setTemplatename(const string& name)  {mTemplateName = name;}

        void reset();
    };

    //--------------------------------------------------------------------
    /// The asteroid class
    class Asteroid : public GameObject
    {
    protected:
        long         mHP;
        SceneNode*   mNode;
        GameManager* mMgr;
        IMovement*   mMove;
        Entity*      mEntity;
        string       mPowerUp;

        ParticleSystem* mParticle;

    public:
        Asteroid();
        ~Asteroid();

        bool init (SceneNode* node, GameManager* mgr);
        bool update(double timeElapsed);
        void setHP(long hp);
        long getHP() const;
        void damage(long damage);
        void kill();
        void setMovement(IMovement* movement);

        SceneNode* getNode() const;

        bool isCollidable() const {return true;}
        int  getType()      const {return OBJ_ASTEROID;}

        void setEntity(Entity* ent) { mEntity = ent;  }
        Entity* getEntity() const   { return mEntity; }

        void setPowerUp(const string& p) { mPowerUp = p; }
        string getPowerUp() const        { return mPowerUp; }
    };

    //--------------------------------------------------------------------
    /// Powerups that come from asteroids
    class PowerUp : public GameObject
    {
    protected:
        SceneNode*   mNode;
        GameManager* mMgr;
        IMovement*   mMove;
        string       mWeapon;

    public:
        PowerUp();
        ~PowerUp();

        bool init (SceneNode* node, GameManager* mgr);
        bool update (double timeElapsed);
        void setMovement(IMovement* movement);

        SceneNode* getNode() const {return mNode;}
        bool isCollidable() const  {return true;}
        int  getType() const       {return OBJ_POWERUP;}

        string getWeapon() const {return mWeapon;}
        void setWeapon(const string& w) {mWeapon = w;}

        void acquired();
    };
}

#endif