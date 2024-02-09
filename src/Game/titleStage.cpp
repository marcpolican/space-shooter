#include "pch.h"
#include "stages.h"
#include "overlaybutton.h"
#include "stagemanager.h"
#include "audio.h"

using namespace PewPew;

TitleStage::TitleStage() :
    mCursor(0),
    mIntroSoundTime(0),
    mIsFirstRun(true),
    mMouseWasDown(false)
{
}
//--------------------------------------------------------------------
TitleStage::~TitleStage()
{
}
//--------------------------------------------------------------------
bool TitleStage::update (double timeElapsed)
{
    OIS::MouseState ms = mMouse->getMouseState();

    double mouseX = ms.X.abs / (float) ms.width;
    double mouseY = ms.Y.abs / (float) ms.height;

    if (mIntroSoundTime > 0)
    {
        mIntroSoundTime -= timeElapsed;
        if (mIntroSoundTime <= 0)
            mAudio->play("zap");
    }

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

    if (mButtons[BTN_EXIT].isClicked())
    {
        mAudio->play("click");
        Sleep(100); //I just want to hear the sound play :p
        return false;
    }

    if (mButtons[BTN_CREDITS].isClicked())
    {
        mAudio->play("click");
        mStageMgr->setCurrentStage("creditsStage");
    }

    if (mButtons[BTN_START].isClicked())
    {
        mStageMgr->setCurrentStage("playStage");
        mAudio->play("click");
    }

    return true;
}
//--------------------------------------------------------------------
void TitleStage::enter()
{
    OverlayManager::getSingletonPtr()->getByName("PewPew/TitleScreen/Overlay")->show();

	mButtons[BTN_START].loadButton("PewPew/TitleScreen/BtnStart");
    mButtons[BTN_CREDITS].loadButton("PewPew/MenuState/BtnCredits");
	mButtons[BTN_EXIT].loadButton("PewPew/MenuState/BtnExit");

	//init cursor overlay
    OverlayManager::getSingletonPtr()->getByName("PewPew/Cursor/Overlay")->show();
	mCursor = OverlayManager::getSingletonPtr()->getOverlayElement("PewPew/Cursor");
	mCursor->setPosition(0.5, 0.5);

    OIS::MouseState ms = mMouse->getMouseState();
    if (ms.buttonDown(OIS::MB_Left))
        mMouseWasDown = true;
    else
        mMouseWasDown = false;

    mAudio->play("titlemusic",true);

    if (mIsFirstRun)
    {
        mIntroSoundTime = 0.2;
        mAudio->play("zap");
    }

    mIsFirstRun = false;
}
//--------------------------------------------------------------------
void TitleStage::exit()
{
    OverlayManager::getSingletonPtr()->getByName("PewPew/TitleScreen/Overlay")->hide();
    OverlayManager::getSingletonPtr()->getByName("PewPew/Cursor/Overlay")->hide();
    mAudio->stop("titlemusic");
}
