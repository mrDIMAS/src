#pragma once


#include "GUIProperties.h"
#include <Windows.h>

class WaitKeyButton {
private:
	unique_ptr<Game> & mGame;
	bool mGrabKey;
	ruInput::Key mSelectedKey;
	int mAnimCounter;
	string mDesc;
	shared_ptr<ruText> mGUIText;
	shared_ptr<ruButton> mGUIButton;
public:
	static vector<WaitKeyButton*> msWaitKeyList;
	explicit WaitKeyButton(unique_ptr<Game> & game, const shared_ptr<ruGUIScene> & scene, float x, float y, shared_ptr<ruTexture> buttonImage, const string & text);
	~WaitKeyButton();
	ruInput::Key GetSelectedKey();
	void SetSelected(ruInput::Key i);
	void Update();
	void SetVisible(bool state);
	void AttachTo(const shared_ptr<ruGUINode> & node);
	static void UpdateAll()
	{
		for(auto pWaitKey : msWaitKeyList) {
			pWaitKey->Update();
		}
	}
};