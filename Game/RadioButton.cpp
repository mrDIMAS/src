#include "Precompiled.h"

#include "RadioButton.h"
#include "GUIProperties.h"
#include "Menu.h"

void RadioButton::Update(  ) {

}

RadioButton::RadioButton(const shared_ptr<ruGUIScene> & scene, float x, float y, shared_ptr<ruTexture> buttonImage, const string & text  ) {
    mOn = false;
    int textHeight = 16;
    float buttonWidth = 110;
    float buttonHeight = 32;
    mGUIButton = scene->CreateButton( x, y, buttonWidth, buttonHeight, buttonImage, text, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
	mCheck = scene->CreateRect( buttonWidth + 10, 6, 20, 20, ruTexture::Request( "data/gui/menu/checkbox_checked.tga" ), pGUIProp->mForeColor );
	mCheck->Attach( mGUIButton );
	mGUIButton->AddAction(ruGUIAction::OnClick, [this] { OnChange(); });
}

void RadioButton::SetEnabled( bool state ) {
    mOn = state;
	SelectCheckTexture();
}

bool RadioButton::IsChecked() {
    return mOn;
}

void RadioButton::SetChangeAction( const ruDelegate & delegat ) {
	mGUIButton->RemoveAllActions();
	mGUIButton->AddAction(ruGUIAction::OnClick, [this] { OnChange(); });
	mGUIButton->AddAction( ruGUIAction::OnClick, delegat );
}

void RadioButton::AttachTo( shared_ptr<ruGUINode> node ) {
	mGUIButton->Attach( node );
}

void RadioButton::SelectCheckTexture()
{
	if( mOn ) {
		mCheck->SetTexture( ruTexture::Request( "data/gui/menu/checkbox_checked.tga" ));
	} else {
		mCheck->SetTexture( ruTexture::Request( "data/gui/menu/checkbox.tga" ));
	}
}

void RadioButton::OnChange() {
	mOn = !mOn;
	SelectCheckTexture();
}
