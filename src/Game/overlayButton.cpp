#include "pch.h"
#include "overlaybutton.h"

using namespace Ogre;
using namespace PewPew;

OverlayButton::OverlayButton(void)
{
	mSpriteUp = NULL;
	mSpriteDown = NULL;
	mWasMouseDown = false;
	mIsClicked = false;
	mIsEnabled = true;
}
//--------------------------------------------------------------------
OverlayButton::~OverlayButton(void)
{
}
//--------------------------------------------------------------------
//enable/disable button
void OverlayButton::setEnabled(bool bIsEnabled)
{
	mIsEnabled = bIsEnabled;
	if (!mIsEnabled)
	{
		mState = BUTTON_DISABLED;
	}
}
//--------------------------------------------------------------------
//loads button from an overlay element name
void OverlayButton::loadButton(const string& name)
{
	String buttonName;
	mState = OverlayButton::BUTTON_UP;

	buttonName = name;
	mSpriteUp = OverlayManager::getSingletonPtr()->getOverlayElement(buttonName);

	buttonName = name + "Down";
	mSpriteDown = OverlayManager::getSingletonPtr()->getOverlayElement(buttonName);

	mTop = mSpriteUp->getTop();
	mLeft = mSpriteUp->getLeft();
	mBottom = mTop +  mSpriteUp->getHeight();
	mRight = mLeft + mSpriteUp->getWidth();

	up();
}
//--------------------------------------------------------------------
//check if mouse press was inside button bounds
void OverlayButton::mousePressed(double x, double y)
{
	if (!mIsEnabled) return;

	//check if coords are inside button bounds
	if (mLeft <= x && x <= mRight && mTop <= y && y <= mBottom)
	{
		mState = BUTTON_DOWN;
		mWasMouseDown = true;
		down();
	}
}
//--------------------------------------------------------------------
//check if mouse release was inside button bounds
void OverlayButton::mouseReleased(double x, double y)
{
	if (!mIsEnabled) return;

	mState = BUTTON_UP;
	up();

	//check if coords are inside button bounds
	if (mLeft <= x && x <= mRight && mTop <= y && y <= mBottom)
	{
		if (mWasMouseDown)
		{
			mWasMouseDown = false;
			mIsClicked = true;
		}
	}
}
//--------------------------------------------------------------------
//show down state of the button
void OverlayButton::down()
{
	if (!mIsEnabled) return;

	if (mSpriteUp && mSpriteDown)	
	{
		mSpriteUp->hide();
		mSpriteDown->show();
	}
}
//--------------------------------------------------------------------
//show up state of the button
void OverlayButton::up()
{
	if (!mIsEnabled) return;

	if (mSpriteUp && mSpriteDown)
	{
		mSpriteDown->hide();
		mSpriteUp->show();
	}
}
//--------------------------------------------------------------------
//set mIsClicked member var
void OverlayButton::clicked()
{
	if (!mIsEnabled) return;

	up();
	mIsClicked = true;
}
//--------------------------------------------------------------------
//returns true if the button was clicked
bool OverlayButton::isClicked(bool resetAfterUse)
{
	if (!mIsEnabled) false;

	bool bTemp = mIsClicked;
	if (resetAfterUse)
		mIsClicked = false;
	return bTemp;
}
//--------------------------------------------------------------------
//shows the button
void OverlayButton::show()
{
	mIsEnabled = true;
	up();
}
//--------------------------------------------------------------------
//hides the button
void OverlayButton::hide()
{
	mIsEnabled = false;
	if (mSpriteDown)
		mSpriteDown->hide();

	if (mSpriteUp)
		mSpriteUp->hide();
}