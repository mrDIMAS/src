#pragma once

#include "Shader.h"
#include "BitmapFont.h"
#include "GUIRect.h"
#include "GUIText.h"
#include "GUIButton.h"

struct TextVertex {
    ruVector3 p;
    ruVector2 t;
    DWORD color;
};

class TextQuad {
public:
    TextVertex v1, v2, v3, v4;

	TextQuad();
};

class Face {
public:
    unsigned short index[6];
};

class GUIRenderer {
private:
    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DVertexDeclaration9 * vertDecl;
    int sizeOfRectBytes;
    D3DXMATRIX orthoMatrix;
    void RenderRect( GUIRect * r );
    VertexShader * vertexShader;
    PixelShader * pixelShader;
public:
    GUIRenderer();
    ~GUIRenderer();
    ruFontHandle CreateFont( int size, const string & name );
    void RenderAllGUIElements();
};