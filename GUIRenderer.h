#pragma once

#include "Shader.h"
#include "BitmapFont.h"

class GUIText {
public:
	BitmapFont * font;
	string text;
	RECT rect;
	int color;
	int textAlign;
	GUIText( string theText, float theX, float theY, float theWidth, float theHeight, Vector3 theColor, int theAlpha, int theTextAlign, FontHandle theFont );
};

class GUIRect {
public:
	float x;
	float y;
	float w;
	float h;
	Texture * texture;
	int color;
	GUIRect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, Vector3 theColor, int theAlpha );
};

class GUILine {
public:
	LinePoint begin;
	LinePoint end;
	GUILine( const LinePoint & theBegin, const LinePoint & theEnd );
};

class GUIRenderer {
private:
    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DVertexBuffer9 * lineVertexBuffer;
    IDirect3DVertexDeclaration9 * lineDecl;
    IDirect3DVertexDeclaration9 * vertDecl;
    vector<ID3DXFont*> fonts;
    map<BitmapFont*,vector<GUIText>> texts;
    queue<GUIRect> rects;
    queue<GUILine> lines;
	int maxLineCount;
    int sizeOfRectBytes;
    D3DXMATRIX orthoMatrix;
    void RenderLines();
    void RenderRects();
    void RenderTexts();
    void RenderCursor();
    void PrepareToDraw2D();
public:
    GUIRenderer();
    ~GUIRenderer();
    FontHandle CreateFont( int size, const char * name, int italic, int underlined );
    void RenderRect( const GUIRect & r );
    void RenderText( const GUIText & text );
    void Render3DLine( const GUILine & line );
    void DrawWireBox( LinePoint min, LinePoint max );
    void RenderAllGUIElements();
};