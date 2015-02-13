#pragma once

#include "Game.h"

class ScrollList {
private:
    int mCurrentValue;
    vector<string> mValues;
    ruButtonHandle mGUIIncreaseButton;
    ruButtonHandle mGUIDecreaseButton;
    ruTextHandle mGUIText;
    ruTextHandle mGUIValueText;
public:
    explicit ScrollList( float x, float y, ruTextureHandle buttonImage, const string & text );
    virtual ~ScrollList();
    void SetCurrentValue( int value );
    int GetCurrentValue();
    void AddValue( string val );
    void Update(  );
    void SetVisible( bool state );
};