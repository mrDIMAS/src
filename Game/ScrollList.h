#pragma once

#include "Game.h"

class ScrollList {
private:
    int mCurrentValue;
    vector<string> mValues;
    ruButton * mGUIIncreaseButton;
    ruButton * mGUIDecreaseButton;
    ruText * mGUIText;
    ruText * mGUIValueText;
public:
    explicit ScrollList( float x, float y, shared_ptr<ruTexture> buttonImage, const string & text );
    virtual ~ScrollList();
    void SetCurrentValue( int value );
    int GetCurrentValue();
    void AddValue( string val );
    void Update(  );
    void AttachTo( ruGUINode * node );
};