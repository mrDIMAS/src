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

void GUIRect::GetSixVertices( Vertex2D * vertices )
{
	vertices[ 0 ] = Vertex2D( mGlobalX, mGlobalY, 0, 0, 0, mColorPacked );
	vertices[ 1 ] = Vertex2D( mGlobalX + mWidth, mGlobalY, 0, 1, 0, mColorPacked );
	vertices[ 2 ] = Vertex2D( mGlobalX, mGlobalY + mHeight, 0, 0, 1, mColorPacked );
	vertices[ 3 ] = Vertex2D( mGlobalX + mWidth, mGlobalY, 0, 1, 0, mColorPacked );
	vertices[ 4 ] = Vertex2D( mGlobalX + mWidth, mGlobalY + mHeight, 0, 1, 1, mColorPacked );
	vertices[ 5 ] = Vertex2D( mGlobalX, mGlobalY + mHeight, 0, 0, 1, mColorPacked );
}
