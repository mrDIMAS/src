#pragma once

#include "Shader.h"
#include "BitmapFont.h"

class GUIText
{
public:
    BitmapFont * font;
    string text;
    RECT rect;
    int color;
    int textAlign;
    explicit GUIText( string theText, float theX, float theY, float theWidth, float theHeight, ruVector3 theColor, int theAlpha, int theTextAlign, ruFontHandle theFont );
};

class GUIRect
{
public:
    float x;
    float y;
    float w;
    float h;
    Texture * texture;
    int color;
    explicit GUIRect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, ruVector3 theColor, int theAlpha );
};

class GUILine
{
public:
    ruLinePoint begin;
    ruLinePoint end;
    explicit GUILine( const ruLinePoint & theBegin, const ruLinePoint & theEnd );
};

class TextVertex
{
public:
    ruVector3 p;
    ruVector2 t;
    DWORD color;
    explicit TextVertex( ruVector3 cp, ruVector2 tp, DWORD clr )
    {
        p = cp;
        t = tp;
        color = clr;
    }
};

class TextQuad
{
public:
    TextVertex v1, v2, v3, v4;
};

class Face
{
public:
    unsigned short index[6];
};

class GUIRenderer
{
private:
    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DVertexBuffer9 * lineVertexBuffer;
    IDirect3DVertexDeclaration9 * lineDecl;
    IDirect3DVertexDeclaration9 * vertDecl;
    map<BitmapFont*,vector<GUIText>> texts;
    queue<GUIRect> rects;
    queue<GUILine> lines;
    int maxLineCount;
    int sizeOfRectBytes;
    D3DXMATRIX orthoMatrix;
    void RenderRect( GUIRect & r );
    void RenderLines();
    void PrepareToDraw2D();
    VertexShader * vertexShader;
    PixelShader * pixelShader;
    D3DXHANDLE vProj;
public:
    GUIRenderer();
    ~GUIRenderer();
    void QueueRect( const GUIRect & rect )
    {
        rects.push( rect );
    }
    ruFontHandle CreateFont( int size, const char * name, int italic, int underlined );
    void RenderText( const GUIText & text );
    void Render3DLine( const GUILine & line );
    void DrawWireBox( ruLinePoint min, ruLinePoint max );
    void RenderAllGUIElements();
};