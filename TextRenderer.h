#pragma once

#include "Common.h"
#include "BitmapFont.h"
#include "Shader.h"

class GUIText;

class TextRenderer {
public:
	int maxChars;

	D3DXMATRIX orthoMatrix;

	IDirect3DVertexBuffer9 * vertexBuffer;
	IDirect3DIndexBuffer9 * indexBuffer;
	IDirect3DVertexDeclaration9 * vertexDeclaration;

	class TextVertex {
	public:
		Vector3 p;
		Vector2 t;
		DWORD color;
		TextVertex( Vector3 cp, Vector2 tp, DWORD clr );
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

	// array for word wrap
	vector< string > words;

	explicit TextRenderer( );
	virtual ~TextRenderer( );
	void ComputeTextMetrics( GUIText & guiText, int & lines, int & height, int & avWidth, int & avSymbolWidth );
	void PrepareToDraw2D();
	void RenderTextGroup( vector<GUIText> & textGroup, BitmapFont * font );
};