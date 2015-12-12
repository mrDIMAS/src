#pragma once

#include "Game.h"
#include "GUIProperties.h"

// Performs animation of goal text and it's rendering

class Goal {
private:
    ruTimerHandle mWaitTimer;
    float mCurrentY;
    float mDestY;
    float mInitialY;
    float mWaitSec;
    float mAlpha;
    string mText;
    ruTextHandle mGoalText;
public:
    explicit Goal();
    virtual ~Goal();
    void SetText( string t );
    void AnimateAndRender();
    void SetVisible( bool state );
};