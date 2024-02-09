#ifndef _STAGES_H_
#define _STAGES_H_

#include "gameobjects.h"
#include "OverlayButton.h"
#include "messageQueue.h"

namespace PewPew
{
    // Forward declarations
    class StageManager;
    class GameManager;
    class Audio;
    class PlayStage;

    //--------------------------------------------------------------------
    // Lua related functions

    /// Initialize Lua and register script functions.
    /// This function also creates the thread to handle the level script.
    /// @param s Pointer to the play stage
    /// @returns True
    bool lInit(PlayStage* s);

    /// Starts the level script.
    /// @param filename The filename of the script.
    void lStartScript (const string& filename);

    /// Terminates the Lua thread and closes the Lua object.
    /// @returns True
    bool lClose();

    //--------------------------------------------------------------------

    /// The parent class where all stages will be derived from
    class Stage
    {
    protected:
        
        Root*          mRoot;       ///< Pointer to the root object
        SceneManager*  mSceneMgr;   ///< Pointer to the scene manager
        OIS::Mouse*    mMouse;      ///< Pointer to the mouse object
        OIS::Keyboard* mKeyboard;   ///< Pointer to the keyboard object
        StageManager*  mStageMgr;   ///< Pointer to the stage manager
        Audio*         mAudio;      ///< Pointer to the audio object
        bool           mIsInitDone; ///< Initialization is done flag

    public:

        /// Fill out this structure and pass to the stageInit function
        struct InitParams
        {
            Root*          root;      ///< Pointer to the root    
            SceneManager*  sceneMgr;  ///< Pointer to the scene manager  
            OIS::Mouse*    mouse;     ///< Pointer to the mouse object 
            OIS::Keyboard* keyboard;  ///< Pointer to the keyboard object
            StageManager*  stageMgr;  ///< Pointer to the stage manager
            Audio*         audio;     ///< Pointer to the audio object
        };

        /// Constructor. Initialize variables
        Stage() : 
            mRoot(0), mSceneMgr(0), mMouse(0), mAudio(0),
            mKeyboard(0), mStageMgr(0), mIsInitDone(false)
        {}

        /// Destructor.
        virtual ~Stage() {}

        /// Called by the StageManager every frame.
        /// @param timeElapsed The number of seconds that elapsed since the last frame
        /// @returns True if update ran normally, continue running the game. If
        ///          the return value is False, it is time to exit the application/game
        virtual bool update (double timeElapsed) = 0;

        /// Called by the StageManager after a frame has been rendered.
        /// @returns True if update ran normally, continue running the game. If
        ///          the return value is False, it is time to exit the application/game
        virtual bool updateEnd() {return true;}

        /// Called by the StageManager if the stage will be started.
	    virtual void enter() = 0;

        /// Called by the StageManager if the stage will be stopped.
	    virtual void exit() = 0;

        /// Initializes pointers to important objects to be used by the stage.
        /// @param params The structure that contains pointers to be assigned
        ///               to the member variables
        virtual void stageInit(const Stage::InitParams& params)
        {
            mRoot     = params.root;
            mSceneMgr = params.sceneMgr;
            mMouse    = params.mouse;
            mKeyboard = params.keyboard;
            mStageMgr = params.stageMgr;
            mAudio    = params.audio;
            mIsInitDone = true;
        }
    };

    //--------------------------------------------------------------------
    /// The stage that handles the main game
    class PlayStage : public Stage
    {
    protected:

        /// Enumeration for the UI buttons used by this stage
	    enum Buttons
	    {
		    BTN_RESUME = 0, ///< The resume button
            BTN_BACK,       ///< The back button
		    BTN_MAX,        ///< Max number of buttons
	    };

        GameManager*  mGameManager; ///< Pointer to the game manager object
        PlayerShip*   mPlayer;      ///< Pointer to the player ship
        Camera*       mCamera;      ///< Pointer to the main camera
       
        /// This is the AABB of the playing field. Outside of this
        /// box, checking of collision is not done
        AxisAlignedBox mBounds;     

        long mScore;           ///< Current player score
        int  mLevel;           ///< The current level
        string mLevelString;   ///< This is used in getting values from the data in the XML file

        double mSpawnPtZ;        ///< The Z coordinate where the enemies or asteroids will spawn
        double mGameOverTime;    ///< Time to wait before exiting this stage and go back to the main menu
        double mLevelNotifyTime; ///< Timer for the level notifications (i.e. level 1 start!)
        bool   mIsGameOver;      ///< Flag to signal game over
        bool   mIsGameComplete;  ///< Flag to signal that the game is complete
        bool   mGoToNextStage;   ///< Flag to signal that the game should advance to the next stage
        bool   mPaused;          ///< Flag that signals that the game is paused
        bool   mMouseWasDown;    ///< Flag that signals that the mouse has been down since the last update
                
        OverlayElement* mGUILives;   ///< OverlayElement to the number of lives label
        OverlayElement* mGUIHealth;  ///< OverlayElement to the health label
        OverlayElement* mGUIShields; ///< OverlayElement to the shields label
        OverlayElement* mGUIScore;   ///< OverlayElement to the scores label
        OverlayElement* mCursor;     ///< OverlayElement to the cursor
        
        MessageQueue    mMsgIn;      ///< IN message queue from the LUA script to the game. This contains the operations and parameters.
        MessageQueue    mMsgOut;     ///< OUT message queue from the game to the LUA script. This contains return values.
        
        OverlayButton   mButtons[BTN_MAX];  ///< The buttons used when the ESC is pressed.
        ParticleEffect* mStarfieldParticle; ///< Pointer to the starfield particle background

        /// Check collisions for all objects that need checking
        void checkCollisions();

        /// Compare 2 objects if they can be collided with each other.
        /// @param obj1 Pointer to the first object
        /// @param obj2 Pointer to the second object
        void compareCollisionObjects (GameObject* obj1, GameObject* obj2);

        /// Process collision for projectile and enemy ship.
        /// @param proj Pointer to the projectile
        /// @param ship Pointer to the enemy ship
        void collisionProjEnemy      (Projectile* proj, EnemyShip* ship);

        /// Process collision for projectile and player ship.
        /// @param proj Pointer to the projectile
        /// @param ship Pointer to the player ship
        void collisionProjPlayer     (Projectile* proj, PlayerShip* ship);

        /// Process collision for two projectiles.
        /// @param proj1 Pointer to the first projectile
        /// @param proj2 Pointer to the second projectile
        void collisionProjProj       (Projectile* proj1, Projectile* proj2);

        /// Process collision between player and enemy ships.
        /// @param enemy Pointer to the enemy ship
        /// @param player Pointer to the player ship
        void collisionEnemyPlayer    (EnemyShip* enemy, PlayerShip* player);

        /// Process collision between projectile and asteroid.
        /// @param proj Pointer to the projectile
        /// @param asteroid Pointer to the asteroid
        void collisionProjAsteroid   (Projectile* proj, Asteroid* asteroid);

        /// Process collision between player ship and asteroid.
        /// @param ship Pointer to the player ship
        /// @param asteroid Pointer to the asteroid
        void collisionPlayerAsteroid (PlayerShip* ship, Asteroid* asteroid);

        /// Process collision between player ship and power up.
        /// @param ship Pointer to the player ship
        /// @param powerUp Pointer to the power up
        void collisionPlayerPowerUp  (PlayerShip* ship, PowerUp* powerUp);

        /// Initializes game variables found in the xml files.
        /// @returns True
        bool initData();

        /// Initializes the 3D scene. It sets up the lights, camera and
        /// starfield effect.
        /// @returns True
        bool initScene();

        /// Refreshes the lives, health, shields and score labels.
        void updateStats();

        /// Update function that is called when the ESC button is pressed and pauses the game.
        /// This is when the small dialog with resume and exit buttons are shown.
        /// @param timeElapsed The time in seconds since the last frame render.
        /// @returns True
        bool updatePaused(double timeElapsed);

        /// Helper function to transform the bounding box of the mesh.
        /// The transformation info (position, rotation and scale) are 
        /// taken from the scene node. The bounding box is then transformed
        /// using that data.
        /// @param node The pointer to the scene node of the object.
        /// @param aabb The reference to the axis aligned bounding box
        void transformBoundingBox (SceneNode* node, AxisAlignedBox& aabb);

        /// Processes messages that came from the Lua script.
        void processMessages();

    public:

        /// Constructor.
        PlayStage();

        /// Destructor.
        virtual ~PlayStage();

        //overrides
        bool update (double timeElapsed);
        bool updateEnd();
	    void enter();
	    void exit();
        //end: overrides

        /// Get the Z coordinate where the enemies will spawn from.
        /// @returns The Z coordinate where the enemies will spawn from.
        double getSpawnPtZ() const { return mSpawnPtZ; }

        /// Gets a pointer to the GameManager object.
        /// @returns A pointer to the GameManager object.
        GameManager* getGameMgr() const {return mGameManager;}

        /// Gets a reference to the IN message queue.
        /// @returns A reference to the IN message queue.
        MessageQueue& getMsgIn()  {return mMsgIn;}

        /// Gets a reference to the OUT message queue.
        /// @returns A reference to the OUT message queue.
        MessageQueue& getMsgOut() {return mMsgOut;}

        /// Retrieves the variables from the XML data for a weapon
        /// and puts that data to the Weapon::Params object.
        /// @param weaponName The name of the weapon to find.
        /// @param p A reference to the Weapon::Params object to be filled up with data.
        static void getWeaponParams(const string& weaponName, Weapon::Params& p);
    };

    //--------------------------------------------------------------------
    /// Class that handles the main menu stage.
    /// This shows a start button to start the game;
    /// a credits button to show the credits screen;
    /// and an exit button to go back to windows.
    class TitleStage : public Stage
    {
    public:

        /// Enumeration for the UI buttons used by this stage
	    enum Buttons
	    {
		    BTN_START = 0,   ///< The start button
            BTN_CREDITS,     ///< The credits button
		    BTN_EXIT,        ///< The exit button
		    BTN_MAX,         ///< Maximum number of buttons
	    };

    protected:
        bool            mIsFirstRun;       ///< Flag to signal that it is the first run of this class
        bool            mMouseWasDown;     ///< Flag that signals that the mouse has been down since the last update
        double          mIntroSoundTime;   ///< Sound delay between the *pew* *pew* sound when the game starts.
        OverlayElement* mCursor;           ///< OverlayElement to the cursor
        OverlayButton   mButtons[BTN_MAX]; ///< An array of the buttons used

    public:
        /// Constructor.
        TitleStage();

        /// Destructor.
        virtual ~TitleStage();

        /// overrides
        bool update (double timeElapsed);
	    void enter();
	    void exit();
    };

    //--------------------------------------------------------------------
    /// Class that handles the credits stage.
    /// This shows the credits screen.
    class CreditsStage : public Stage
    {
    protected:
        OverlayElement* mCursor; ///< OverlayElement to the cursor

    public:

        /// Constructor.
        CreditsStage();

        /// Destructor.
        virtual ~CreditsStage();

        //overrides
        bool update (double timeElapsed);
	    void enter();
	    void exit();
    };
}

#endif