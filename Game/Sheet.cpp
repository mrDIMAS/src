#include "Precompiled.h"
#include "Sheet.h"
#include "GUIProperties.h"

shared_ptr<ruFont> Sheet::msSheetFont;

Sheet::Sheet(const shared_ptr<ruGUIScene> & scene, const shared_ptr<ruSceneNode> & object, const string & desc, const string & text) : InteractiveObject( object ), mText( text ), mDescription( desc ) {
    mBackgroundTexture = ruTexture::Request( "data/gui/sheet.tga" );

    mPaperFlipSound = ruSound::Load2D( "data/sounds/paperflip.ogg" );
    if( !msSheetFont ) {
        msSheetFont = ruFont::LoadFromFile( 16, "data/fonts/font1.otf" );
    }
    int cx = ruVirtualScreenWidth / 2;
    int cy = ruVirtualScreenHeight / 2;

    int w = 400;
    int h = 600;

    mGUIBackground = scene->CreateRect( cx - w / 2, cy - h / 2, w, h, mBackgroundTexture, pGUIProp->mBackColor );
    mGUIText = scene->CreateText( mText, cx - w / 2 + 20, cy - h / 2 + 20, w - 40, h - 40, msSheetFont, pGUIProp->mForeColor, ruTextAlignment::Left, 255 );
    SetVisible( false );
}

void Sheet::Update() {
}

const string & Sheet::GetDescription() const {
    return mDescription;
}

void Sheet::SetDescription( const string & description ) {
    mDescription = description;
}

const string & Sheet::GetText() const {
    return mText;
}

void Sheet::SetText( const string & text ) {
    mText = text;
    mGUIText->SetText( text );
}

Sheet::~Sheet() {

}

void Sheet::SetVisible( bool state ) {
	mGUIBackground->SetVisible( state );
	mGUIText->SetVisible( state );
	if( state ) {
		mObject->Hide();
	} else {
		mObject->Show();
	}
	mPaperFlipSound->Play();
}
