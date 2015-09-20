#include "Precompiled.h"

#include "GameCamera.h"

GameCamera * GameCamera::currentCamera = 0;

void GameCamera::Update() {
    if( currentCamera == this ) {
        ruSetGUINodeVisible( mFullscreenQuad, true );
        ruSetGUINodeAlpha( mFullscreenQuad, quadAlpha );
        quadAlpha += ( quadAlphaTo - quadAlpha ) * 0.15f;
    } else {
        ruSetGUINodeVisible( mFullscreenQuad, false );
    }
}

void GameCamera::FadeIn() {
    quadAlphaTo = 0.0f;
}

void GameCamera::FadeOut() {
    quadAlphaTo = 255.0f;
}

GameCamera::GameCamera( float fov ) {
    mNode = ruCreateCamera( fov );

    quadAlpha = 0.0f;
    quadAlphaTo = 0.0f;

    SetFadeColor( ruVector3( 0, 0, 0 ));

    MakeCurrent();

    mFullscreenQuad = ruCreateGUIRect( 0, 0, ruEngine::GetResolutionWidth(), ruEngine::GetResolutionHeight(), ruGetTexture( "data/textures/generic/black.jpg" ), fadeColor, quadAlpha );
    ruSetGUINodeAlpha( mFullscreenQuad, 0 );
}

void GameCamera::MakeCurrent() {
    ruSetActiveCamera( mNode );

    currentCamera = this;
}

bool GameCamera::FadeComplete() {
    return abs( quadAlpha - quadAlphaTo ) < 1.5f;
}

void GameCamera::FadePercent( int percent ) {
    quadAlphaTo = 255 - (float)percent / 100.0f * 255.0f;
}

void GameCamera::SetFadeColor( ruVector3 newFadeColor ) {
    fadeColor = newFadeColor;
}

GameCamera::~GameCamera()
{
	mNode.Free();
	ruFreeGUINode( mFullscreenQuad );
}
