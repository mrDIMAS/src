#pragma once

#include "Game.h"

class GUIProperties {
public:
	shared_ptr<IFont> mFont;

	Vector3 mBackColor;
	Vector3 mForeColor;
	Vector3 mNoticeColor;

	GUIProperties(unique_ptr<Game> & game);
};

