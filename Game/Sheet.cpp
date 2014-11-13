#include "Sheet.h"
#include "GUI.h"

vector<Sheet*> Sheet::sheets;
SoundHandle Sheet::paperFlip;
FontHandle Sheet::sheetFont;

Sheet::Sheet( NodeHandle object, string desc, string text ) : InteractiveObject( object ) {
    txt = text;
    this->desc = desc;
    noteTex = GetTexture( "data/textures/generic/note.jpg" );
    sheets.push_back( this );
    if( !paperFlip.IsValid() ) {
        paperFlip = CreateSound2D( "data/sounds/paperflip.ogg" );
    }

    if( !sheetFont.IsValid() ) {
        sheetFont = CreateGUIFont( 20, "Arial", 0, 0 );
    }
}

void Sheet::Draw( ) {
    int sw = GetResolutionWidth();
    int sh = GetResolutionHeight();

    int cx = sw / 2;
    int cy = sh / 2;

    int w = 400;
    int h = 600;

    DrawGUIRect( cx - w / 2, cy - h / 2, w, h, noteTex );
    DrawGUIText( txt.c_str(), cx - w / 2 + 20, cy - h / 2 + 20, w - 20, h - 20, sheetFont, Vector3( 0, 0, 0 ), 0, 255 );
}

Sheet * Sheet::GetSheetByObject( NodeHandle o ) {
    for( auto sh : sheets )
        if( sh->object == o ) {
            return sh;
        }

    return 0;
}

void Sheet::Update() {

}
