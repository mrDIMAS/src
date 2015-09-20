#pragma once

#include "Game.h"
#include "GUI.h"
#include "SmoothFloat.h"
#include "SaveFile.h"

class Tip {
private:
    SmoothFloat mAlpha;
    int mX;
    SmoothFloat mY;
	int mWidth;
	int mHeight;
    ruTimerHandle mTimer;
    ruTextHandle mGUIText;
public:
    explicit Tip();
	virtual ~Tip();
	void SetVisible( bool state );
    void SetNewText( string text );
    void AnimateAndDraw();
    void Serialize( SaveFile & out );
    void Deserialize( SaveFile & in );
};