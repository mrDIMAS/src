#include "Precompiled.h"
#include "Sheet.h"
#include "GUIProperties.h"

vector<Sheet*> Sheet::msSheetList;
ruSound Sheet::msPaperFlipSound;
ruFont * Sheet::msSheetFont;

Sheet::Sheet( ruSceneNode * object, string desc, string text ) : InteractiveObject( object ), mText( text ), mDescription( desc ) {
    mBackgroundTexture = ruTexture::Request( "data/gui/sheet.tga" );
    msSheetList.push_back( this );
    if( !msPaperFlipSound.IsValid() ) {
        msPaperFlipSound = ruSound::Load2D( "data/sounds/paperflip.ogg" );
    }

    if( !msSheetFont ) {
        msSheetFont = ruFont::LoadFromFile( 16, "data/fonts/font1.otf" );
    }

    int cx = ruEngine::GetResolutionWidth() / 2;
    int cy = ruEngine::GetResolutionHeight() / 2;

    int w = 400;
    int h = 600;

    mGUIBackground = ruRect::Create( cx - w / 2, cy - h / 2, w, h, mBackgroundTexture, pGUIProp->mBackColor );
    mGUIText = ruText::Create( mText, cx - w / 2 + 20, cy - h / 2 + 20, w - 40, h - 40, msSheetFont, pGUIProp->mForeColor, ruTextAlignment::Left, 255 );
    SetVisible( false );
}

void Sheet::Draw( ) {

}

Sheet * Sheet::GetSheetPointerByNode( ruSceneNode * node ) {
    for( auto pSheet : msSheetList ) {
        if( pSheet->mObject == node ) {
            return pSheet;
        }
	}
    return nullptr;
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
    mGUIBackground->Free();
    mGUIText->Free();
    msSheetList.erase( find( msSheetList.begin(), msSheetList.end(), this ));
}

void Sheet::SetVisible( bool state ) {
	mGUIBackground->SetVisible( state );
	mGUIText->SetVisible( state );
}
