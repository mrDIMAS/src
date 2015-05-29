#pragma once

#include "GUIRect.h"
#include "GUIText.h"

class GUIButton : public GUIRect {
private:    
    bool mPicked;
    bool mLeftPressed;
    bool mRightPressed;
    bool mLeftHit;
    bool mRightHit;
	bool mActive;
	ruVector3 mInitColor;
    ruVector3 pickedColor;
	GUIText * mpText;
	virtual void OnClick() {
		// action defined in Update()
	}
public:
    static vector< GUIButton* > msButtonList;
    explicit GUIButton( int x, int y, int w, int h, Texture* texture, const string & text, BitmapFont * font, ruVector3 color, int textAlign, int alpha );
    virtual ~GUIButton();
    void Update(); // must be called inside ruInputUpdate
    bool IsPicked();
    bool IsLeftPressed();
    bool IsRightPressed();
    ruVector3 GetPickedColor() const;
    void SetPickedColor(ruVector3 val);
    bool IsRightHit() const;
    bool IsLeftHit() const;
	virtual void SetAlpha( int alpha );
	void SetActive( bool state );
    GUIText * GetText();
};