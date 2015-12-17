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
#include "BitmapFont.h"
#include "Utility.h"

vector< BitmapFont* > BitmapFont::fonts;

BitmapFont::BitmapFont( const string & file, int size ) {
	glyphSize = size;
	mSourceFile = file;
	Create();
	BitmapFont::fonts.push_back( this );
}

BitmapFont::~BitmapFont() {
	BitmapFont::fonts.erase( find( BitmapFont::fonts.begin(), BitmapFont::fonts.end(), this ));
	if( atlas ) {
		atlas->Release();
	}
}

void BitmapFont::BindAtlasToLevel( int level )
{
	Engine::I().GetDevice()->SetTexture( level, atlas );
}

int BitmapFont::GetGlyphSize()
{
	return glyphSize;
}

void BitmapFont::OnLostDevice()
{
	charsMetrics.clear();
	atlas->Release();
}

void BitmapFont::OnResetDevice()
{
	Create();
}

CharMetrics * BitmapFont::GetCharacterMetrics( int charNum )
{
	if( charNum > 0 && charNum < 256 ) {
		return &charsMetrics[charNum];
	} else {
		return nullptr;
	}
}

void BitmapFont::Create()
{
	int pow2Size = CeilPow2(glyphSize);
	int atlasWidth = pow2Size * 16;
	int atlasHeight = pow2Size * 16;
	float scale = static_cast<float>(glyphSize) / static_cast<float>(pow2Size);
	FT_Library ftLibrary;
	if( FT_Init_FreeType( &ftLibrary ) ) {
		throw std::runtime_error( "Unable to initialize FreeType 2.53" );
	}    
	FT_Face face;
	// load new font face
	if( FT_New_Face( ftLibrary, mSourceFile.c_str(), 0, &face ) ) {
		Log::Error( StringBuilder( "Failed to load" ) << mSourceFile << "font!" );
	}
	if( FT_Set_Pixel_Sizes( face, 0, pow2Size )) {
		Log::Error( StringBuilder( "Failed to FT_Set_Pixel_Sizes!" ));
	}
	if( FT_Select_Charmap( face, FT_ENCODING_UNICODE )) {
		Log::Error( StringBuilder( "Failed to FT_Select_Charmap!" ));
	}
	// create atlas texture
	Engine::I().GetDevice()->CreateTexture( atlasWidth, atlasHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &atlas, nullptr );
	IDirect3DSurface9 * atlasSurface = nullptr;
	atlas->GetSurfaceLevel( 0, &atlasSurface );
	D3DLOCKED_RECT lockedRect;
	RECT rect = { 0, 0, atlasWidth, atlasHeight };
	atlasSurface->LockRect( &lockedRect, &rect, 0 );
	// render
	int subRectRow = 0;
	int subRectCol = 0;
	float tcStep = 1.0f / 16.0f; 
	float tcX = 0.0f;
	float tcY = 0.0f;
	int charIndexOffset = 0;
	for( int i = 0; i < 256; i++ ) {
		int charIndex = i;
		if( i >= 177 ) {
			charIndex = 1024;
			charIndexOffset++;
		}
		if( FT_Load_Glyph( face, FT_Get_Char_Index( face, charIndex + charIndexOffset ), FT_LOAD_DEFAULT )) {
			Log::Error( StringBuilder( "Failed to FT_Load_Glyph!" ));
		}
		if( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL )) {
			Log::Error( StringBuilder( "Failed to FT_Load_Glyph!" ));
		}
		int memOffsetBytes = subRectRow * lockedRect.Pitch * pow2Size + subRectCol * pow2Size * sizeof( BGRA8Pixel );
		BGRA8Pixel * subRectPixel = reinterpret_cast<BGRA8Pixel*>( reinterpret_cast<char*>(lockedRect.pBits) + memOffsetBytes );
		FT_Bitmap * bitmap = &( face->glyph->bitmap );
		// grab char metrics for rendering
		CharMetrics cm;
		cm.advanceX = static_cast<float>( face->glyph->advance.x >> 6 ) * scale;
		cm.advanceY = static_cast<float>( face->glyph->advance.y >> 6 ) * scale;
		cm.texCoords[0] = ruVector2( tcX, tcY );
		cm.texCoords[1] = ruVector2( tcX + tcStep, tcY );
		cm.texCoords[2] = ruVector2( tcX + tcStep, tcY + tcStep );
		cm.texCoords[3] = ruVector2( tcX, tcY + tcStep );
		cm.bitmapTop = static_cast<float>( face->glyph->bitmap_top ) * scale;
		cm.bitmapLeft = static_cast<float>( face->glyph->bitmap_left ) * scale;
		charsMetrics.push_back( cm );
		// read glyph bitmap into the atlas
		for( int row = 0; row < bitmap->rows; row++ ) {
			for( int col = 0; col < bitmap->width; col++ ) {
				BGRA8Pixel * pixel = subRectPixel + row * atlasWidth + col ;
				pixel->a = bitmap->buffer[ row * bitmap->width + col ];
				pixel->r = 255;
				pixel->g = 255;
				pixel->b = 255;
			}
		}

		tcX += tcStep;
		subRectCol++;
		if( subRectCol >= 16 ) {
			tcX = 0.0f;
			tcY += tcStep;
			subRectCol = 0;
			subRectRow++;
		}
	}
	atlasSurface->UnlockRect();
	FT_Done_Face( face );
	atlasSurface->Release();
	FT_Done_FreeType( ftLibrary );
}

void ruFont::Free() {
	delete this;
}

ruFont * ruFont::LoadFromFile( int size, const string & name ) {
	return new BitmapFont( name, size );
}

ruFont::~ruFont() {

}