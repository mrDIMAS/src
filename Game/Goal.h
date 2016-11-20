#pragma once

#include "Game.h"
#include "GUIProperties.h"

// Performs animation of goal text and it's rendering

class Goal {
private:
	shared_ptr<ruTimer> mWaitTimer;
	float mCurrentY;
	float mDestY;
	float mInitialY;
	float mWaitSec;
	float mAlpha;
	string mText;
	shared_ptr<ruText> mGoalText;
public:
	explicit Goal(const shared_ptr<ruGUIScene> & scene);
	virtual ~Goal();
	void SetText(string t);
	void AnimateAndRender();
	void SetVisible(bool state);
};