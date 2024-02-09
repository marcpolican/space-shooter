#include "pch.h"
#include "gamemanager.h"
#include "stages.h"
#include "globalvars.h"
#include "audio.h"
#include "utils.h"
#include "stagemanager.h"

using namespace PewPew;


DWORD WINAPI clearSceneFunc( LPVOID param ) 
{ 
	SceneManager* sceneMgr = static_cast <SceneManager*> (param);
    sceneMgr->clearScene();
    return 0;
} 
//--------------------------------------------------------------------
PlayStage::PlayStage() :
    mGameManager (new GameManager),
    mPlayer(0),
    mCamera(0),
    mScore(0),
    mGUILives(0),
    mGUIHealth(0),
    mGUIShields(0),
    mGUIScore(0),
    mLevel(0),
    mSpawnPtZ(0),
    mPaused(false),
    mCursor(0),
    mGameOverTime(0.0),
    mIsGameOver(false),
    mIsGameComplete(false),
    mGoToNextStage(0),
    mStarfieldParticle(0)
{
}
//--------------------------------------------------------------------
PlayStage::~PlayStage()
{
    delete mGameManager;
}
//--------------------------------------------------------------------
bool PlayStage::update (double timeElapsed)
{
    if (!mIsInitDone) return true;

    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE) && !mIsGameOver)
    {
        //show paused overlay
        OverlayManager::getSingleton().getByName("PewPew/GamePaused/Overlay")->show();
        
	    //init cursor overlay
        OverlayManager::getSingletonPtr()->getByName("PewPew/Cursor/Overlay")->show();
	    mCursor = OverlayManager::getSingletonPtr()->getOverlayElement("PewPew/Cursor");

        mButtons[BTN_RESUME].loadButton("PewPew/TitleScreen/BtnResume");
        mButtons[BTN_BACK].loadButton("PewPew/TitleScreen/BtnBack");

        ControllerManager::getSingleton().setTimeFactor(0);
        mPaused = true;
        return true;
    }

    if (mPlayer->getLives() == 0 && !mIsGameOver)
    {
        //show game over overlay
        OverlayManager::getSingleton().getByName("PewPew/GameOver/Overlay")->show();
        mGameOverTime = GLOBALS->getDouble("globals/dGameOverDelay");
        mIsGameOver = true;  
    }

    if (mIsGameOver)
    {
        mGameOverTime -= timeElapsed;
        if (mGameOverTime <= 0)
        {
            mStageMgr->setCurrentStage("titleStage");
            return true;
        }
    }

    if (mPaused)
        return updatePaused(timeElapsed);
    
    if (mLevelNotifyTime >= 0)
    {
        mLevelNotifyTime -= timeElapsed;
        if (mLevelNotifyTime < 0)
            OverlayManager::getSingleton().getByName("PewPew/LevelNotify/Overlay")->hide();
    }

    //update entities
    mGameManager->update(timeElapsed);
    processMessages();

    if (!mIsGameOver)
    {
        if (mGoToNextStage)
        {
            mLevel++;
            mLevelString = "level" + StringConverter::toString(mLevel);

            string scriptFile = GLOBALS->getString (mLevelString + "/szScript") ;

            if (scriptFile != "")
            {
                OverlayManager::getSingleton().getByName("PewPew/LevelNotify/Overlay")->show();
                OverlayElement* elem = OverlayManager::getSingleton().getOverlayElement("PewPew/LevelNotify/Text");
                elem->setCaption(GLOBALS->getString(mLevelString + "/szName") + " Start!!");
                mLevelNotifyTime = 2.0;


                //have some delay saying level is complete
                lClose();
                lInit(this);
                lStartScript ( scriptFile );
                mGoToNextStage = false;

                mMsgIn.clear();
                mMsgOut.clear();
            }
            else
            {
                //show congrats
                OverlayManager::getSingleton().getByName("PewPew/LevelNotify/Overlay")->show();
                OverlayElement* elem = OverlayManager::getSingleton().getOverlayElement("PewPew/LevelNotify/Text");
                elem->setCaption("Congratulations!!\nYou have completed the game");

                mGameOverTime = GLOBALS->getDouble("globals/dGameOverDelay");
                mIsGameOver = true;
            }
        }

        checkCollisions();
        updateStats();
    }
    return true;
}
//--------------------------------------------------------------------
bool PlayStage::updateEnd()
{
    return true;
}
//--------------------------------------------------------------------
bool PlayStage::updatePaused(double timeElapsed)
{
    OIS::MouseState ms = mMouse->getMouseState();

    double mouseX = ms.X.abs / (float) ms.width;
    double mouseY = ms.Y.abs / (float) ms.height;

    mCursor->setPosition(mouseX, mouseY);

    if (ms.buttonDown(OIS::MB_Left) && !mMouseWasDown)
    {
        for (int i=0; i<BTN_MAX; i++)
            mButtons[i].mousePressed(mouseX, mouseY);

        mMouseWasDown = true;
    }
    else if (!ms.buttonDown(OIS::MB_Left) && mMouseWasDown)
    {
        for (int i=0; i<BTN_MAX; i++)
            mButtons[i].mouseReleased(mouseX, mouseY);

        mMouseWasDown = false;
    }

    if (mButtons[BTN_RESUME].isClicked())
    {
        mAudio->play("click");
        OverlayManager::getSingleton().getByName("PewPew/GamePaused/Overlay")->hide();
        OverlayManager::getSingletonPtr()->getByName("PewPew/Cursor/Overlay")->hide();
        ControllerManager::getSingleton().setTimeFactor(1);
        mPaused = false;
    }

    if (mButtons[BTN_BACK].isClicked())
    {
        mAudio->play("click");
        mStageMgr->setCurrentStage("titleStage");
        OverlayManager::getSingleton().getByName("PewPew/GamePaused/Overlay")->hide();
        OverlayManager::getSingletonPtr()->getByName("PewPew/Cursor/Overlay")->hide();
        ControllerManager::getSingleton().setTimeFactor(1);
    }

    return true;
}
//--------------------------------------------------------------------
void PlayStage::enter()
{
    OverlayManager::getSingleton().getByName("PewPew/Playstage/Overlay")->show();
    mGUILives   = OverlayManager::getSingleton().getOverlayElement("PewPew/Playstage/LivesValue");
    mGUIHealth  = OverlayManager::getSingleton().getOverlayElement("PewPew/Playstage/HealthValue");
    mGUIShields = OverlayManager::getSingleton().getOverlayElement("PewPew/Playstage/ShieldsValue");
    mGUIScore   = OverlayManager::getSingleton().getOverlayElement("PewPew/Playstage/ScoreValue");

    if (!mIsInitDone) return;

    initScene();
    initData();

    mLevel = 0;

    mCamera->lookAt (0,0,0);
    mAudio->play("music2",true);

    mLevelString = "level" + StringConverter::toString(mLevel);
    if (lInit(this))
        lStartScript ( GLOBALS->getString (mLevelString + "/szScript") );

    OverlayManager::getSingleton().getByName("PewPew/LevelNotify/Overlay")->show();
    OverlayElement* elem = OverlayManager::getSingleton().getOverlayElement("PewPew/LevelNotify/Text");
    elem->setCaption(GLOBALS->getString(mLevelString + "/szName") + " Start!!");
    mLevelNotifyTime = 2.0;

    mPaused = false;
    mIsGameOver = false;
    mGoToNextStage = false;
}
//--------------------------------------------------------------------
void PlayStage::exit()
{
    OverlayManager::getSingleton().getByName("PewPew/Playstage/Overlay")->hide();
    OverlayManager::getSingleton().getByName("PewPew/GameOver/Overlay")->hide();
    OverlayManager::getSingleton().getByName("PewPew/LevelNotify/Overlay")->hide();

    mGameManager->releaseAll();
    mSceneMgr->clearScene();

    lClose();
    mMsgIn.clear();
    mMsgOut.clear();

    //create a thread to call clearScene so we dont get a very long delay
	HANDLE thread;
	DWORD  threadID;
    thread = CreateThread(NULL, 0, clearSceneFunc, mSceneMgr, 0, &threadID);

    mAudio->stop("music2");
}
//--------------------------------------------------------------------
void PlayStage::checkCollisions()
{
    GameManager::LGameObjects* list = mGameManager->getGameObjectsList();

    if (list->size() < 2) return;

    GameManager::LGameObjects::iterator iter;
    GameManager::LGameObjects::iterator iter2;

    for (iter = list->begin(); iter != list->end(); iter++)
    {
        if (!(*iter)->isCollidable()) continue;

        iter2 = iter;
        iter2++;
        if (iter2 == list->end())
            break;

        for (; iter2 != list->end(); iter2++)
        {
            if (!(*iter2)->isCollidable()) continue;

            compareCollisionObjects((*iter), (*iter2));
        }
    }
}
//--------------------------------------------------------------------
void PlayStage::compareCollisionObjects(GameObject* obj1, GameObject* obj2)
{
    //collision occured do something about it!
    int type1 = obj1->getType();
    int type2 = obj2->getType();

    //define a macro so it is easier
#define COMPARE_PAIR(a,b) ((type1==a && type2==b) || (type1==b && type2==a))

    if (COMPARE_PAIR(OBJ_PROJECTILE, OBJ_ENEMY_SHIP))
    {
        //kill projectile
        if (type1 == OBJ_PROJECTILE)
            collisionProjEnemy(static_cast<Projectile*>(obj1),static_cast<EnemyShip*> (obj2));
        else
            collisionProjEnemy(static_cast<Projectile*>(obj2),static_cast<EnemyShip*> (obj1));
    }

    else if (COMPARE_PAIR(OBJ_PROJECTILE, OBJ_PLAYER_SHIP))
    {
        //kill projectile
        if (type1 == OBJ_PROJECTILE)
            collisionProjPlayer(static_cast<Projectile*>(obj1),static_cast<PlayerShip*> (obj2));
        else
            collisionProjPlayer(static_cast<Projectile*>(obj2),static_cast<PlayerShip*> (obj1));
    }


    else if (COMPARE_PAIR(OBJ_PLAYER_SHIP, OBJ_ENEMY_SHIP))
    {
        //kill projectile
        if (type1 == OBJ_ENEMY_SHIP)
            collisionEnemyPlayer(static_cast<EnemyShip*>(obj1),static_cast<PlayerShip*> (obj2));
        else
            collisionEnemyPlayer(static_cast<EnemyShip*>(obj2),static_cast<PlayerShip*> (obj1));
    }

    else if (COMPARE_PAIR(OBJ_PROJECTILE, OBJ_ASTEROID))
    {
        if (type1 == OBJ_PROJECTILE)
            collisionProjAsteroid(static_cast<Projectile*>(obj1),static_cast<Asteroid*>(obj2));
        else
            collisionProjAsteroid(static_cast<Projectile*>(obj2),static_cast<Asteroid*>(obj1));
    }

    else if (COMPARE_PAIR(OBJ_PLAYER_SHIP, OBJ_ASTEROID))
    {
        if (type1 == OBJ_PLAYER_SHIP)
            collisionPlayerAsteroid(static_cast<PlayerShip*>(obj1),static_cast<Asteroid*>(obj2));
        else
            collisionPlayerAsteroid(static_cast<PlayerShip*>(obj2),static_cast<Asteroid*>(obj1));
    }

    else if (COMPARE_PAIR(OBJ_PLAYER_SHIP, OBJ_POWERUP))
    {
        if (type1 == OBJ_PLAYER_SHIP)
            collisionPlayerPowerUp(static_cast<PlayerShip*>(obj1),static_cast<PowerUp*>(obj2));
        else
            collisionPlayerPowerUp(static_cast<PlayerShip*>(obj2),static_cast<PowerUp*>(obj1));
    }

    else if (type1 == OBJ_PROJECTILE && type2 == OBJ_PROJECTILE)
    {
        collisionProjProj(static_cast<Projectile*>(obj1),static_cast<Projectile*>(obj2));
    }

    //undefine it so we dont get confused if we see it elsewhere
#undef COMPARE_PAIR
}
//--------------------------------------------------------------------
void PlayStage::collisionProjEnemy   (Projectile* proj, EnemyShip* ship)
{
    //check if the projectile came from the player
    if (!proj->isFromPlayer()) return;

    AxisAlignedBox box = ship->getNode()->getAttachedObject(0)->getBoundingBox();
    transformBoundingBox (ship->getNode(), box );

    Vector3 pos2 = proj->getNode()->getWorldPosition();

    //check if objects are inside the playing field
    if (!box.intersects(mBounds)) return;
    if (!mBounds.contains (pos2)) return;
        
    if (box.contains(pos2))
    {
        ship->damage(proj->getDamage());
        proj->kill();
        if (ship->getHP() <= 0)
        {
            mAudio->play("explode");
            ship->kill();
            if (ship->isBoss())
                mGoToNextStage = true;
            mScore += 100;
        }
    }   
}
//--------------------------------------------------------------------
void PlayStage::collisionProjPlayer  (Projectile* proj, PlayerShip* ship)
{
    //check if the projectile came from the player
    if (proj->isFromPlayer()) return;
    if (!ship->canBeHit()) return;

    AxisAlignedBox box = ship->getNode()->getAttachedObject(0)->getBoundingBox();
    transformBoundingBox (ship->getNode(), box );

    Vector3 pos2 = proj->getNode()->getWorldPosition();

    //check if objects are inside the playing field
    if (!box.intersects(mBounds)) return;
    if (!mBounds.contains (pos2)) return;
        
    if (box.contains(pos2))
    {
        ship->damage(proj->getDamage());
        proj->kill();
        mAudio->play("playerhit");
        if (ship->getHP() <= 0)
        {
            mAudio->play("explode");
            ship->kill();
        }
    }   
}
//--------------------------------------------------------------------
void PlayStage::collisionProjProj    (Projectile* proj1, Projectile* proj2)
{
    if ((proj1->isFromPlayer() && !proj2->isFromPlayer()) ||
        (proj2->isFromPlayer() && !proj1->isFromPlayer()))
    {
        AxisAlignedBox box1 = proj1->getNode()->getAttachedObject(0)->getBoundingBox();
        AxisAlignedBox box2 = proj2->getNode()->getAttachedObject(0)->getBoundingBox();

        transformBoundingBox (proj1->getNode(), box1 );
        transformBoundingBox (proj2->getNode(), box2 );

        //check if objects are inside the playing field
        if (!box1.intersects(mBounds)) return;
        if (!box2.intersects(mBounds)) return;

        if (box1.intersects( box2 ))
        {
            proj1->kill();
            proj2->kill();
        }
    }
}
//--------------------------------------------------------------------
void PlayStage::collisionEnemyPlayer (EnemyShip* enemy, PlayerShip* player)
{
    if (!player->canBeHit()) return;

    AxisAlignedBox boxEnemy = enemy->getNode()->getAttachedObject(0)->getBoundingBox();
    AxisAlignedBox boxPlayer = player->getNode()->getAttachedObject(0)->getBoundingBox();

    transformBoundingBox (enemy->getNode(), boxEnemy );
    transformBoundingBox (player->getNode(), boxPlayer );

    //check if objects are inside the playing field
    if (!boxEnemy.intersects(mBounds)) return;
    if (!boxPlayer.intersects(mBounds)) return;

    if (boxPlayer.intersects( boxEnemy ))
    {
        mAudio->play("explode");
        //enemy->doDeath();
        player->damage(10000); //instantly kill the player
    }
}
//--------------------------------------------------------------------
void PlayStage::collisionProjAsteroid(Projectile* proj, Asteroid* asteroid)
{
    if (!proj->isFromPlayer()) return;

    AxisAlignedBox boxAsteroid = asteroid->getNode()->getAttachedObject(0)->getBoundingBox();
    transformBoundingBox (asteroid->getNode(), boxAsteroid );

    Vector3 pos2 = proj->getNode()->getWorldPosition();

    //check if objects are inside the playing field
    if (!boxAsteroid.intersects(mBounds)) return;
    if (!mBounds.contains (pos2)) return;
        
    if (boxAsteroid.contains(pos2))
    {
        asteroid->damage(proj->getDamage());
        proj->kill();

        if (asteroid->getHP() <= 0)
        {
            mAudio->play("explode");
            asteroid->kill();
            mScore += 100;

            if (!asteroid->getPowerUp().empty())
            {
                double   speed = 0;
                PowerUp* ptr = 0;
                string   mesh = GLOBALS->getString (asteroid->getPowerUp() + "/szModel");

                speed = GLOBALS->getDouble(asteroid->getPowerUp() + "/dSpeedZ");

                if (!mesh.empty())
                    ptr = mGameManager->createPowerUp(mesh);
                else
                    ptr = mGameManager->createPowerUp("PowerUp1.mesh"); 

                ptr->setWeapon ( GLOBALS->getString (asteroid->getPowerUp() + "/szWeapon") );
                ptr->getNode()->setPosition( asteroid->getNode()->getPosition() );
                ptr->setMovement( new MoveStraight( ptr->getNode(), Vector3(0,0,speed) ));
            }
        }
    }   
}
//--------------------------------------------------------------------
void PlayStage::collisionPlayerAsteroid (PlayerShip* ship, Asteroid* asteroid)
{
    AxisAlignedBox boxShip = ship->getNode()->getAttachedObject(0)->getBoundingBox();
    AxisAlignedBox boxAsteroid = asteroid->getNode()->getAttachedObject(0)->getBoundingBox();

    transformBoundingBox (ship->getNode(), boxShip );
    transformBoundingBox (asteroid->getNode(), boxAsteroid );

    //check if objects are inside the playing field
    if (!boxAsteroid.intersects(mBounds)) return;
    if (!boxShip.intersects(mBounds)) return;

    if (boxShip.intersects( boxAsteroid ))
    {
        mAudio->play("explode");
        asteroid->kill();
        ship->damage(100);
    }
}
//--------------------------------------------------------------------
void PlayStage::collisionPlayerPowerUp  (PlayerShip* ship, PowerUp* powerUp)
{
    AxisAlignedBox boxShip = ship->getNode()->getAttachedObject(0)->getBoundingBox();
    AxisAlignedBox boxPowerUp = powerUp->getNode()->getAttachedObject(0)->getBoundingBox();

    transformBoundingBox (powerUp->getNode(), boxPowerUp );
    transformBoundingBox (ship->getNode(), boxShip );

    //check if objects are inside the playing field
    if (!boxPowerUp.intersects(mBounds)) return;
    if (!boxShip.intersects(mBounds)) return;

    if (boxShip.intersects( boxPowerUp ))
    {
        powerUp->acquired();
        mScore += 100;

        Weapon::Params paramsWeapon;
        Weapon* weapon = mPlayer->getWeapon();

        getWeaponParams(powerUp->getWeapon(), paramsWeapon);
        weapon->setParams(paramsWeapon);

        mAudio->play("powerup");
    }
}
//--------------------------------------------------------------------
void PlayStage::updateStats()
{
    if (mGUILives) mGUILives->setCaption(StringConverter::toString(mPlayer->getLives()));
    if (mGUIHealth) mGUIHealth->setCaption(StringConverter::toString(mPlayer->getHP()));
    if (mGUIShields) mGUIShields->setCaption(StringConverter::toString(mPlayer->getShields()));
    if (mGUIScore) mGUIScore->setCaption(StringConverter::toString(mScore));
}
//--------------------------------------------------------------------
void PlayStage::transformBoundingBox (SceneNode* node, AxisAlignedBox& aabb)
{
    Matrix4 mat;
    mat.makeTransform(node->getPosition(), node->getScale(), node->getOrientation());
    aabb.transform(mat);
}
//--------------------------------------------------------------------
void PlayStage::processMessages()
{
    MessageElem msg;

    while (mMsgIn.getMessage(msg))
    {
        if (msg.op == "spawn") // string p1, double p2
        {
            string     name     = msg.p1.getString();
            double     posX     = msg.p2.getDouble();
            double     speed    = GLOBALS->getDouble(name + "/dSpeedZ");
            int        hp       = GLOBALS->getInt(name + "/nHP");
            SceneNode* node     = 0;
            string     meshName = GLOBALS->getString( name + "/szModel" );
            EnemyShip* enemy    = mGameManager->createEnemyShip(meshName);

            node = enemy->getNode();
            node->setPosition(posX, 0, getSpawnPtZ());
            enemy->setMovement (new MoveStraight(node, Vector3(0,0,speed)));
            enemy->setIsBoss( GLOBALS->getInt( name + "/nIsBoss" ) == 1 ? true : false);

            Weapon::Params w;
            Projectile::Params p;

            p.damage = GLOBALS->getInt( name + "/weap/nDamage" );
            p.model =  GLOBALS->getString( name + "/weap/szModel" );;
            p.velocity.z = -125;     
            p.origin.z = -10;
            p.fromPlayer = false;

            w.fireDelay = GLOBALS->getDouble( name + "/weap/dDelay" );
            w.model = "";
            w.numAmmo = -1;
            w.projectiles.push_back (p);
            enemy->getWeapon()->setParams(w);


            if (hp != 0) enemy->setHP(hp);

            //reuse msg
            msg.op = "spawnRet";
            msg.p1.set (enemy->getName());
            mMsgOut.addMessage(msg);
        }
        else if (msg.op == "spawn_a") // string p1, double p2
        {
            string     name     = msg.p1.getString();
            double     posX     = msg.p2.getDouble();
            int        hp       = GLOBALS->getInt(name + "/nHP");
            SceneNode* node = 0;
            Asteroid*  asteroid = 0;
            string     meshName = GLOBALS->getString( name + "/szModel" );
            string     matName  = GLOBALS->getString( name + "/szMaterial" );
            double     speed    = GLOBALS->getDouble( name + "/dSpeedZ" );
            
            asteroid = mGameManager->createAsteroid(meshName);

            if (!matName.empty())
                asteroid->getEntity()->setMaterialName( matName );

            node = asteroid->getNode();
            node->setPosition(posX, 0, getSpawnPtZ());

            Radian spin(Math::PI / 4.0);
            Vector3 velocity (0,0,speed);
            Vector3 axis ((rand()%10 - 5),(rand()%10 - 5),(rand()%10 - 5));
            axis.normalise();

            asteroid->setMovement( new MoveStraightSpin (node,velocity,spin,axis) );

            if (hp != 0)
                asteroid->setHP(hp);

            //reuse msg
            msg.op = "spawn_aRet";
            msg.p1.set (asteroid->getName());
            mMsgOut.addMessage(msg);
        }
        else if (msg.op == "assign_pow") // string p1, string p2
        {
            string ast = msg.p1.getString();
            string pow = msg.p2.getString();

            Asteroid* p = mGameManager->getAsteroid(ast);

            if (p)
                p->setPowerUp(pow);
        }
        else if (msg.op == "moveto") // string p1, string p2, double p3
        {
            string name   = msg.p1.getString();
            string strPos = msg.p2.getString();
            double speed  = msg.p3.getDouble();
            double x, y, z;
            EnemyShip* ship = mGameManager->getEnemyShip(name);
            bool parseOK = parseDouble3( strPos, x, y, z );

            if (ship && parseOK)
                ship->setMovement( new MoveToDest(ship->getNode(), Vector3(x,y,z),  speed));
        }
    }
}
