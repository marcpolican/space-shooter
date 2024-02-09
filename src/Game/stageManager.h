#ifndef _MANAGERS_H_
#define _MANAGERS_H_

#include "stages.h"
#include "gameobjects.h"

namespace PewPew
{
    class Audio;


    /// The starting point of the whole game. This class manages
    /// GameStages. It also handles the OGRE intialization.
    /// @remarks
    ///     Some of the code here is patterned from the 
    ///     ExampleApplication and ExampleFrameListener classes
    ///     that comes with OGRE.
    
    class StageManager : public FrameListener, WindowEventListener
    {
    public:
        StageManager();
        ~StageManager();
        bool init();
        void start();
        void addStage(const string& name, Stage* stage);
        void setCurrentStage(const string& name);

    protected:
        typedef map <string, Stage*> MStage;
        typedef vector <Stage*> VStage;

        Root*         mRoot;
        Camera*       mCamera;
        SceneManager* mSceneMgr;
        RenderWindow* mWindow;
        Overlay*      mDebugOverlay;
        Audio*        mAudio;

        OIS::InputManager* mInputManager;
        OIS::Mouse*        mMouse;
        OIS::Keyboard*     mKeyboard;

        MStage mStages;
        Stage* mCurrentStage;
        Stage* mNextStage;
        bool   mIsInitDone;
        bool   mStatsOn;

        ///solid, wireframe, points
        int    mSceneRenderMode; 

        /// just to stop toggles flipping too fast
        Real   mKeyDelay;        
        string mDebugText;
        string mResourcePath;

        unsigned int mNumScreenShots;

        virtual bool setup();
        virtual bool configure();
        virtual void chooseSceneManager();
        virtual void createCamera();
        virtual void createViewports();
        virtual void setupResources();
        virtual void createResourceListener();
        virtual void loadResources();
        virtual void createFrameListener();
        virtual void windowResized(RenderWindow* rw);
        virtual void windowClosed(RenderWindow* rw);
        virtual bool frameStarted(const FrameEvent& evt);
        virtual bool frameEnded(const FrameEvent& evt);
        virtual void updateStats(void);
    };
}


#endif