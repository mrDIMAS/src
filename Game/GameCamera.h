#pragma once

#include "Game.h"

class GameCamera {
public:
    shared_ptr<ruCamera> mCamera;
    float quadAlpha;
    float quadAlphaTo;
    ruVector3 fadeColor;
    ruRect * mFullscreenQuad;
public:
    explicit GameCamera( float fov = 65 );
	virtual ~GameCamera();
    void FadeOut();
    void FadeIn();
    void FadePercent( int percent );
    void SetFadeColor( ruVector3 newFadeColor );
    void Update();
    void MakeCurrent();
    bool FadeComplete();
    static GameCamera * currentCamera;
};