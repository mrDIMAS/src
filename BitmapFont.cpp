#include "BitmapFont.h"
#include "Utility.h"

FT_Library g_ftLibrary;
vector< BitmapFont* > BitmapFont::fonts;

void BitmapFont::RenderAtlas( EffectsQuad * quad ) {
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF ));
	CheckDXErrorFatal( g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL));
	CheckDXErrorFatal( g_device->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_NOTEQUAL));
	CheckDXErrorFatal( g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZENABLE, FALSE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ));

	g_device->SetTexture( 4, atlas );
	quad->Bind();
	quad->SetSize( atlasWidth, atlasHeight );
	quad->Render();
}

BitmapFont::BitmapFont( const char * file, int size ) {
	glyphSize = size;
	// load new font face
	if( FT_New_Face( g_ftLibrary, file, 0, &face ) ) {
		LogError( Format( "Failed to load '%s' font!", file ));
	}
	if( FT_Set_Pixel_Sizes( face, 0, size )) {
		LogError( Format( "Failed to FT_Set_Pixel_Sizes!", file ));
	}
	if( FT_Select_Charmap( face, FT_ENCODING_UNICODE )) {
		LogError( Format( "Failed to FT_Select_Charmap!", file ));
	}
	// create atlas texture
	atlasWidth = atlasHeight = size * 16;
	CheckDXErrorFatal( D3DXCreateTexture( g_device, atlasWidth, atlasHeight, 0, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &atlas ));
	IDirect3DSurface9 * atlasSurface = nullptr; 
	atlas->GetSurfaceLevel( 0, &atlasSurface );
	D3DLOCKED_RECT lockedRect;
	RECT rect = { 0, 0, atlasWidth, atlasHeight };
	CheckDXErrorFatal( atlasSurface->LockRect( &lockedRect, &rect, 0 ));
	// render		
	int subRectRow = 0;
	int subRectCol = 0;
	float tcStep = 1.0f / 16.0f;
	float tcX = 0.0f;
	float tcY = 0.0f;
	int charIndexOffset = 0;
	unsigned char ggg = 'à';
	for( int i = 0; i < 256; i++ ) {
		int charIndex = i;
		if( i >= 177 ) {
			charIndex = 1024;
			charIndexOffset++;
		}
		if( FT_Load_Glyph( face, FT_Get_Char_Index( face, charIndex + charIndexOffset ), FT_LOAD_DEFAULT )) {
			LogError( Format( "Failed to FT_Load_Glyph!", file ));
		}
		if( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL )) {
			LogError( Format( "Failed to FT_Load_Glyph!", file ));
		}
		int memOffsetBytes = subRectRow * lockedRect.Pitch * size + subRectCol * size * sizeof( ARGB8Pixel );
		ARGB8Pixel * subRectPixel = (ARGB8Pixel *)( (char*)lockedRect.pBits + memOffsetBytes );
		FT_Bitmap * bitmap = &( face->glyph->bitmap );
		// grab char metrics for rendering
		CharMetrics cm;
		cm.offsetX = face->glyph->metrics.width >> 6;
		cm.offsetY = face->glyph->metrics.height >> 6;
		cm.advanceX = face->glyph->advance.x >> 6;
		cm.advanceY = face->glyph->advance.y >> 6;
		cm.texCoords[0] = Vector2( tcX		   , tcY		  );
		cm.texCoords[1] = Vector2( tcX + tcStep, tcY	      );
		cm.texCoords[2] = Vector2( tcX + tcStep, tcY + tcStep );
		cm.texCoords[3] = Vector2( tcX		   , tcY + tcStep );
		cm.bitmapTop = face->glyph->bitmap_top;
		cm.bitmapLeft = face->glyph->bitmap_left;
		cm.bitmapWidth = bitmap->width;
		cm.bitmapHeight = bitmap->rows;
		charsMetrics.push_back( cm );		
		// read glyph bitmap into the atlas
		for( int row = 0; row < bitmap->rows; row++ ) {
			for( int col = 0; col < bitmap->width; col++ ) {
				ARGB8Pixel * pixel = subRectPixel + row * atlasWidth + col ;
				char bitmapPixel = bitmap->buffer[ row * bitmap->width + col ];
				pixel->a = bitmapPixel;
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
	FT_Done_Face( face );
	atlasSurface->Release();
	BitmapFont::fonts.push_back( this );
	CheckDXErrorFatal( atlasSurface->UnlockRect());
}
