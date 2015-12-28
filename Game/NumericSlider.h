#pragma once

#include "Game.h"

class Slider {
private:
    float mValue;
    float mfMinimum, mfMaximum;
    float mfStep;
    shared_ptr<ruButton> mGUIIncreaseButton;
    shared_ptr<ruButton> mGUIDecreaseButton;
    shared_ptr<ruText> mGUIText;
    shared_ptr<ruText> mGUIValueText;
	void UpdateText();
	void OnIncreaseClick();
	void OnDecreaseClick();
public:
    explicit Slider( float x, float y, float minimum, float maximum, float step,  shared_ptr<ruTexture> buttonImage, const string & text );
    float GetValue();
    void SetValue( float value );
    ~Slider();
    void AttachTo( shared_ptr<ruGUINode> node );
	void SetChangeAction( const ruDelegate & action );
	int GetWidth();
	int GetHeight();
};