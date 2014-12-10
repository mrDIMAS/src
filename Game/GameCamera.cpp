#include "GameCamera.h"

GameCamera * GameCamera::currentCamera = 0;

void GameCamera::Update()
{
    if( currentCamera == this )
    {
        ruDrawGUIRect( 0, 0, ruGetResolutionWidth(), ruGetResolutionHeight(), ruTextureHandle::Empty(), fadeColor, quadAlpha );

        quadAlpha += ( quadAlphaTo - quadAlpha ) * 0.15f;
    }
}

void GameCamera::FadeIn()
{
    quadAlphaTo = 0.0f;
}

void GameCamera::FadeOut()
{
    quadAlphaTo = 255.0f;
}

GameCamera::GameCamera( float fov )
{
    mNode = ruCreateCamera( fov );

    quadAlpha = 0.0f;
    quadAlphaTo = 0.0f;

    SetFadeColor( ruVector3( 0, 0, 0 ));

    MakeCurrent();
}

void GameCamera::MakeCurrent()
{
    ruSetActiveCamera( mNode );

    currentCamera = this;
}

bool GameCamera::FadeComplete()
{
    return abs( quadAlpha - quadAlphaTo ) < 1.5f;
}

void GameCamera::FadePercent( int percent )
{
    quadAlphaTo = 255 - (float)percent / 100.0f * 255.0f;
}

void GameCamera::SetFadeColor( ruVector3 newFadeColor )
{
    fadeColor = newFadeColor;
}
