#pragma once

#include "Shader.h"
#include "BitmapFont.h"
#include "GUIRect.h"
#include "GUIText.h"
#include "GUIButton.h"


class ruGUILine : public GUINode {
public:
    ruLinePoint begin;
    ruLinePoint end;
    explicit ruGUILine( const ruLinePoint & theBegin, const ruLinePoint & theEnd );
    explicit ruGUILine( );
};


class TextVertex {
public:
    ruVector3 p;
    ruVector2 t;
    DWORD color;
    explicit TextVertex( ruVector3 cp, ruVector2 tp, DWORD clr ) {
        p = cp;
        t = tp;
        color = clr;
    }
};

class TextQuad {
public:
    TextVertex v1, v2, v3, v4;
};

class Face {
public:
    unsigned short index[6];
};

class GUIRenderer {
private:
    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DVertexBuffer9 * lineVertexBuffer;
    IDirect3DVertexDeclaration9 * lineDecl;
    IDirect3DVertexDeclaration9 * vertDecl;
    int maxLineCount;
    int sizeOfRectBytes;
    D3DXMATRIX orthoMatrix;
    void RenderRect( GUIRect * r );
    void RenderLines();
    VertexShader * vertexShader;
    PixelShader * pixelShader;
    D3DXHANDLE vProj;
public:
    GUIRenderer();
    ~GUIRenderer();
    ruFontHandle CreateFont( int size, const char * name, int italic, int underlined );
    void RenderAllGUIElements();
};