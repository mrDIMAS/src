#pragma once

#include "Game.h"

class GameCamera
{
public:
    ruNodeHandle mNode;

    float quadAlpha;
    float quadAlphaTo;

    ruVector3 fadeColor;

public:
    GameCamera( float fov = 65 );
    void FadeOut();
    void FadeIn();
    void FadePercent( int percent );
    void SetFadeColor( ruVector3 newFadeColor );
    void Update();
    void MakeCurrent();
    bool FadeComplete();

    static GameCamera * currentCamera;
};