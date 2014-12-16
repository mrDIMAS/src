#pragma once

#include "Common.h"
#include "BitmapFont.h"

class GUINode {
protected:
    float mX;
    float mY;
    float mWidth;
    float mHeight;
    int mAlpha;
    bool mVisible;
    ruVector3 mColor;
    Texture * mpTexture;
    int mColorPacked;
public:
    static vector<GUINode*> msNodeList;
    explicit GUINode();
    virtual ~GUINode();
    void PackColor();
    void SetColor( ruVector3 color );
    void SetAlpha( int alpha );
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
};