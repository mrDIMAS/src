#pragma once

#include "Game.h"

class ScrollList {
private:
    int mCurrentValue;
    vector<string> mValues;
    shared_ptr<ruButton> mIncreaseButton;
    shared_ptr<ruButton> mDecreaseButton;
    shared_ptr<ruText> mText;
    shared_ptr<ruText> mValueText;
public:
	ruEvent OnChange;
    explicit ScrollList( const shared_ptr<ruGUIScene> & scene, float x, float y, shared_ptr<ruTexture> buttonImage, const string & text );
    virtual ~ScrollList();
    void SetCurrentValue( int value );
	int GetValueCount() const {
		return mValues.size();
	}
    int GetCurrentValue();
    void AddValue( string val );
	string GetValueString(int i) const {
		return mValues[i];
	}
    void Update(  );
    void AttachTo( shared_ptr<ruGUINode> node );
};