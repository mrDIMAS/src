#include "GUIText.h"

vector<GUIText*> GUIText::msTextList;

GUIText::GUIText( const string & theText, float theX, float theY, float theWidth,
                  float theHeight, ruVector3 theColor, int theAlpha, int theTextAlign, BitmapFont * theFont ) {
    mX = theX;
    mY = theY;
    mWidth = theWidth;
    mHeight = theHeight;
    mText = theText;
    mFont = theFont;
    mTextAlign = theTextAlign;
    SetColor( theColor );
    SetAlpha( theAlpha );
    mVisible = true;
    msTextList.push_back( this );
}

GUIText::~GUIText() {
    msTextList.erase( find( msTextList.begin(), msTextList.end(), this ));
}

RECT GUIText::GetBoundingRect() {
    mRect.left = mX;
    mRect.top = mY;
    mRect.right = mX + mWidth;
    mRect.bottom = mY + mHeight;
    return mRect;
}

BitmapFont * GUIText::GetFont() {
    return mFont;
}

std::string GUIText::GetText() {
    return mText;
}

int GUIText::GetTextAlignment() {
    return mTextAlign;
}
