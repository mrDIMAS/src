#pragma once

#include "Game.h"
#include "GUIProperties.h"
#include "SmoothFloat.h"
#include "SaveFile.h"

class Tip {
private:
	SmoothFloat mAlpha;
	int mX;
	SmoothFloat mY;
	int mWidth;
	int mHeight;
	shared_ptr<ruTimer> mTimer;
	shared_ptr<ruText> mGUIText;
public:
	explicit Tip(const shared_ptr<ruGUIScene> & scene);
	virtual ~Tip();
	void SetVisible(bool state);
	void SetNewText(string text);
	void AnimateAndDraw();
	//void Serialize( SaveFile & out );
	//void Deserialize( SaveFile & in );
};