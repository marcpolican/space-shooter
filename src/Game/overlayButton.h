#pragma once

#include <ogre.h>

using namespace Ogre;

namespace PewPew
{
    /// Handles 2D overlay button events
    class OverlayButton
    {
    public:
	    enum ButtonStates
	    {
		    BUTTON_UP,	BUTTON_DOWN, BUTTON_DISABLED,
	    };

	    OverlayButton(void);
	    ~OverlayButton(void);

	    void loadButton     (const string& name);
	    void mousePressed   (double x, double y);
	    void mouseReleased  (double x, double y);
	    void setEnabled     (bool bIsEnabled);
	    bool isClicked      (bool resetAfterUse = true);

	    void show();
	    void hide();

	    virtual void down();
	    virtual void up();
	    virtual void clicked();

        bool isDown() {return mState == BUTTON_DOWN;}

    protected:
	    bool            mWasMouseDown;
	    bool            mIsClicked;
	    bool            mIsEnabled;
	    ButtonStates    mState;
	    OverlayElement* mSpriteUp;
	    OverlayElement* mSpriteDown;

	    //overlay bounds
	    double mTop, mLeft, mBottom, mRight;
    };
}