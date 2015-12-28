#include "Precompiled.h"

#include "ScrollList.h"
#include "GUIProperties.h"

void ScrollList::Update(  ) {
    if( mValues.size() ) {
        mGUIValueText->SetText( mValues[ mCurrentValue ] );
        if( mGUIIncreaseButton->IsHit() )
            if( mCurrentValue < mValues.size() - 1 ) {
                mCurrentValue++;
            }

        if( mGUIDecreaseButton->IsHit() ) {
            if( mCurrentValue > 0 ) {
                mCurrentValue--;
            }
		}
    }
}

void ScrollList::AddValue( string val ) {
    mValues.push_back( val );
}

int ScrollList::GetCurrentValue() {
    return mCurrentValue;
}

ScrollList::ScrollList( float x, float y, shared_ptr<ruTexture> buttonImage, const string & text ) {
    mCurrentValue = 0;

    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    mGUIText = ruText::Create( text, x, y + textHeight / 2, captionWidth, textHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left );
    mGUIValueText = ruText::Create( "Value", x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 3.15f * buttonWidth, 16, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
    mGUIIncreaseButton = ruButton::Create( x + captionWidth + 4.5f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
    mGUIDecreaseButton = ruButton::Create( x + captionWidth, y, buttonWidth, buttonHeight, buttonImage, "<", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
}

void ScrollList::SetCurrentValue( int value ) {
    if( value >= 0 && value < mValues.size() ) {
        mCurrentValue = value;
    }
}

ScrollList::~ScrollList() {

}

void ScrollList::AttachTo( shared_ptr<ruGUINode> node ) {
	mGUIIncreaseButton->Attach( node );
	mGUIDecreaseButton->Attach( node );
	mGUIText->Attach( node );
	mGUIValueText->Attach( node );
}
