#pragma once


#include "BitmapFont.h"

class Texture;

class GUINode {
protected:
	float mGlobalX;
	float mGlobalY;
    float mX;
    float mY;
    float mWidth;
    float mHeight;
    int mAlpha;
    bool mVisible;
	bool mControlChildAlpha;
	bool mLastMouseInside;
    ruVector3 mColor;
    Texture * mpTexture;
    int mColorPacked;
	vector<GUINode*> mChildList;
	GUINode * mParent;
	unordered_map<ruGUIAction, ruEvent> mEventList;
	bool IsGotAction( ruGUIAction act );
	bool IsMouseInside();	
	virtual void OnClick();
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();	
public:
	void DoActions();
    static vector<GUINode*> msNodeList;
    explicit GUINode();
    virtual ~GUINode();
    void PackColor();
    void SetColor( ruVector3 color );
    virtual void SetAlpha( int alpha );
    float GetX();
    float GetY();
    float GetWidth();
    float GetHeight();
    void SetSize( float w, float h );
    virtual void SetVisible( bool visible );
    bool IsVisible();
    Texture * GetTexture();
    void SetTexture( Texture * pTexture );
    int GetPackedColor();
    virtual void SetPosition( float x, float y );
    ruVector2 GetPosition( );
    ruVector2 GetSize( );
    ruVector3 GetColor();
    int GetAlpha();
	void AttachTo( GUINode * parent );
	void CalculateTransform();
	void SetControlChildAlpha( bool control );
	void AddAction( ruGUIAction act, const ruDelegate & delegat );
	void RemoveAction( ruGUIAction act );
	void RemoveAllActions();
};