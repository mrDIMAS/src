#include "Precompiled.h"

#include "RadioButton.h"
#include "GUIProperties.h"
#include "Menu.h"

void RadioButton::Update(  ) {

}

RadioButton::RadioButton( float x, float y, shared_ptr<ruTexture> buttonImage, const string & text  ) {
    mOn = false;
    int textHeight = 16;
    float buttonWidth = 110;
    float buttonHeight = 32;
    mGUIButton = ruButton::Create( x, y, buttonWidth, buttonHeight, buttonImage, text, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
	mCheck = ruRect::Create( buttonWidth + 10, 6, 20, 20, ruTexture::Request( "data/gui/menu/checkbox_checked.tga" ), pGUIProp->mForeColor );
	mCheck->Attach( mGUIButton );
	mGUIButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &RadioButton::OnChange ));
}

void RadioButton::SetEnabled( bool state ) {
    mOn = state;
	SelectCheckTexture();
}

bool RadioButton::IsEnabled() {
    return mOn;
}

void RadioButton::SetChangeAction( const ruDelegate & delegat ) {
	mGUIButton->RemoveAllActions();
	mGUIButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &RadioButton::OnChange ));
	mGUIButton->AddAction( ruGUIAction::OnClick, delegat );
}

void RadioButton::AttachTo( shared_ptr<ruGUINode> node ) {
	mGUIButton->Attach( node );
}

void RadioButton::OnChange() {
	mOn = !mOn;
	SelectCheckTexture();
}
