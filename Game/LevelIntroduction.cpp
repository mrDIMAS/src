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

    mGUIText->SetAlpha( mTextAlpha );
    mGUISkipText->SetVisible( mShowIntro );

    if( mShowIntro == false ) {
        if( mTextAlpha < 5.0f ) {
            Level::Change( LevelName::L1Arrival );
        }
    }

    DoneInitialization();
}

LevelIntroduction::~LevelIntroduction() {
    mGUIText->Free();
	mGUISkipText->Free();
	mGUIBackground->SetVisible( false );
	mGUIBackground->Free();
}

LevelIntroduction::LevelIntroduction() {
    CreateBlankScene();
    mTextAlpha = 0.0f;
    mTextAlphaTo = 255.0f;
    mShowIntro = true;
    mTypeNum = 1;
    LoadLocalization( "intro.loc" );
    int scx = ruEngine::GetResolutionWidth() / 2;
    int scy = ruEngine::GetResolutionHeight() / 2;
    int w = 600;
    int h = 400;
    mGUIText = ruText::Create( mLocalization.GetString( "intro" ), scx - w / 2, scy - h / 2, w, h, pGUIProp->mFont, ruVector3( 0, 0, 0 ), ruTextAlignment::Left, mTextAlpha );
    mGUISkipText = ruText::Create( mLocalization.GetString( "skip" ), ruEngine::GetResolutionWidth() / 2 - 256, ruEngine::GetResolutionHeight() - 200, 512, 128, pGUIProp->mFont, ruVector3( 255, 0, 0 ), ruTextAlignment::Center );
	mGUIBackground = ruRect::Create( 0, 0, ruEngine::GetResolutionWidth(), ruEngine::GetResolutionHeight(),  ruTexture::Request( "data/textures/generic/loadingScreen.jpg" ));
}

void LevelIntroduction::OnDeserialize( SaveFile & in ) {

}

void LevelIntroduction::OnSerialize( SaveFile & out ) {

}
