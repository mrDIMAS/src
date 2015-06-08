#pragma once

#include "Shader.h"
#include "BitmapFont.h"
#include "GUIRect.h"
#include "GUIText.h"
#include "GUIButton.h"

class TextVertex {
private:
    ruVector3 mPosition;
    ruVector2 mTexCoord;
    DWORD mColor;
public:
	void Set( float posX, float posY, ruVector2 & texCoord, DWORD color );
};

struct TextQuad {
    TextVertex mVertices[ 4 ];
};

class Face {
private:
	unsigned short mIndex[6];
public:    
	void Set( unsigned short i1, unsigned short i2, unsigned short i3, unsigned short i4, unsigned short i5, unsigned short i6 );
};

class GUIRenderer : public RendererComponent {
private:
    IDirect3DVertexBuffer9 * mVertexBuffer;
    IDirect3DVertexDeclaration9 * mVertexDeclaration;
    int mSizeOfRectBytes;
    D3DXMATRIX mOrthoMatrix;
    void RenderRect( GUIRect * r );
    VertexShader * mVertexShader;
    PixelShader * mPixelShader;
	void Initialize();
public:
    GUIRenderer();
    ~GUIRenderer();
	void OnResetDevice();
	void OnLostDevice();
    ruFontHandle CreateFont( int size, const string & name );
    void RenderAllGUIElements();
};