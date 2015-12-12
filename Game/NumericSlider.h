#pragma once

#include "Game.h"

class Slider {
private:
    float mValue;
    float mfMinimum, mfMaximum;
    float mfStep;
    ruButtonHandle mGUIIncreaseButton;
    ruButtonHandle mGUIDecreaseButton;
    ruTextHandle mGUIText;
    ruTextHandle mGUIValueText;
	void UpdateText();
	void OnIncreaseClick();
	void OnDecreaseClick();
public:
    explicit Slider( float x, float y, float minimum, float maximum, float step,  ruTextureHandle buttonImage, const string & text );
    float GetValue();
    void SetValue( float value );
    ~Slider();
    void AttachTo( ruGUINodeHandle node );
	void SetChangeAction( const ruDelegate & action );
	int GetWidth();
	int GetHeight();
};