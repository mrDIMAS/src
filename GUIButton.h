#pragma once

#include "GUIRect.h"
#include "GUIText.h"

class GUIButton : public GUIRect {
private:
    GUIText * mpText;
    bool mPicked;
    bool mLeftPressed;
    bool mRightPressed;
    bool mLeftHit;
    bool mRightHit;
    ruVector3 pickedColor;
public:
    static vector< GUIButton* > msButtonList;
    explicit GUIButton( int x, int y, int w, int h, Texture* texture, const char * text, BitmapFont * font, ruVector3 color, int textAlign, int alpha );
    virtual ~GUIButton();
    void Update(); // must be called inside ruInputUpdate
    bool IsPicked();
    bool IsLeftPressed();
    bool IsRightPressed();
    ruVector3 GetPickedColor() const;
    void SetPickedColor(ruVector3 val);
    bool IsRightHit() const;
    bool IsLeftHit() const;
    virtual void SetPosition( float x, float y );
    virtual void SetVisible( bool visible );
    GUIText * GetText();
};