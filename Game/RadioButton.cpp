#include "Precompiled.h"

#include "RadioButton.h"
#include "GUIProperties.h"
#include "Menu.h"

void RadioButton::Update(  ) {

}

RadioButton::RadioButton( float x, float y, ruTextureHandle buttonImage, const string & text  ) {
    mOn = false;
    int textHeight = 16;
    float buttonWidth = 110;
    float buttonHeight = 32;
    mGUIButton = ruCreateGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, text, pGUIProp->mFont, pGUIProp->mForeColor, 1 );
	mCheck = ruCreateGUIRect( buttonWidth + 10, 6, 20, 20, ruGetTexture( "data/gui/menu/checkbox_checked.tga" ), pGUIProp->mForeColor );
	ruAttachGUINode( mCheck, mGUIButton );
	ruAddGUINodeAction( mGUIButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &RadioButton::OnChange ));
}

void RadioButton::SetEnabled( bool state ) {
    mOn = state;
}

bool RadioButton::IsEnabled() {
    return mOn;
}

void RadioButton::SetChangeAction( const ruDelegate & delegat ) {
	ruRemoveAllGUINodeActions( mGUIButton );
	ruAddGUINodeAction( mGUIButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &RadioButton::OnChange ));
	ruAddGUINodeAction( mGUIButton, ruGUIAction::OnClick, delegat );
}

void RadioButton::AttachTo( ruGUINodeHandle node ) {
	ruAttachGUINode( mGUIButton, node );
}

void RadioButton::OnChange() {
	mOn = !mOn;
	if( mOn ) {
		ruSetGUINodeTexture( mCheck, ruGetTexture( "data/gui/menu/checkbox_checked.tga" ) );
	} else {
		ruSetGUINodeTexture( mCheck, ruGetTexture( "data/gui/menu/checkbox.tga" ) );
	}
}
