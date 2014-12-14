#pragma once

#include "GUINode.h"
#include "Vertex.h"

class GUIRect : public GUINode
{
public:
	static vector<GUIRect*> msRectList;

	explicit GUIRect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, ruVector3 theColor, int theAlpha, bool selfRegister );
	explicit GUIRect();
	virtual ~GUIRect();

	void GetSixVertices( Vertex2D * vertices )
	{
		vertices[ 0 ] = Vertex2D( mX, mY, 0, 0, 0, mColorPacked );
		vertices[ 1 ] = Vertex2D( mX + mWidth, mY, 0, 1, 0, mColorPacked );
		vertices[ 2 ] = Vertex2D( mX, mY + mHeight, 0, 0, 1, mColorPacked );
		vertices[ 3 ] = Vertex2D( mX + mWidth, mY, 0, 1, 0, mColorPacked );
		vertices[ 4 ] = Vertex2D( mX + mWidth, mY + mHeight, 0, 1, 1, mColorPacked );
		vertices[ 5 ] = Vertex2D( mX, mY + mHeight, 0, 0, 1, mColorPacked );
	}
};