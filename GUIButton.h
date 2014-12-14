#pragma once

#include "GUIRect.h"
#include "GUIText.h"

class GUIButton : public GUIRect
{
private:
	GUIText * mpText;
	bool mPicked;
	bool mLeftPressed;
	bool mRightPressed;
	ruVector3 pickedColor;
public:
	static vector< GUIButton* > msButtonList;
	explicit GUIButton( int x, int y, int w, int h, Texture* texture, const char * text, BitmapFont * font, ruVector3 color, int textAlign, int alpha );
	void Update();
	bool IsPicked();
	bool IsLMBPressed();
	ruVector3 GetPickedColor() const;
	void SetPickedColor(ruVector3 val);
};