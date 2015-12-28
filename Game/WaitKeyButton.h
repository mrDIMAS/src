#pragma once

#include "Game.h"
#include "GUIProperties.h"
#include <Windows.h>

class WaitKeyButton {
private:
    bool mGrabKey;
    ruInput::Key mSelectedKey;
	int mAnimCounter;
    string mDesc;
    shared_ptr<ruText> mGUIText;
    shared_ptr<ruButton> mGUIButton;
public:
	static vector<WaitKeyButton*> msWaitKeyList;
    explicit WaitKeyButton( float x, float y, shared_ptr<ruTexture> buttonImage, const string & text );
	~WaitKeyButton();
    ruInput::Key GetSelectedKey();
    void SetSelected( ruInput::Key i );
    void Update( );
    void SetVisible( bool state );
	void AttachTo( const shared_ptr<ruGUINode> & node );
	static void UpdateAll() {
		for( auto pWaitKey : msWaitKeyList ) {
			pWaitKey->Update();
		}
	}
};