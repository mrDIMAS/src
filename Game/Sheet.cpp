#include "Sheet.h"
#include "GUI.h"

vector<Sheet*> Sheet::sheets;
ruSoundHandle Sheet::paperFlip;
ruFontHandle Sheet::sheetFont;

Sheet::Sheet( ruNodeHandle object, string desc, string text ) : InteractiveObject( object ) {
    txt = text;
    this->desc = desc;
    noteTex = ruGetTexture( "data/textures/generic/note.jpg" );
    sheets.push_back( this );
    if( !paperFlip.IsValid() ) {
        paperFlip = ruLoadSound2D( "data/sounds/paperflip.ogg" );
    }

    if( !sheetFont.IsValid() ) {
        sheetFont = ruCreateGUIFont( 16, "data/fonts/font1.otf", 0, 0 );
    }
}

void Sheet::Draw( ) {
    int sw = ruGetResolutionWidth();
    int sh = ruGetResolutionHeight();

    int cx = sw / 2;
    int cy = sh / 2;

    int w = 400;
    int h = 600;

    ruDrawGUIRect( cx - w / 2, cy - h / 2, w, h, noteTex );
    ruDrawGUIText( txt.c_str(), cx - w / 2 + 20, cy - h / 2 + 20, w - 40, h - 40, sheetFont, ruVector3( 0, 0, 0 ), 0, 255 );
}

Sheet * Sheet::GetSheetByObject( ruNodeHandle o ) {
    for( auto sh : sheets )
        if( sh->object == o ) {
            return sh;
        }

    return 0;
}

void Sheet::Update() {

}
