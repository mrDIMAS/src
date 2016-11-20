#pragma once

#include "Game.h"

class GUIProperties {
public:
	shared_ptr<ruFont> mFont;

	ruVector3 mBackColor;
	ruVector3 mForeColor;
	ruVector3 mNoticeColor;

	GUIProperties(unique_ptr<Game> & game);
};

