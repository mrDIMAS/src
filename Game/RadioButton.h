#pragma once

#include "Game.h"

class RadioButton {
private:
	bool mOn;
	shared_ptr<ruRect> mCheck;
	shared_ptr<ruButton> mGUIButton;
	void SelectCheckTexture();
	void OnChange();
public:
	explicit RadioButton(const shared_ptr<ruGUIScene> & scene, float x, float y, shared_ptr<ruTexture> buttonImage, const string & text);
	bool IsChecked();
	void SetEnabled(bool state);
	void AttachTo(shared_ptr<ruGUINode> node);
	void SetChangeAction(const ruDelegate & delegat);
};