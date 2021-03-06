#include "Precompiled.h"

#include "GameCamera.h"

GameCamera * GameCamera::currentCamera = 0;

void GameCamera::Update()
{
	if(currentCamera == this) {
		quadAlpha += (quadAlphaTo - quadAlpha) * 0.15f;
	}

	if(mFullscreenQuad) {
		if(currentCamera == this) {
			mFullscreenQuad->SetVisible(true);
			mFullscreenQuad->SetAlpha(quadAlpha);
		} else {
			mFullscreenQuad->SetVisible(false);
		}
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

GameCamera::GameCamera(const shared_ptr<IGUIScene> & scene, float fov)
{
	mCamera = Game::Instance()->GetEngine()->GetSceneFactory()->CreateCamera(fov);

	quadAlpha = 0.0f;
	quadAlphaTo = 0.0f;

	SetFadeColor(Vector3(0, 0, 0));

	MakeCurrent();

	if(scene) {
		mFullscreenQuad = scene->CreateRect(0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, scene->GetEngine()->GetRenderer()->GetTexture("data/textures/generic/black.jpg"), fadeColor, quadAlpha);
		mFullscreenQuad->SetAlpha(0);
		mFullscreenQuad->SetIndependentAlpha(true);
		mFullscreenQuad->SetLayer(0xFFFF); // top most
	}
}

void GameCamera::MakeCurrent()
{
	mCamera->SetActive();
	currentCamera = this;
}

bool GameCamera::FadeComplete()
{
	return abs(quadAlpha - quadAlphaTo) < 1.5f;
}

void GameCamera::FadePercent(int percent)
{
	quadAlphaTo = 255 - (float)percent / 100.0f * 255.0f;
}

void GameCamera::SetFadeColor(Vector3 newFadeColor)
{
	fadeColor = newFadeColor;
}

GameCamera::~GameCamera()
{

}
