#include "GUIButton.h"

vector< GUIButton* > GUIButton::msButtonList;

void GUIButton::Update()
{
	int mouseX = ruGetMouseX();
	int mouseY = ruGetMouseY();

	mpText->SetColor( ruVector3( 255, 255, 255 ));

	mPicked = false;
	mLeftPressed = false;
	mRightPressed = false;

	if( mouseX > mX && mouseX < ( mX + mWidth ) && mouseY > mY && mouseY < ( mY + mHeight ) )
	{
		mPicked = true;

		if( ruIsMouseHit( MB_Left ))
		{
			mLeftPressed = true;
		}

		if( ruIsMouseHit( MB_Right ))
		{
			mRightPressed = true;
		}

		mpText->SetColor( ruVector3( 255, 0, 0 ));
	}
}

GUIButton::GUIButton( int x, int y, int w, int h, Texture* texture, const char * text, BitmapFont * font, ruVector3 color, int textAlign, int alpha )
	: GUIRect( x, y, w, h, texture, color, alpha, true )
{
	mpText = new GUIText( text, x, y, w, h, color, alpha, textAlign, font );
	msButtonList.push_back( this );
	mLeftPressed = false;
	mRightPressed = false;
	mPicked = false;
	pickedColor = ruVector3( 255, 255, 255 );
}

void GUIButton::SetPickedColor( ruVector3 val )
{
	pickedColor = val;
}

ruVector3 GUIButton::GetPickedColor() const
{
	return pickedColor;
}

bool GUIButton::IsLMBPressed()
{
	return mLeftPressed;
}

bool GUIButton::IsPicked()
{
	return mPicked;
}
