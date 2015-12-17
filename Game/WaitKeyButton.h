#pragma once

#include "Game.h"
#include "GUIProperties.h"
#include <Windows.h>

class WaitKeyButton {
private:
    bool mGrabKey;
    int mSelectedKey;
	int mAnimCounter;
    string mDesc;
    ruText * mGUIText;
    ruButton * mGUIButton;
public:
	static vector<WaitKeyButton*> msWaitKeyList;
    explicit WaitKeyButton( float x, float y, shared_ptr<ruTexture> buttonImage, const string & text );
	~WaitKeyButton();
    int GetSelectedKey();
    void SetSelected( int i );
    void Update( );
    void SetVisible( bool state );
	void AttachTo( ruGUINode * node );
	static void UpdateAll() {
		for( auto pWaitKey : msWaitKeyList ) {
			pWaitKey->Update();
		}
	}
};