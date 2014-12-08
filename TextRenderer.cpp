#include "TextRenderer.h"
#include "GUIRenderer.h"

TextRenderer * g_textRenderer = nullptr;

void TextRenderer::RenderTextGroup( vector<GUIText> & textGroup, BitmapFont * font) {
	TextQuad * quad = nullptr;
	CheckDXErrorFatal( vertexBuffer->Lock( 0, maxChars * sizeof( TextQuad ), (void**)&quad, 0 ));
	Face * face = nullptr;
	CheckDXErrorFatal( indexBuffer->Lock( 0, maxChars * sizeof( Face ), (void**)&face, 0 ));
	int n = 0, totalLetters = 0;
	for( auto & guiText : textGroup ) {
		int caretX = guiText.rect.left;
		int caretY = guiText.rect.top;

		int lines, height, avWidth, avSymbolWidth ;
		ComputeTextMetrics( guiText, lines, height, avWidth, avSymbolWidth );

		if( guiText.textAlign ) {
			caretY = guiText.rect.top + (( guiText.rect.bottom - guiText.rect.top ) - height ) / 2.0f;
			caretX = guiText.rect.left + (( guiText.rect.right - guiText.rect.left ) - avWidth ) / 2.0f;
		}
						
		char buf[8192];
		strcpy( buf, guiText.text.c_str() );
		char * ptr = strtok( buf, " " );
		while( ptr ) {
			// word wrap
			int wordLen = strlen( ptr );
			if( caretX + wordLen * avSymbolWidth > guiText.rect.right ) {
				caretX = guiText.rect.left;
				caretY += font->glyphSize;
			}
			char * strPtr = ptr;
			while( true ) {				
				unsigned char symbol = *strPtr;
				char lineEnd = symbol == 0;
				if( lineEnd ) {
					symbol = ' '; // draw space
				}
				BitmapFont::CharMetrics & charMetr = font->charsMetrics[ symbol ];

				int currentX = caretX + charMetr.bitmapLeft;
				int currentY = caretY - charMetr.bitmapTop + font->glyphSize;

				quad->v1 = TextVertex( ruVector3( currentX, currentY, 0.0f ), charMetr.texCoords[0], guiText.color );
				quad->v2 = TextVertex( ruVector3( currentX + font->glyphSize, currentY, 0.0f ), charMetr.texCoords[1], guiText.color );
				quad->v3 = TextVertex( ruVector3( currentX + font->glyphSize, currentY + font->glyphSize, 0.0f ), charMetr.texCoords[2], guiText.color );
				quad->v4 = TextVertex( ruVector3( currentX, currentY + font->glyphSize, 0.0f ), charMetr.texCoords[3], guiText.color );

				caretX += charMetr.advanceX;

				if( caretX >= guiText.rect.right || symbol == '\n' ) {
					caretX = guiText.rect.left;
					caretY += font->glyphSize;
				}

				quad++;
				totalLetters++;

				// indices
				face->index[0] = n;
				face->index[1] = n + 1;
				face->index[2] = n + 2;
				face->index[3] = n;
				face->index[4] = n + 2;
				face->index[5] = n + 3;
				face++;
				n += 4;
				if( lineEnd )  {
					break;
				}
				strPtr++;
			}
			// get next token
			ptr = strtok( 0, " " );
		}
	};

	CheckDXErrorFatal( vertexBuffer->Unlock());
	CheckDXErrorFatal( indexBuffer->Unlock());

	CheckDXErrorFatal( g_device->SetTexture( 0, font->atlas ) );

	CheckDXErrorFatal( g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( TextVertex )));
	CheckDXErrorFatal( g_device->SetIndices( indexBuffer ));
	CheckDXErrorFatal( g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, totalLetters * 4, 0, totalLetters * 2 ));
	g_dips++;
}


void TextRenderer::ComputeTextMetrics( GUIText & guiText, int & lines, int & height, int & avWidth, int & avSymbolWidth  ) {
	lines = 1;

	int caretX = guiText.rect.left;
	int caretY = guiText.rect.top;

	int totalWidth = 0;
	int totalHeight = 0;

	for( unsigned char symbol : guiText.text ) {
		BitmapFont::CharMetrics & charMetr = guiText.font->charsMetrics[ symbol ];

		caretX += charMetr.advanceX;
		totalWidth += charMetr.advanceX;
		totalHeight += guiText.font->glyphSize * 2 - charMetr.bitmapTop ;

		if( caretX >= guiText.rect.right || symbol == '\n' ) {
			lines++;
			caretX = guiText.rect.left;
			caretY +=  guiText.font->glyphSize;
		}
	}

	avSymbolWidth = (float)totalWidth / (float)guiText.text.size();
	avWidth = (float)totalWidth / (float)lines;
	height = (float)totalHeight / (float)guiText.text.size();
}

TextRenderer::TextRenderer() {
	maxChars = 8192;
	int vBufLen = maxChars * sizeof( TextQuad );
	CheckDXErrorFatal( g_device->CreateVertexBuffer( vBufLen, D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_XYZ, D3DPOOL_DEFAULT, &vertexBuffer, nullptr ));
	int iBufLen = maxChars * sizeof( Face );
	CheckDXErrorFatal( g_device->CreateIndexBuffer( iBufLen, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr ));
}

TextRenderer::~TextRenderer() {
	vertexBuffer->Release();
	indexBuffer->Release();
}

