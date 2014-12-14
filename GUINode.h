#pragma once

#include "Common.h"
#include "BitmapFont.h"

class GUINode
{
protected:
	float mX;
	float mY;
	float mWidth;
	float mHeight;
	int mAlpha;
	bool mVisible;
	ruVector3 mColor;
	Texture * mpTexture;
	int mColorPacked;
public:
	explicit GUINode();
	virtual ~GUINode();
	void PackColor();
	void SetColor( ruVector3 color );
	void SetAlpha( int alpha );
	float GetX();
	float GetY();
	float GetWidth();
	float GetHeight();
	void SetSize( float w, float h );
	void SetVisible( bool visible );
	bool IsVisible();
	Texture * GetTexture();
	void SetTexture( Texture * pTexture );
	int GetPackedColor()
	{
		return mColorPacked;
	}
	void SetPosition( float x, float y )
	{
		mX = x;
		mY = y;
	}
	ruVector2 GetPosition( )
	{
		return ruVector2( mX, mY );
	}
	ruVector2 GetSize( )
	{
		return ruVector2( mWidth, mHeight );
	}
	ruVector3 GetColor()
	{
		return mColor;
	}
	int GetAlpha()
	{
		return mAlpha;
	}
};