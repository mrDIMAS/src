#pragma once

#include "Game.h"

class GameCamera {
public:
    NodeHandle cameraNode;

    float quadAlpha;
    float quadAlphaTo;

    Vector3 fadeColor;

public:
    GameCamera( float fov = 65 );
    void FadeOut();
    void FadeIn();
    void FadePercent( int percent );
    void SetFadeColor( Vector3 newFadeColor );
    void Update();
    void MakeCurrent();
    bool FadeComplete();

    static GameCamera * currentCamera;
};