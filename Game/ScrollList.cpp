#include "Precompiled.h"

#include "ScrollList.h"
#include "GUIProperties.h"

void ScrollList::Update(  ) {
    if( mValues.size() ) {
        mValueText->SetText( mValues[ mCurrentValue ] );
    }
}

void ScrollList::AddValue( string val ) {
    mValues.push_back( val );
}

int ScrollList::GetCurrentValue() {
    return mCurrentValue;
}

ScrollList::ScrollList(const shared_ptr<ruGUIScene> & scene, float x, float y, shared_ptr<ruTexture> buttonImage, const string & text ) {
    mCurrentValue = 0;

    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    mText = scene->CreateText( text, x, y + textHeight / 2, captionWidth, textHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left );
    mValueText = scene->CreateText( " ", x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 3.15f * buttonWidth, 16, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
    mIncreaseButton = scene->CreateButton( x + captionWidth + 4.5f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
	mIncreaseButton->AddAction(ruGUIAction::OnClick, [this] {
		mCurrentValue++;
		if (mCurrentValue >= mValues.size()) {
			mCurrentValue = 0;
		}
		OnChange(); 		
	});
    mDecreaseButton = scene->CreateButton( x + captionWidth, y, buttonWidth, buttonHeight, buttonImage, "<", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
	mDecreaseButton->AddAction(ruGUIAction::OnClick, [this] { 
		mCurrentValue--; 
		if (mCurrentValue < 0) {
			mCurrentValue = mValues.size() - 1;
		}
		OnChange(); 		
	});
	
}

void ScrollList::SetCurrentValue( int value ) {
    if( value >= 0 && value < mValues.size() ) {
        mCurrentValue = value;
    }
}

ScrollList::~ScrollList() {

}

void ScrollList::AttachTo( shared_ptr<ruGUINode> node ) {
	mIncreaseButton->Attach( node );
	mDecreaseButton->Attach( node );
	mText->Attach( node );
	mValueText->Attach( node );
}
