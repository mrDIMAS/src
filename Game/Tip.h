#pragma once

#include "Game.h"
#include "GUI.h"
#include "SmoothFloat.h"
#include "TextFileStream.h"

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
    Tip();
	void SetVisible( bool state ) {
		ruSetGUINodeVisible( mGUIText, state );
	}
    void SetNewText( string text );
    void AnimateAndDraw();
    void Serialize( TextFileStream & out );
    void Deserialize( TextFileStream & in );
};