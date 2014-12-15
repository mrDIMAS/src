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
	mLeftHit = false;
	mRightHit = false;

	if( mouseX > mX && mouseX < ( mX + mWidth ) && mouseY > mY && mouseY < ( mY + mHeight ) )
	{
		mPicked = true;

		if( ruIsMouseHit( MB_Left ))
		{
			mLeftHit = true;
		}

		if( ruIsMouseHit( MB_Right ))
		{
			mRightHit = true;
		}

		if( ruIsMouseDown( MB_Left ))
		{
			mLeftPressed = true;
		}

		if( ruIsMouseDown( MB_Right ))
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
	mPicked = false;
	mLeftPressed = false;
	mRightPressed = false;
	mLeftHit = false;
	mRightHit = false;
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

bool GUIButton::IsLeftPressed()
{
	return mLeftPressed;
}

bool GUIButton::IsPicked()
{
	return mPicked;
}


bool GUIButton::IsLeftHit() const
{
	return mLeftHit;
}

bool GUIButton::IsRightHit() const
{
	return mRightHit;
}

bool GUIButton::IsRightPressed()
{
	return mRightPressed;
}

GUIText * GUIButton::GetText()
{
	return mpText;
}

GUIButton::~GUIButton()
{
	msButtonList.erase( find( msButtonList.begin(), msButtonList.end(), this ));
}
