/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

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

int GetStringWidth( BitmapFont * font, string & text, int start ) {
	int wordWidth = 0;
	for( int i = start; i < text.size(); i++ ) {
		unsigned char symbol = text[ i ];
		wordWidth += font->GetCharacterMetrics( symbol )->advanceX;
	}
	return wordWidth;
}

// grim code, must be rewritten !!!
void TextRenderer::RenderText( const shared_ptr<GUIText> & guiText )
{
    Vertex * vertices = nullptr;
    mVertexBuffer->Lock( 0, mMaxChars * 4 * sizeof( Vertex ), reinterpret_cast<void**>( &vertices ), D3DLOCK_DISCARD );
    Face * face = nullptr;
    mIndexBuffer->Lock( 0, mMaxChars * sizeof( Face ), reinterpret_cast<void**>( &face ), D3DLOCK_DISCARD );
    int n = 0, totalLetters = 0;

    RECT boundingRect = guiText->GetBoundingRect();

    int caretX = boundingRect.left;
    int caretY = boundingRect.top;

    int lines, height, avWidth, avSymbolWidth ;
    ComputeTextMetrics( guiText, lines, height, avWidth, avSymbolWidth );

    if( guiText->GetTextAlignment() == ruTextAlignment::Center ) {
        caretY = boundingRect.top + (( boundingRect.bottom - boundingRect.top ) - height ) / 2.0f;
        caretX = boundingRect.left + (( boundingRect.right - boundingRect.left ) - avWidth ) / 2.0f;
    }

	shared_ptr<BitmapFont> & font = guiText->GetFont();

	int symbolCounter = 0;

	string & text = guiText->GetText();

	for( unsigned char symbol : guiText->GetText() ) {
		CharMetrics * charMetr = font->GetCharacterMetrics( symbol );

		// word wrap
		if( caretX + GetWordWidth( font.get(), guiText->GetText(), symbolCounter ) > boundingRect.right ) {
			if( guiText->GetTextAlignment() == ruTextAlignment::Center ) {
				caretX = boundingRect.left + (( boundingRect.right - boundingRect.left ) - GetStringWidth( font.get(), text, symbolCounter ) ) / 2.0f;
			} else {
				caretX = boundingRect.left;
			}
			caretY += font->GetGlyphSize();
		}
		
		int currentX = caretX + charMetr->bitmapLeft;
		int currentY = caretY - charMetr->bitmapTop + font->GetGlyphSize();

		*(vertices + 0) = Vertex( ruVector3( currentX, currentY, 0.0f ), charMetr->texCoords[0], ruVector4( guiText->GetColor(), guiText->GetAlpha() / 255.0f ));
		*(vertices + 1) = Vertex( ruVector3( currentX + font->GetGlyphSize(), currentY, 0.0f ), charMetr->texCoords[1], ruVector4( guiText->GetColor(), guiText->GetAlpha() / 255.0f ));
		*(vertices + 2) = Vertex( ruVector3( currentX + font->GetGlyphSize(), currentY + font->GetGlyphSize(), 0.0f ), charMetr->texCoords[2], ruVector4( guiText->GetColor(), guiText->GetAlpha() / 255.0f ));
		*(vertices + 3) = Vertex( ruVector3( currentX, currentY + font->GetGlyphSize(), 0.0f ), charMetr->texCoords[3], ruVector4( guiText->GetColor(), guiText->GetAlpha() / 255.0f ));

		caretX += charMetr->advanceX;

		if( caretX >= boundingRect.right || symbol == '\n' ) {
			caretX = boundingRect.left;
			caretY += font->GetGlyphSize();

			if( guiText->GetTextAlignment() == ruTextAlignment::Center ) {
				caretX = boundingRect.left + (( boundingRect.right - boundingRect.left ) - GetStringWidth( font.get(), text, symbolCounter ) ) / 2.0f;
			}
		}

		vertices += 4;
		totalLetters++;

		// indices
		face->Set( n, n + 1, n + 2, n, n + 2, n + 3 );
		face++;
		n += 4;

		symbolCounter++;
	}

    mVertexBuffer->Unlock();
    mIndexBuffer->Unlock();

    font->BindAtlasToLevel( 0 );

    pD3D->SetStreamSource( 0, mVertexBuffer, 0, sizeof( Vertex ));
    pD3D->SetIndices( mIndexBuffer );
	if( totalLetters > 0 ) {
		pD3D->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, totalLetters * 4, 0, totalLetters * 2 );
	}
    pEngine->RegisterDIP();
}


void TextRenderer::ComputeTextMetrics( const shared_ptr<GUIText> & guiText, int & lines, int & height, int & avWidth, int & avSymbolWidth  )
{
    lines = 1;

    RECT boundingRect = guiText->GetBoundingRect();

    int caretX = boundingRect.left;
    int caretY = boundingRect.top;

    int totalWidth = 0;
    int totalHeight = 0;

    int symbolCount = 0;

	shared_ptr<BitmapFont> & font = guiText->GetFont();

	for( unsigned char symbol : guiText->GetText()  ) {
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

void TextRenderer::OnLostDevice() {
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();
}

void TextRenderer::OnResetDevice() {
	mMaxChars = 8192;
	int vBufLen = mMaxChars * 4 * sizeof( Vertex );
	pD3D->CreateVertexBuffer( vBufLen, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &mVertexBuffer, nullptr );
	int iBufLen = mMaxChars * sizeof( Face );
	pD3D->CreateIndexBuffer( iBufLen, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mIndexBuffer, nullptr );
}

