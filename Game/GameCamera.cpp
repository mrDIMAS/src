#include "Precompiled.h"

#include "GameCamera.h"

GameCamera * GameCamera::currentCamera = 0;

void GameCamera::Update() {
	if( currentCamera == this ) {
		mFullscreenQuad->SetVisible( true );
		mFullscreenQuad->SetAlpha( quadAlpha );
		quadAlpha += ( quadAlphaTo - quadAlpha ) * 0.15f;
	} else {
		mFullscreenQuad->SetVisible( false );
	}
}

void GameCamera::FadeIn() {
	quadAlphaTo = 0.0f;
}

void GameCamera::FadeOut() {
	quadAlphaTo = 255.0f;
}

GameCamera::GameCamera( float fov ) {
	mCamera = ruCamera::Create( fov );

	quadAlpha = 0.0f;
	quadAlphaTo = 0.0f;

	SetFadeColor( ruVector3( 0, 0, 0 ));

	MakeCurrent();

	mFullscreenQuad = ruRect::Create( 0, 0, ruEngine::GetResolutionWidth(), ruEngine::GetResolutionHeight(), ruTexture::Request( "data/textures/generic/black.jpg" ), fadeColor, quadAlpha );
	mFullscreenQuad->SetAlpha( 0 );
}

void GameCamera::MakeCurrent() {
	mCamera->SetActive();

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

GameCamera::~GameCamera() {
	mFullscreenQuad->Free();
}
