#pragma once

#include "Game.h"
#include "GUIProperties.h"

// Performs animation of goal text and it's rendering

class Goal {
private:
	shared_ptr<ITimer> mWaitTimer;
	float mCurrentY;
	float mDestY;
	float mInitialY;
	float mWaitSec;
	float mAlpha;
	string mText;
	shared_ptr<IText> mGoalText;
public:
	explicit Goal(const shared_ptr<IGUIScene> & scene);
	virtual ~Goal();
	void SetText(string t);
	void AnimateAndRender();
	void SetVisible(bool state);
};