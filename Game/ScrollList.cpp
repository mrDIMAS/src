#include "ScrollList.h"
#include "GUI.h"

void ScrollList::Draw( float x, float y, ruTextureHandle buttonImage, const char * text )
{
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    y = y + 1.5f * buttonHeight;
    ruGUIState increase = ruDrawGUIButton( x + captionWidth + 4.5f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
    ruGUIState decrease = ruDrawGUIButton( x + captionWidth, y, buttonWidth, buttonHeight, buttonImage, "<", pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );

    ruDrawGUIText( text, x, y + textHeight / 2, captionWidth, textHeight, pGUI->mFont, ruVector3( 0, 255, 0 ), 0 );

    if( mValues.size() ) {
        ruDrawGUIText( mValues[ mCurrentValue ].c_str(), x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 3.15f * buttonWidth, 16, pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );

        if( increase.mouseLeftClicked )
            if( mCurrentValue < mValues.size() - 1 )
                mCurrentValue++;

        if( decrease.mouseLeftClicked )
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

ScrollList::ScrollList()
{
    mCurrentValue = 0;
}

void ScrollList::SetCurrentValue( int value )
{
	if( value >= 0 && value < mValues.size() ) {
		mCurrentValue = value;
	}
}
