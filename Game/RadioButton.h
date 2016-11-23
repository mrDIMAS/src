#pragma once

#include "Game.h"

class RadioButton {
private:
	bool mOn;
	shared_ptr<IRect> mCheck;
	shared_ptr<IButton> mGUIButton;
	void SelectCheckTexture();
	void OnChange();
public:
	explicit RadioButton(const shared_ptr<IGUIScene> & scene, float x, float y, shared_ptr<ITexture> buttonImage, const string & text);
	bool IsChecked();
	void SetEnabled(bool state);
	void AttachTo(shared_ptr<IGUINode> node);
	void SetChangeAction(const Delegate & delegat);
};