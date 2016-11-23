#pragma once

#include "Game.h"

class GameCamera {
public:
	shared_ptr<ICamera> mCamera;
	float quadAlpha;
	float quadAlphaTo;
	Vector3 fadeColor;
	shared_ptr<IRect> mFullscreenQuad;
public:
	explicit GameCamera(const shared_ptr<IGUIScene> & scene, float fov = 65);
	virtual ~GameCamera();
	void FadeOut();
	void FadeIn();
	void FadePercent(int percent);
	void SetFadeColor(Vector3 newFadeColor);
	void Update();
	void MakeCurrent();
	bool FadeComplete();
	static GameCamera * currentCamera;
};