#include "Precompiled.h"

#include "LevelIntroduction.h"

void LevelIntroduction::Hide() {

}

void LevelIntroduction::Show() {

}

void LevelIntroduction::DoScenario() {
    if( mShowIntro ) {
        mTextAlphaTo = 255.0f;
    } else {
        mTextAlphaTo = 0.0f;
    }

    float fadeSpeed = 0.025f;

    if( !mShowIntro ) {
        fadeSpeed = 0.05f;
    }

    mTextAlpha += ( mTextAlphaTo - mTextAlpha ) * 0.025f;

    if( ruIsKeyHit( KEY_Space ) ) {
        mShowIntro = false;
    }

    ruSetGUINodeAlpha( mGUIText, mTextAlpha );
    ruSetGUINodeVisible( mGUISkipText, mShowIntro );

    if( mShowIntro == false ) {
        if( mTextAlpha < 5.0f ) {
            Level::Change( LevelName::L1Arrival );
        }
    }

    DoneInitialization();
}

LevelIntroduction::~LevelIntroduction() {
    ruFreeGUINode( mGUIText );
	ruFreeGUINode( mGUISkipText );
	ruSetGUINodeVisible( mGUIBackground, false );
	ruFreeGUINode( mGUIBackground );
}

LevelIntroduction::LevelIntroduction() {
    CreateBlankScene();
    mTextAlpha = 0.0f;
    mTextAlphaTo = 255.0f;
    mShowIntro = true;
    mTypeNum = 1;
    LoadLocalization( "intro.loc" );
    int scx = ruGetResolutionWidth() / 2;
    int scy = ruGetResolutionHeight() / 2;
    int w = 600;
    int h = 400;
    mGUIText = ruCreateGUIText( mLocalization.GetString( "intro" ), scx - w / 2, scy - h / 2, w, h, pGUI->mFont, ruVector3( 0, 0, 0 ), 0, mTextAlpha );
    mGUISkipText = ruCreateGUIText( mLocalization.GetString( "skip" ), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 );
	mGUIBackground = ruCreateGUIRect( 0, 0, ruGetResolutionWidth(), ruGetResolutionHeight(),  ruGetTexture( "data/textures/generic/loadingScreen.jpg" ));
}

void LevelIntroduction::OnDeserialize( TextFileStream & in ) {

}

void LevelIntroduction::OnSerialize( TextFileStream & out ) {

}
