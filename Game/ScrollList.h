#pragma once

#include "Game.h"

class ScrollList {
private:
    int mCurrentValue;
    vector<string> mValues;
    shared_ptr<ruButton> mGUIIncreaseButton;
    shared_ptr<ruButton> mGUIDecreaseButton;
    shared_ptr<ruText> mGUIText;
    shared_ptr<ruText> mGUIValueText;
public:
    explicit ScrollList( const shared_ptr<ruGUIScene> & scene, float x, float y, shared_ptr<ruTexture> buttonImage, const string & text );
    virtual ~ScrollList();
    void SetCurrentValue( int value );
	int GetValueCount() const {
		return mValues.size();
	}
    int GetCurrentValue();
    void AddValue( string val );
    void Update(  );
    void AttachTo( shared_ptr<ruGUINode> node );
};