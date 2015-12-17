#pragma once

#include "Game.h"

class Slider {
private:
    float mValue;
    float mfMinimum, mfMaximum;
    float mfStep;
    ruButton * mGUIIncreaseButton;
    ruButton * mGUIDecreaseButton;
    ruText * mGUIText;
    ruText * mGUIValueText;
	void UpdateText();
	void OnIncreaseClick();
	void OnDecreaseClick();
public:
    explicit Slider( float x, float y, float minimum, float maximum, float step,  shared_ptr<ruTexture> buttonImage, const string & text );
    float GetValue();
    void SetValue( float value );
    ~Slider();
    void AttachTo( ruGUINode * node );
	void SetChangeAction( const ruDelegate & action );
	int GetWidth();
	int GetHeight();
};