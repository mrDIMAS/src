#pragma once

#include "GUINode.h"

class GUIText : public GUINode {
protected:
    BitmapFont * mFont;
    string mText;
    int mTextAlign;
    RECT mRect;
public:
    static vector<GUIText*> msTextList;

    explicit GUIText( const string & theText, float theX, float theY, float theWidth, float theHeight, ruVector3 theColor, int theAlpha, int theTextAlign, BitmapFont * theFont );
    virtual ~GUIText( );
    RECT GetBoundingRect();
    int GetTextAlignment();
    string & GetText();
    BitmapFont * GetFont();
    void SetText( const string & text );
};
