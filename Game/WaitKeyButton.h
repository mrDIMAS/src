#pragma once


#include "GUIProperties.h"
#include <Windows.h>

class WaitKeyButton {
private:
	unique_ptr<Game> & mGame;
	bool mGrabKey;
	IInput::Key mSelectedKey;
	int mAnimCounter;
	string mDesc;
	shared_ptr<IText> mGUIText;
	shared_ptr<IButton> mGUIButton;
public:
	static vector<WaitKeyButton*> msWaitKeyList;
	explicit WaitKeyButton(unique_ptr<Game> & game, const shared_ptr<IGUIScene> & scene, float x, float y, shared_ptr<ITexture> buttonImage, const string & text);
	~WaitKeyButton();
	IInput::Key GetSelectedKey();
	void SetSelected(IInput::Key i);
	void Update();
	void SetVisible(bool state);
	void AttachTo(const shared_ptr<IGUINode> & node);
	static void UpdateAll()
	{
		for(auto pWaitKey : msWaitKeyList) {
			pWaitKey->Update();
		}
	}
};