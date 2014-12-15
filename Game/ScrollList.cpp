#include "ScrollList.h"
#include "GUI.h"

void ScrollList::Draw( float x, float y )
{
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    y = y + 1.5f * buttonHeight;
	ruSetGUINodePosition( mGUIIncreaseButton, x + captionWidth + 4.5f * buttonWidth, y );
	ruSetGUINodePosition( mGUIDecreaseButton, x + captionWidth * buttonWidth, y );
	ruSetGUINodePosition( mGUIText, x, y + textHeight / 2 );

    if( mValues.size() ) {
		ruSetGUINodeText( mGUIValueText, mValues[ mCurrentValue ].c_str() );
		ruSetGUINodePosition( mGUIValueText, x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2 );

        if( ruIsButtonHit( mGUIIncreaseButton ) )
            if( mCurrentValue < mValues.size() - 1 )
                mCurrentValue++;

        if( ruIsButtonHit( mGUIDecreaseButton ) )
            if( mCurrentValue > 0 )
                mCurrentValue--;
    }
}

void ScrollList::AddValue( string val )
{
    mValues.push_back( val );
}

int ScrollList::GetCurrentValue()
{
    return mCurrentValue;
}

ScrollList::ScrollList( ruTextureHandle buttonImage, const char * text )
{
    mCurrentValue = 0;

	float buttonWidth = 32;
	float buttonHeight = 32;

	int textHeight = 16;
	int captionWidth = 100;

	mGUIText = ruCreateGUIText( text, 0, 0, captionWidth, textHeight, pGUI->mFont, ruVector3( 255, 255, 255 ), 0 );
	mGUIValueText = ruCreateGUIText( "Value", 0, 0, 3.15f * buttonWidth, 16, pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
	mGUIIncreaseButton = ruCreateGUIButton( 0, 0, buttonWidth, buttonHeight, buttonImage, ">", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
	mGUIDecreaseButton = ruCreateGUIButton( 0, 0, buttonWidth, buttonHeight, buttonImage, ">", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
}

void ScrollList::SetCurrentValue( int value )
{
	if( value >= 0 && value < mValues.size() ) {
		mCurrentValue = value;
	}
}

ScrollList::~ScrollList()
{
	ruFreeGUINode( mGUIValueText );
	ruFreeGUINode( mGUIText );
	ruFreeGUINode( mGUIIncreaseButton );
	ruFreeGUINode( mGUIDecreaseButton );
}
