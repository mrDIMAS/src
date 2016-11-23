#pragma once

#include "Game.h"

class ScrollList {
private:
	int mCurrentValue;
	vector<string> mValues;
	shared_ptr<IButton> mIncreaseButton;
	shared_ptr<IButton> mDecreaseButton;
	shared_ptr<IText> mText;
	shared_ptr<IText> mValueText;
public:
	Event OnChange;
	explicit ScrollList(const shared_ptr<IGUIScene> & scene, float x, float y, shared_ptr<ITexture> buttonImage, const string & text);
	virtual ~ScrollList();
	void SetCurrentValue(int value);
	int GetValueCount() const;
	int GetCurrentValue();
	void AddValue(string val);
	string GetValueString(int i) const;
	const vector<string> & GetValues();
	void AttachTo(shared_ptr<IGUINode> node);
};