
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
#include "Renderer.h"
#include "BitmapFont.h"
#include "Utility.h"



BitmapFont::BitmapFont(const string & file, int size) {
	// check for available symbols
	ifstream pFile("data/fonts/symbols.txt", ios_base::binary | ios_base::in);
	if (!pFile.good()) {
		Log::Error("Failed to load symbols.txt from data/fonts. Add this file and restart! symbols.txt must be in UTF8");
	}

	pFile.seekg(0, ios_base::end);
	size_t fSize = pFile.tellg();
	pFile.seekg(0, ios_base::beg);
	char * symbolSet = new char[fSize];
	pFile.read(reinterpret_cast<char*>(symbolSet), fSize);

	if (gft_font_create(file.c_str(), size * pEngine->GetGUIWidthScaleFactor(), GFT_DEFAULT, symbolSet, &mFont) != GFT_NO_ERROR) {
		Log::Error(StringBuilder("Failed to create font '") << file << "'!");
	}

	delete[] symbolSet;
	
	gft_rgba_pixel_t * pixels;
	int atlasWidth, atlasHeight;
	gft_font_get_width(mFont, &atlasWidth);
	gft_font_get_height(mFont, &atlasHeight);
	gft_font_get_atlas_pixels(mFont, &pixels);

	D3DCALL(pD3D->CreateTexture(atlasWidth, atlasHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mAtlas, nullptr));

	IDirect3DSurface9 * surface;
	D3DLOCKED_RECT lockedRect;
	mAtlas->GetSurfaceLevel(0, &surface);	
	surface->LockRect(&lockedRect, nullptr, 0);

	memcpy(lockedRect.pBits, pixels, atlasWidth * atlasHeight * sizeof(A8R8G8B8Pixel));

	surface->UnlockRect();
	surface->Release();
}

BitmapFont::~BitmapFont() {
	gft_font_free(mFont);

	mAtlas.Reset();
}


shared_ptr<ruFont> ruFont::LoadFromFile(int size, const string & name) {
	return shared_ptr<BitmapFont>(new BitmapFont(name, size));
}

ruFont::~ruFont() {

}