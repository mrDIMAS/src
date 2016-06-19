#pragma once

#include "Game.h"

class GameCamera {
public:
	shared_ptr<ruCamera> mCamera;
	float quadAlpha;
	float quadAlphaTo;
	ruVector3 fadeColor;
	shared_ptr<ruRect> mFullscreenQuad;
public:
	explicit GameCamera(const shared_ptr<ruGUIScene> & scene, float fov = 65);
	virtual ~GameCamera();
	void FadeOut();
	void FadeIn();
	void FadePercent(int percent);
	void SetFadeColor(ruVector3 newFadeColor);
	void Update();
	void MakeCurrent();
	bool FadeComplete();
	static GameCamera * currentCamera;
};