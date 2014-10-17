#include "GameCamera.h"

GameCamera * GameCamera::currentCamera = 0;

void GameCamera::Update() {
    if( currentCamera == this ) {
        DrawGUIRect( 0, 0, GetResolutionWidth(), GetResolutionHeight(), TextureHandle::Empty(), fadeColor, quadAlpha );

        quadAlpha += ( quadAlphaTo - quadAlpha ) * 0.15f;
    }
}

void GameCamera::FadeIn() {
    quadAlphaTo = 0.0f;
}

void GameCamera::FadeOut() {
    quadAlphaTo = 255.0f;
}

GameCamera::GameCamera( float fov ) {
    cameraNode = CreateCamera( fov );

    quadAlpha = 0.0f;
    quadAlphaTo = 0.0f;

    SetFadeColor( Vector3( 0, 0, 0 ));

    MakeCurrent();
}

void GameCamera::MakeCurrent() {
    SetCamera( cameraNode );

    currentCamera = this;
}

bool GameCamera::FadeComplete() {
    return abs( quadAlpha - quadAlphaTo ) < 1.5f;
}

void GameCamera::FadePercent( int percent ) {
    quadAlphaTo = 255 - (float)percent / 100.0f * 255.0f;
}

void GameCamera::SetFadeColor( Vector3 newFadeColor ) {
    fadeColor = newFadeColor;
}
