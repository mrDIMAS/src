#pragma once

#include "Game.h"

class Slider {
private:
	float mValue;
	float mfMinimum, mfMaximum;
	float mfStep;
	shared_ptr<IButton> mGUIIncreaseButton;
	shared_ptr<IButton> mGUIDecreaseButton;
	shared_ptr<IText> mGUIText;
	shared_ptr<IText> mGUIValueText;
	void UpdateText();
	void OnIncreaseClick();
	void OnDecreaseClick();
public:
	explicit Slider(const shared_ptr<IGUIScene> & scene, float x, float y, float minimum, float maximum, float step, shared_ptr<ITexture> buttonImage, const string & text);
	float GetValue();
	void SetValue(float value);
	~Slider();
	void AttachTo(shared_ptr<IGUINode> node);
	void SetChangeAction(const Delegate & action);
	int GetWidth();
	int GetHeight();
};