#include "Precompiled.h"

#include "GUIRect.h"
#include "GUIRenderer.h"

vector<GUIRect*> GUIRect::msRectList;

GUIRect::~GUIRect() {
	auto iter = find( msRectList.begin(), msRectList.end(), this );
	if( iter != msRectList.end() ) {
		msRectList.erase( iter );
	}
}

GUIRect::GUIRect() {

}

GUIRect::GUIRect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, ruVector3 theColor, int theAlpha, bool selfRegister ) {
    mX = theX;
    mY = theY;
    mWidth = theWidth;
    mHeight = theHeight;
    mpTexture = theTexture;
    mVisible = true;
    SetColor( theColor );
    SetAlpha( theAlpha );
    if( selfRegister ) {
        msRectList.push_back( this );
    }
}
