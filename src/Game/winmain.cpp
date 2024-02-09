#include "pch.h"
#include "stagemanager.h"
#include "globalvars.h"

using namespace PewPew;

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
    // Create application object
    StageManager app;
    PlayStage    playStage;
    TitleStage   titleStage;
    CreditsStage creditsStage;

    try
    {
        if (!app.init())
            throw Ogre::Exception(0,"app.init() failed", __FILE__);

        app.addStage ("playStage", &playStage);
        app.addStage ("titleStage", &titleStage);
        app.addStage ("creditsStage", &creditsStage);
        app.setCurrentStage ("titleStage");

        app.start();
    }
    catch(...) //( Ogre::Exception& e )
    {
        //MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }

    GlobalVars::release();
    return 0;
}

