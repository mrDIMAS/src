#pragma once

#include "Common.h"
#include "BitmapFont.h"
#include "Shader.h"

class GUIText;

class TextRenderer {
public:
	int maxChars;
	IDirect3DVertexBuffer9 * vertexBuffer;
	IDirect3DIndexBuffer9 * indexBuffer;
	explicit TextRenderer( );
	virtual ~TextRenderer( );
	void ComputeTextMetrics( GUIText & guiText, int & lines, int & height, int & avWidth, int & avSymbolWidth );
	void RenderTextGroup( vector<GUIText> & textGroup, BitmapFont * font );
};