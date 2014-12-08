#pragma once

#include "Common.h"

class Cursor {
public:
    int w;
    int h;
    bool visible;
    Texture * tex;

    Cursor();
    void Setup( ruTextureHandle texture, int w, int h );
    void Show();
    void Hide();
};