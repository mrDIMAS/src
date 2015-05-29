#include "Precompiled.h"

#include "ScrollList.h"
#include "GUI.h"

void ScrollList::Update(  ) {
    if( mValues.size() ) {
        ruSetGUINodeText( mGUIValueText, mValues[ mCurrentValue ] );

        if( ruIsButtonHit( mGUIIncreaseButton ) )
            if( mCurrentValue < mValues.size() - 1 ) {
                mCurrentValue++;
            }

        if( ruIsButtonHit( mGUIDecreaseButton ) )
            if( mCurrentValue > 0 ) {
                mCurrentValue--;
            }
    }
}

void ScrollList::AddValue( string val ) {
    mValues.push_back( val );
}

int ScrollList::GetCurrentValue() {
    return mCurrentValue;
}

ScrollList::ScrollList( float x, float y, ruTextureHandle buttonImage, const string & text ) {
    mCurrentValue = 0;

    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    mGUIText = ruCreateGUIText( text, x, y + textHeight / 2, captionWidth, textHeight, pGUI->mFont, ruVector3( 255, 255, 255 ), 0 );
    mGUIValueText = ruCreateGUIText( "Value", x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 3.15f * buttonWidth, 16, pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
    mGUIIncreaseButton = ruCreateGUIButton( x + captionWidth + 4.5f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
    mGUIDecreaseButton = ruCreateGUIButton( x + captionWidth, y, buttonWidth, buttonHeight, buttonImage, "<", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
}

void ScrollList::SetCurrentValue( int value ) {
    if( value >= 0 && value < mValues.size() ) {
        mCurrentValue = value;
    }
}

ScrollList::~ScrollList() {
    ruFreeGUINode( mGUIValueText );
    ruFreeGUINode( mGUIText );
    ruFreeGUINode( mGUIIncreaseButton );
    ruFreeGUINode( mGUIDecreaseButton );
}
