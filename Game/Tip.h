#pragma once

#include "Game.h"
#include "GUI.h"
#include "SmoothFloat.h"
#include "TextFileStream.h"

class Tip
{
private:
    string txt;
    SmoothFloat alpha;
    int x;
    SmoothFloat y;
    int w;
    int h;
    ruTimerHandle timer;
	ruTextHandle mGUIText;
public:
    Tip();
    void SetNewText( string text );
    void AnimateAndDraw();
    void Serialize( TextFileStream & out );
    void Deserialize( TextFileStream & in );
};