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
public:
    explicit Slider( float x, float y, float minimum, float maximum, float step,  ruTextureHandle buttonImage, const string & text );
    void Update(  );
    float GetValue();
    void SetValue( float value );
    ~Slider();
    void AttachTo( ruGUINodeHandle node ) {
		ruAttachGUINode( mGUIIncreaseButton, node );
		ruAttachGUINode( mGUIDecreaseButton, node );
		ruAttachGUINode( mGUIText, node );
		ruAttachGUINode( mGUIValueText, node );
	}
};