#include "Precompiled.h"

#include "RadioButton.h"
#include "GUI.h"
#include "Menu.h"

void RadioButton::Update(  ) {
    ruSetGUINodeText( mGUIText, mOn ? pMainMenu->GetLocalization()->GetString( "toggleEnabled" ) : pMainMenu->GetLocalization()->GetString( "toggleDisabled" ) );
    if( ruIsButtonHit( mGUIButton )) {
        mOn = !mOn;
    }
    ruSetGUINodeColor( mGUIText,  mOn ? ruVector3( 0, 255, 0 ) : ruVector3( 255, 0, 0 ) );
}

RadioButton::RadioButton( float x, float y, ruTextureHandle buttonImage, const string & text  ) {
    mOn = false;
    int textHeight = 16;
    float buttonWidth = 110;
    float buttonHeight = 32;
    mGUIText = ruCreateGUIText( "text", x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, pGUI->mFont, ruVector3( 255, 0, 0 ), 0 );
    mGUIButton = ruCreateGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, text, pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
}

void RadioButton::SetEnabled( bool state ) {
    mOn = state;
}

bool RadioButton::IsEnabled() {
    return mOn;
}
