#pragma once

#include "Common.h"
#include "BitmapFont.h"
#include "Shader.h"

class TextRenderer {
public:
	int maxChars;
	BitmapFont * font;
	RECT renderRect;

	D3DXMATRIX orthoMatrix;

	Vector3 textColor;
	IDirect3DVertexBuffer9 * vertexBuffer;
	IDirect3DIndexBuffer9 * indexBuffer;
	IDirect3DVertexDeclaration9 * vertexDeclaration;

	int alignment;

	class TextVertex {
	public:
		Vector3 p;
		Vector2 t;
		TextVertex( Vector3 cp, Vector2 tp );
	};

	class TextQuad {
	public:
		TextVertex v1, v2, v3, v4;
	};

	class Face {
	public:
		unsigned short index[6];
	};

	VertexShader * vertexShader;
	PixelShader * pixelShader;

	D3DXHANDLE vProj;
	D3DXHANDLE pColor;

	// array for word wrap
	vector< string > words;

	TextRenderer( );

	void SetFont( BitmapFont * cfont );
	void SetAlignment( int al );
	void SetRect( int x, int y, int w, int h );
	void ComputeTextMetrics( const string & text, int & lines, int & height, int & avWidth, int & avSymbolWidth );

	void SetColor( Vector3 color );
	void PrepareToDraw2D();

	void RenderText( string text );
};