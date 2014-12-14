#include "GUINode.h"

void GUINode::SetAlpha( int alpha )
{
	mAlpha = alpha;
	PackColor();
}

void GUINode::SetColor( ruVector3 color )
{
	mColor = color;
	PackColor();
}

void GUINode::PackColor()
{
	mColorPacked = D3DCOLOR_ARGB( mAlpha, (int)mColor.x, (int)mColor.y, (int)mColor.z );
}

GUINode::GUINode()
{
	mX = 0;
	mY = 0;
	mWidth = 0;
	mHeight = 0;
	mVisible = true;
	SetColor( ruVector3( 255, 255, 255 ));
	SetAlpha( 255 );
}

GUINode::~GUINode()
{

}

void GUINode::SetTexture( Texture * pTexture )
{
	mpTexture = pTexture;
}

Texture * GUINode::GetTexture()
{
	return mpTexture;
}

bool GUINode::IsVisible()
{
	return mVisible;
}

void GUINode::SetVisible( bool visible )
{
	mVisible = visible;
}

void GUINode::SetSize( float w, float h )
{
	mWidth = w;
	mHeight = h;
}

float GUINode::GetHeight()
{
	return mHeight;
}

float GUINode::GetWidth()
{
	return mWidth;
}

float GUINode::GetY()
{
	return mY;
}

float GUINode::GetX()
{
	return mX;
}
