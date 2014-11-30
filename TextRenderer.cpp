#include "TextRenderer.h"

TextRenderer * g_textRenderer = nullptr;

void TextRenderer::RenderText( string text ) {
	// write vertices
	TextQuad * quad = nullptr;
	CheckDXErrorFatal( vertexBuffer->Lock( 0, maxChars * sizeof( TextQuad ), (void**)&quad, 0 ));

	int caretX = renderRect.left;
	int caretY = renderRect.top;

	int lines, height, avWidth, avSymbolWidth ;
	ComputeTextMetrics( text, lines, height, avWidth, avSymbolWidth );

	if( alignment ) {
		caretY = renderRect.top + (( renderRect.bottom - renderRect.top ) - height ) / 2.0f;
		caretX = renderRect.left + (( renderRect.right - renderRect.left ) - avWidth ) / 2.0f;
	}

	words.clear();
	// break line into array of words, need for correct word wrap
	char buf[4096];
	strcpy( buf, text.c_str() );
	char * ptr = strtok( buf, " " );
	while( ptr ) {
		words.push_back( ptr );
		ptr = strtok( 0, " " );
	}
	
	for( auto & word : words ) {
		// word wrap
		if( caretX + word.size() * avSymbolWidth > renderRect.right ) {
			caretX = renderRect.left;
			caretY += font->glyphSize;
		}

		// each word ens with space
		word.push_back( ' ' );
		for( unsigned char symbol : word ) {
			BitmapFont::CharMetrics & charMetr = font->charsMetrics[ symbol ];

			int currentX = caretX + charMetr.bitmapLeft;
			int currentY = caretY - charMetr.bitmapTop + font->glyphSize;

			quad->v1 = TextVertex( Vector3( currentX,					currentY,					0.0f ), charMetr.texCoords[0] );
			quad->v2 = TextVertex( Vector3( currentX + font->glyphSize, currentY,					0.0f ), charMetr.texCoords[1] );
			quad->v3 = TextVertex( Vector3( currentX + font->glyphSize, currentY + font->glyphSize, 0.0f ), charMetr.texCoords[2] );
			quad->v4 = TextVertex( Vector3( currentX,					currentY + font->glyphSize, 0.0f ), charMetr.texCoords[3] );

			caretX += charMetr.advanceX;

			if( caretX >= renderRect.right || symbol == '\n' ) {
				caretX = renderRect.left;
				caretY += font->glyphSize;
			}

			quad++;
		}
	}

	CheckDXErrorFatal( vertexBuffer->Unlock());

	// write indices
	Face * face = nullptr;
	CheckDXErrorFatal( indexBuffer->Lock( 0, maxChars * sizeof( Face ), (void**)&face, 0 ));

	int n = 0;
	for( auto symbol : text ) {

		face->index[0] = n + 0;
		face->index[1] = n + 1;
		face->index[2] = n + 2;

		face->index[3] = n + 0;
		face->index[4] = n + 2;
		face->index[5] = n + 3;

		face++;
		n += 4;
	}

	CheckDXErrorFatal( indexBuffer->Unlock());

	IDirect3DStateBlock9 * state = nullptr;
	CheckDXErrorFatal( g_device->CreateStateBlock( D3DSBT_ALL, &state ) );

	PrepareToDraw2D();

	CheckDXErrorFatal( g_device->SetTexture( 0, font->atlas ) );

	pixelShader->Bind();
	vertexShader->Bind();
	CheckDXErrorFatal( vertexShader->GetConstantTable()->SetMatrix( g_device, vProj, &orthoMatrix ));
	CheckDXErrorFatal( pixelShader->GetConstantTable()->SetFloatArray( g_device, pColor, textColor.elements, 3 ));
	CheckDXErrorFatal( g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( TextVertex )));
	CheckDXErrorFatal( g_device->SetIndices( indexBuffer ));
	CheckDXErrorFatal( g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, text.size() * 4, 0, text.size() * 2 ));

	CheckDXErrorFatal( state->Apply( ) );
	state->Release();
}


void TextRenderer::ComputeTextMetrics( const string & text, int & lines, int & height, int & avWidth, int & avSymbolWidth  ) {
	lines = 1;

	int caretX = renderRect.left;
	int caretY = renderRect.top;

	int totalWidth = 0;
	int totalHeight = 0;

	for( unsigned char symbol : text ) {
		BitmapFont::CharMetrics & charMetr = font->charsMetrics[ symbol ];

		caretX += charMetr.advanceX;
		totalWidth += charMetr.advanceX;
		totalHeight += font->glyphSize * 2 - charMetr.bitmapTop ;

		if( caretX >= renderRect.right || symbol == '\n' ) {
			lines++;
			caretX = renderRect.left;
			caretY += font->glyphSize;
		}
	}

	avSymbolWidth = (float)totalWidth / (float)text.size();
	avWidth = (float)totalWidth / (float)lines;
	height = (float)totalHeight / (float)text.size();
}

void TextRenderer::PrepareToDraw2D() {
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZENABLE, FALSE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ));
	CheckDXErrorFatal( g_device->SetVertexDeclaration( vertexDeclaration ));
}

void TextRenderer::SetRect( int x, int y, int w, int h ) {
	renderRect.left = x;
	renderRect.top = y;
	renderRect.right = x + w;
	renderRect.bottom = y + h;
}

void TextRenderer::SetFont( BitmapFont * cfont ) {
	font = cfont;
}

TextRenderer::TextRenderer() {
	maxChars = 4096;

	int vBufLen = maxChars * sizeof( TextQuad );
	CheckDXErrorFatal( g_device->CreateVertexBuffer( vBufLen, D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_XYZ, D3DPOOL_DEFAULT, &vertexBuffer, nullptr ));

	int iBufLen = maxChars * sizeof( Face );
	CheckDXErrorFatal( g_device->CreateIndexBuffer( iBufLen, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr ));

	D3DVERTEXELEMENT9 vertexDeclarationElements[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	CheckDXErrorFatal( g_device->CreateVertexDeclaration( vertexDeclarationElements, &vertexDeclaration ));

	D3DVIEWPORT9 vp; CheckDXErrorFatal( g_device->GetViewport( &vp ));
	D3DXMatrixOrthoOffCenterLH ( &orthoMatrix, 0, vp.Width, vp.Height, 0, 0, 1024 );

	string vertexShaderSource = 
		"float4x4 gProj;\n"

		"struct VSOutput {\n"
		"	float4 position : POSITION;\n"
		"	float2 texCoord : TEXCOORD0;\n"
		"};\n"

		"VSOutput main( float4 position : POSITION, float2 texCoord : TEXCOORD0 ) {\n "
		"	VSOutput output;\n"
		"	output.position = mul( position, gProj );\n"
		"	output.texCoord = texCoord;\n"
		"	return output;\n"
		"};\n";

	vertexShader = new VertexShader( vertexShaderSource );

	vProj = vertexShader->GetConstantTable()->GetConstantByName( 0, "gProj" );

	string pixelShaderSource = 
		"sampler diffuse : register( s0 );\n"
		"float3 color;\n"
		"float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0 {\n"
		"	float4 texel = tex2D( diffuse, texCoord );\n"
		"	return float4( color.x, color.y, color.z, texel.a );\n"			
		"};\n";

	pixelShader = new PixelShader( pixelShaderSource );

	pColor = pixelShader->GetConstantTable()->GetConstantByName( 0, "color" );

	SetColor( Vector3( 1.0f, 1.0f, 1.0f ));
	SetAlignment( 0 );
}

void TextRenderer::SetColor( Vector3 color )
{
	textColor = color;
}

void TextRenderer::SetAlignment( int al )
{
	alignment = al;
}

TextRenderer::TextVertex::TextVertex( Vector3 cp, Vector2 tp ) {
	p = cp;
	t = tp;
}
