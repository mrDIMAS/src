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
    mGUIText = ruCreateGUIText( mText, cx - w / 2 + 20, cy - h / 2 + 20, w - 40, h - 40, msSheetFont, ruVector3( 0, 0, 0 ), 0, 255 );
    SetVisible( false );
}

void Sheet::Draw( ) {

}

Sheet * Sheet::GetSheetPointerByNode( ruNodeHandle o ) {
    for( auto sh : msSheetList )
        if( sh->mObject == o ) {
            return sh;
        }

    return 0;
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
    ruSetGUINodeText( mGUIText, text );
}

Sheet::~Sheet() {
    ruFreeGUINode(mGUIBackground);
    ruFreeGUINode(mGUIText);
    msSheetList.erase( find( msSheetList.begin(), msSheetList.end(), this ));
}
