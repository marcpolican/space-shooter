#include "pch.h"
#include "gameobjects.h"
#include "stagemanager.h"
#include "audio.h"
#include "globalvars.h"
#include "utils.h"

using namespace PewPew;
using namespace OIS;


/** Constructor */
StageManager::StageManager() :
    mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mKeyDelay(0),
    mStatsOn(false),
    mIsInitDone(false),
    mSceneRenderMode(0),
    mNumScreenShots(0),
    mDebugOverlay(0),
    mInputManager(0),
    mMouse(0),
    mKeyboard(0),
    mCurrentStage(0),
    mNextStage(0),
    mAudio(0)
{
}
//--------------------------------------------------------------------
/** Destructor */
StageManager::~StageManager()
{
    //Remove listeners
    WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    mRoot->removeFrameListener (this);

    if (mAudio) { delete mAudio; mAudio = 0; }
}
//--------------------------------------------------------------------
/** OGRE loading stuff */
bool StageManager::init()
{
    if (!setup())
        return false;

    mAudio = new Audio;
    if (!mAudio->init()) return false;
    if (!mAudio->loadSoundsXml("../media/sounds/sounds.xml")) return false;
    if (!GLOBALS->loadDataFromFile("../media/game/globals.xml")) return false;

    //load levels xml too
    string filename;
    int numLevels = GLOBALS->getInt("globals/nNumLevels");
    for (int i=0; i<numLevels; i++)
    {
        filename = GLOBALS->getString("globals/szLevel" + StringConverter::toString(i));
        GLOBALS->loadDataFromFile(filename);
    }

    mIsInitDone = true;
    return true;
}
//--------------------------------------------------------------------
/** Start the game */
void StageManager::start()
{
    if (!mIsInitDone) return;

    mRoot->startRendering();
}
//--------------------------------------------------------------------
/** @remarks replace the old one if it exists */
void StageManager::addStage(const string& name, Stage* stage)
{
    assert (stage);

    Stage::InitParams p;

    p.root     = mRoot;
    p.sceneMgr = mSceneMgr;
    p.mouse    = mMouse;
    p.keyboard = mKeyboard;
    p.audio    = mAudio;
    p.stageMgr = this;
    
    stage->stageInit(p);

    string tempName(name);
    strToLower(tempName);
    mStages[tempName] = stage;
}
//--------------------------------------------------------------------
void StageManager::setCurrentStage(const string& name)
{
    string tempName(name);
    strToLower(tempName);

    //do nothing if stage cannot be found.
    //use find() instead of [] so we dont create another element
    MStage::iterator iter = mStages.find(tempName);
    if (iter != mStages.end())
    {
        //if mCurrentStage is NULL, it means that no stage has
        //been set yet. Just set it to the correct pointer.
        //if mCurrentStage exists, store the pointer for the
        //next stage then set it on next update
        if (mCurrentStage == 0)
        {
            mCurrentStage = (*iter).second;
            mCurrentStage->enter();
        }
        else
        {
            mNextStage = (*iter).second;
        }
    }
}
//--------------------------------------------------------------------
/** Sets up the application - returns false if the user chooses
    to abandon configuration. 
    @remarks Copied from OGRE ExampleApplication.cpp, but modified
        a little to suit my needs.
*/
bool StageManager::setup()
{
	string pluginsPath;

	pluginsPath = mResourcePath + "plugins.cfg";

    mRoot = new Root(pluginsPath, mResourcePath + "ogre.cfg", mResourcePath + "Ogre.log");

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    TextureManager::getSingleton().setDefaultNumMipmaps(5);

    //for the loading screen
	createResourceListener();
	loadResources();

    createFrameListener();

    return true;
}
//--------------------------------------------------------------------
/** @remarks Copied from OGRE ExampleApplication.cpp, but modified
        a little to suit my needs.
*/
bool StageManager::configure()
{
    //TODO: improvements

    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true);
        // Let's add a nice window icon
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        HWND hwnd;
        mWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
        //LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON) );
        //SetClassLong( hwnd, GCL_HICON, iconID );
#endif
        return true;
    }
    else
    {
        return false;
    }
    return true;
}
//--------------------------------------------------------------------
/** Use the generic SceneManager
    @remarks Copied from OGRE ExampleApplication.cpp.
*/
void StageManager::chooseSceneManager()
{
    mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");
}
//--------------------------------------------------------------------
/** Create the game camera */
void StageManager::createCamera()
{
    mCamera = mSceneMgr->createCamera("MainCamera");
    mCamera->setNearClipDistance(5);
}
//--------------------------------------------------------------------
/** Create the viewport
    @remarks Copied from OGRE ExampleApplication.cpp
*/
void StageManager::createViewports()
{
    // Create viewport for the entire window
    Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));
    mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}
//--------------------------------------------------------------------
/** Load the locations of the resources
    @remarks Copied from OGRE ExampleApplication.cpp, but modified
        a little to suit my needs.
*/
void StageManager::setupResources()
{
    // Load resource paths from config file
    ConfigFile cf;
    cf.load(mResourcePath + "resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;

            ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
}
//--------------------------------------------------------------------
/** for the loading screen */
void StageManager::createResourceListener()
{
}
//--------------------------------------------------------------------
/** for the loading screen */
void StageManager::loadResources()
{
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//--------------------------------------------------------------------
/** Create the frame listener and input devices 
    @remarks Copied from OGRE ExampleFrameListener. 
        Changed a bit to suit my needs
*/
void StageManager::createFrameListener()
{
    mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");

    LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = InputManager::createInputSystem( pl );

    //Create all devices
    mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, false ));
    mMouse = static_cast<Mouse*>(mInputManager->createInputObject( OISMouse, false ));

    //Set initial mouse clipping size
    windowResized(mWindow);
    mDebugOverlay->hide();

    //Register listeners
    WindowEventUtilities::addWindowEventListener(mWindow, this);    
    mRoot->addFrameListener(this);
}
//--------------------------------------------------------------------
/** Adjust mouse clipping area 
    @remarks Copied from OGRE ExampleFrameListener. 
*/
void StageManager::windowResized(RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}
//--------------------------------------------------------------------
/** Unattach OIS before window shutdown 
    @remarks Copied from OGRE ExampleFrameListener. 
        Changed a bit to suit my needs
*/
void StageManager::windowClosed(RenderWindow* rw)
{
	//Only close for window that created OIS
	if( rw == mWindow )
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject( mMouse );
			mInputManager->destroyInputObject( mKeyboard );
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}
//--------------------------------------------------------------------
/** This function is called by OGRE before every frame
    is to be rendered.
*/
bool StageManager::frameStarted(const FrameEvent& evt)
{
    //limit to 60 fps;
    static double timeAccumulated = 0.0;
    static double timeTarget = 1 / 60.0;

    timeAccumulated += evt.timeSinceLastFrame;

    if (timeAccumulated < timeTarget) return true;
    if (!mIsInitDone) return true;
	if (mWindow->isClosed())	return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	//Check if one of the devices is not buffered
	if (!mMouse->buffered() || !mKeyboard->buffered())
	{
        if (mKeyDelay >= 0)
			mKeyDelay -= timeAccumulated;
	}

    //F1 key to show/hide the debug stats
    if (mKeyboard->isKeyDown(KC_F1) && mKeyDelay <=0)
    {
        mStatsOn = !mStatsOn;
        if (mStatsOn) mDebugOverlay->show();
        else          mDebugOverlay->hide();

        mKeyDelay = 0.5;
    }

    //F2 key to change render mode (i.e. points, wirefreame and solid)
    if(mKeyboard->isKeyDown(KC_F2) && mKeyDelay <=0)
    {
        mSceneRenderMode = (mSceneRenderMode+1)%3 ;
        switch(mSceneRenderMode) 
        {
            case 0 : mCamera->setPolygonMode(PM_SOLID); break;
            case 1 : mCamera->setPolygonMode(PM_WIREFRAME); break;
            case 2 : mCamera->setPolygonMode(PM_POINTS); break;
        }
        mKeyDelay = 0.5;
    }

    //PRTSC key to same a screen shot
	if(mKeyboard->isKeyDown(KC_SYSRQ) && mKeyDelay <= 0)
	{
		std::ostringstream ss;
		ss << "screenshot_" << ++mNumScreenShots << ".png";
		mWindow->writeContentsToFile(ss.str());
		mKeyDelay = 0.5;
		mDebugText = "Saved: " + ss.str();
	}

    mAudio->update();

    bool retVal = true;

    //check if there is a new stage to be set
    if (mNextStage)
    {
        mCurrentStage->exit();
        mCurrentStage = mNextStage;
        mCurrentStage->enter();
        mNextStage = 0;
    }

    if (mCurrentStage)
    {
        retVal = mCurrentStage->update(timeAccumulated);
    }

    //reset time;
    timeAccumulated = 0;

	return retVal;
}
//--------------------------------------------------------------------
/** This function is called by OGRE after every frame
    has finished rendering
*/
bool StageManager::frameEnded(const FrameEvent& evt)
{
    if (mCurrentStage)
        mCurrentStage->updateEnd();

	updateStats();
	return true;
}
//--------------------------------------------------------------------
/** Update the stats */
void StageManager::updateStats(void)
{
    if (!mStatsOn) return;

    static String currFps = "Current FPS: ";
    static String avgFps = "Average FPS: ";
    static String bestFps = "Best FPS: ";
    static String worstFps = "Worst FPS: ";
    static String tris = "Triangle Count: ";
    static String batches = "Batch Count: ";

    // update stats when necessary
    try
    {
        OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
        OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
        OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
        OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

        const RenderTarget::FrameStats& stats = mWindow->getStatistics();
        guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
        guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
        guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
	        +" "+StringConverter::toString(stats.bestFrameTime)+" ms");
        guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
	        +" "+StringConverter::toString(stats.worstFrameTime)+" ms");

        OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
        guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

        OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
        guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

        OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
        guiDbg->setCaption(mDebugText);
    }
    catch(...) 
    { /* ignore */ 
    }
}