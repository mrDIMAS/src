#pragma once

#include "Game.h"
#include "GUI.h"
#include <Windows.h>

class WaitKeyButton {
private:
    bool mGrabKey;
    int mSelectedKey;
    string mDesc;
    ruTextHandle mGUIText;
    ruButtonHandle mGUIButton;
public:
    explicit WaitKeyButton( float x, float y, ruTextureHandle buttonImage, const string & text );
    int GetSelectedKey();
    void SetSelected( int i );
    void Update( );
    void SetVisible( bool state );
};