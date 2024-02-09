#include "pch.h"
#include "stages.h"
#include "overlaybutton.h"
#include "stagemanager.h"
#include "audio.h"

using namespace PewPew;

CreditsStage::CreditsStage() :
    mCursor(0)
{
}
//--------------------------------------------------------------------
CreditsStage::~CreditsStage()
{
}
//--------------------------------------------------------------------
bool CreditsStage::update (double timeElapsed)
{
    OIS::MouseState ms = mMouse->getMouseState();

    double mouseX = ms.X.abs / (float) ms.width;
    double mouseY = ms.Y.abs / (float) ms.height;

    mCursor->setPosition(mouseX, mouseY);

    if (ms.buttonDown(OIS::MB_Left) ||
        ms.buttonDown(OIS::MB_Right) ||
        ms.buttonDown(OIS::MB_Middle) ||
        mKeyboard->isKeyDown (OIS::KC_ESCAPE))
    {
        mAudio->play("click");
        mStageMgr->setCurrentStage("titleStage");
    }

    return true;
}
//--------------------------------------------------------------------
void CreditsStage::enter()
{
    OverlayManager::getSingletonPtr()->getByName("PewPew/Credits/Overlay")->show();
    OverlayManager::getSingletonPtr()->getByName("PewPew/Cursor/Overlay")->show();

    mCursor = OverlayManager::getSingletonPtr()->getOverlayElement("PewPew/Cursor");
    mAudio->play("titlemusic",true);
}
//--------------------------------------------------------------------
void CreditsStage::exit()
{
    OverlayManager::getSingletonPtr()->getByName("PewPew/Credits/Overlay")->hide();
    mAudio->stop("titlemusic");
}
