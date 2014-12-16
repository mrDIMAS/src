#include "Sheet.h"
#include "GUI.h"

vector<Sheet*> Sheet::msSheetList;
ruSoundHandle Sheet::msPaperFlipSound;
ruFontHandle Sheet::msSheetFont;

Sheet::Sheet( ruNodeHandle object, string desc, string text ) : InteractiveObject( object ) {
    mText = text;
    this->mDescription = desc;
    mBackgroundTexture = ruGetTexture( "data/textures/generic/note.jpg" );
    msSheetList.push_back( this );
    if( !msPaperFlipSound.IsValid() ) {
        msPaperFlipSound = ruLoadSound2D( "data/sounds/paperflip.ogg" );
    }

    if( !msSheetFont.IsValid() ) {
        msSheetFont = ruCreateGUIFont( 16, "data/fonts/font1.otf", 0, 0 );
    }

    int sw = ruGetResolutionWidth();
    int sh = ruGetResolutionHeight();

    int cx = sw / 2;
    int cy = sh / 2;

    int w = 400;
    int h = 600;

    mGUIBackground = ruCreateGUIRect( cx - w / 2, cy - h / 2, w, h, mBackgroundTexture );
    mGUIText = ruCreateGUIText( mText.c_str(), cx - w / 2 + 20, cy - h / 2 + 20, w - 40, h - 40, msSheetFont, ruVector3( 0, 0, 0 ), 0, 255 );
    SetVisible( false );
}

void Sheet::Draw( ) {

}

Sheet * Sheet::GetSheetByObject( ruNodeHandle o ) {
    for( auto sh : msSheetList )
        if( sh->mObject == o ) {
            return sh;
        }

    return 0;
}

void Sheet::Update() {

}

const char * Sheet::GetDescription() const {
    return mDescription.c_str();
}

void Sheet::SetDescription( const char * description ) {
    mDescription = description;
}

const char * Sheet::GetText() const {
    return mText.c_str();
}

void Sheet::SetText( const char * text ) {
    mText = text;
    ruSetGUINodeText( mGUIText, text );
}

Sheet::~Sheet() {
    ruFreeGUINode(mGUIBackground);
    ruFreeGUINode(mGUIText);
    msSheetList.erase( find( msSheetList.begin(), msSheetList.end(), this ));
}
