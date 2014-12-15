#include "GUINode.h"

vector<GUINode*> GUINode::msNodeList;

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
	msNodeList.push_back( this );
}

GUINode::~GUINode()
{
	msNodeList.erase( find( msNodeList.begin(), msNodeList.end(), this ));
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

int GUINode::GetAlpha()
{
	return mAlpha;
}

ruVector3 GUINode::GetColor()
{
	return mColor;
}

ruVector2 GUINode::GetSize()
{
	return ruVector2( mWidth, mHeight );
}

ruVector2 GUINode::GetPosition()
{
	return ruVector2( mX, mY );
}

void GUINode::SetPosition( float x, float y )
{
	mX = x;
	mY = y;
}

int GUINode::GetPackedColor()
{
	return mColorPacked;
}
