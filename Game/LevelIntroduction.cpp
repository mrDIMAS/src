#include "LevelIntroduction.h"




void LevelIntroduction::Hide() {

}

void LevelIntroduction::Show() {

}

void LevelIntroduction::DoScenario() {
    if( intro ) {
        textAlphaTo = 255.0f;
    } else {
        textAlphaTo = 0.0f;
    }

    float fadeSpeed = 0.025f;

    if( !intro ) {
        fadeSpeed = 0.05f;
    }

    textAlpha += ( textAlphaTo - textAlpha ) * 0.025f;

    int scx = ruGetResolutionWidth() / 2;
    int scy = ruGetResolutionHeight() / 2;

    int w = 600;
    int h = 400;

    ruDrawGUIText( text.c_str(), scx - w / 2, scy - h / 2, w, h, gui->font, ruVector3( 255, 255, 255 ), 0, textAlpha );

    if( ruGetElapsedTimeInSeconds( textTimer ) >= 22 || ruIsKeyHit( KEY_Space ) ) {
        intro = false;
    }

    if( intro ) {
        ruDrawGUIText( "[Space] - пропустить", ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );
    }

    if( intro == false ) {
        if( textAlpha < 5.0f ) {
            Level::Change( LevelName::L1Arrival );
        }
    }
}

LevelIntroduction::~LevelIntroduction() {

}

LevelIntroduction::LevelIntroduction() {
    scene = ruCreateSceneNode();
    textAlpha = 0.0f;
    textAlphaTo = 255.0f;
    textTimer = ruCreateTimer( );
    intro = true;
	typeNum = 1;
    LoadLocalization( "intro.loc" );
    text = localization.GetString( "intro" );
}

void LevelIntroduction::OnDeserialize( TextFileStream & in )
{

}

void LevelIntroduction::OnSerialize( TextFileStream & out )
{

}
