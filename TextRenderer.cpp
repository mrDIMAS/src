#include "Precompiled.h"
#include "Engine.h"
#include "TextRenderer.h"
#include "GUIRenderer.h"


int GetWordWidth( BitmapFont * font, string & text, int start ) {
	int wordWidth = 0;
	for( int i = start; i < text.size(); i++ ) {
		unsigned char symbol = text[ i ];
		if( symbol == 0 || symbol == ' ') {
			break;
		}
		wordWidth += font->GetCharacterMetrics( symbol )->advanceX;
	}
	return wordWidth;
}


void TextRenderer::RenderText( GUIText* guiText ) {
    TextQuad * quad = nullptr;
    vertexBuffer->Lock( 0, maxChars * sizeof( TextQuad ), (void**)&quad, D3DLOCK_DISCARD );
    Face * face = nullptr;
    indexBuffer->Lock( 0, maxChars * sizeof( Face ), (void**)&face, D3DLOCK_DISCARD );
    int n = 0, totalLetters = 0;

    RECT boundingRect = guiText->GetBoundingRect();

    int caretX = boundingRect.left;
    int caretY = boundingRect.top;

    int lines, height, avWidth, avSymbolWidth ;
    ComputeTextMetrics( guiText, lines, height, avWidth, avSymbolWidth );

    if( guiText->GetTextAlignment() ) {
        caretY = boundingRect.top + (( boundingRect.bottom - boundingRect.top ) - height ) / 2.0f;
        caretX = boundingRect.left + (( boundingRect.right - boundingRect.left ) - avWidth ) / 2.0f;
    }
	BitmapFont * font = guiText->GetFont();
	int symbolCounter = 0;
	for( unsigned char symbol : guiText->GetText() ) {
		CharMetrics * charMetr = font->GetCharacterMetrics( symbol );

		// word wrap
		if( caretX + GetWordWidth( font, guiText->GetText(), symbolCounter ) > boundingRect.right ) {
			caretX = boundingRect.left;
			caretY += font->GetGlyphSize();
		}
		
		int currentX = caretX + charMetr->bitmapLeft;
		int currentY = caretY - charMetr->bitmapTop + font->GetGlyphSize();

		int color = guiText->GetPackedColor();

		quad->mVertices[0].Set( currentX, currentY, charMetr->texCoords[0], color );
		quad->mVertices[1].Set( currentX + font->GetGlyphSize(), currentY, charMetr->texCoords[1], color );
		quad->mVertices[2].Set( currentX + font->GetGlyphSize(), currentY + font->GetGlyphSize(), charMetr->texCoords[2], color );
		quad->mVertices[3].Set( currentX, currentY + font->GetGlyphSize(), charMetr->texCoords[3], color );

		caretX += charMetr->advanceX;

		if( caretX >= boundingRect.right || symbol == '\n' ) {
			caretX = boundingRect.left;
			caretY += font->GetGlyphSize();
		}

		quad++;
		totalLetters++;

		// indices
		face->Set( n, n + 1, n + 2, n, n + 2, n + 3 );
		face++;
		n += 4;

		symbolCounter++;
	}

    vertexBuffer->Unlock();
    indexBuffer->Unlock();

    font->BindAtlasToLevel( 0 );

    Engine::Instance().GetDevice()->SetStreamSource( 0, vertexBuffer, 0, sizeof( TextVertex ));
    Engine::Instance().GetDevice()->SetIndices( indexBuffer );
	if( totalLetters > 0 ) {
		Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, totalLetters * 4, 0, totalLetters * 2 );
	}
    Engine::Instance().RegisterDIP();
}


void TextRenderer::ComputeTextMetrics( GUIText * guiText, int & lines, int & height, int & avWidth, int & avSymbolWidth  ) {
    lines = 1;

    RECT boundingRect = guiText->GetBoundingRect();

    int caretX = boundingRect.left;
    int caretY = boundingRect.top;

    int totalWidth = 0;
    int totalHeight = 0;

    int symbolCount = 0;
	string & str = guiText->GetText();
	int size = str.size();
	for( int i = 0; i < size; i++ ) {
		unsigned char symbol = str[i];
		BitmapFont * font = guiText->GetFont();
        CharMetrics * charMetr = font->GetCharacterMetrics( symbol );

        caretX += charMetr->advanceX;
        totalWidth += charMetr->advanceX;
        totalHeight += font->GetGlyphSize() * 2 - charMetr->bitmapTop ;

        if( caretX >= boundingRect.right || symbol == '\n' ) {
            lines++;
            caretX = boundingRect.left;
            caretY += font->GetGlyphSize();
        }
        symbol++;
        symbolCount++;
    }

    avSymbolWidth = static_cast<float>( totalWidth ) / static_cast<float>( symbolCount );
    avWidth = static_cast<float>( totalWidth ) / static_cast<float>( lines );
    height = static_cast<float>( totalHeight ) / static_cast<float>( symbolCount );
}

TextRenderer::TextRenderer() {
    OnResetDevice();
}

TextRenderer::~TextRenderer() {
    OnLostDevice();
}

void TextRenderer::OnLostDevice()
{
	vertexBuffer->Release();
	indexBuffer->Release();
}

void TextRenderer::OnResetDevice()
{
	maxChars = 8192;
	int vBufLen = maxChars * sizeof( TextQuad );
	Engine::Instance().GetDevice()->CreateVertexBuffer( vBufLen, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vertexBuffer, nullptr );
	int iBufLen = maxChars * sizeof( Face );
	Engine::Instance().GetDevice()->CreateIndexBuffer( iBufLen, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr );
}

