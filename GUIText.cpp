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
#include "GUIText.h"

GUIText::GUIText(const weak_ptr<GUIScene> & scene, const string & theText, float theX, float theY, float theWidth, float theHeight, ruVector3 theColor, int theAlpha, ruTextAlignment theTextAlign, const shared_ptr<BitmapFont> & theFont) :
	GUINode(scene),
	mFont(theFont),
	mTextAlign(theTextAlign) {
	SetPosition(theX, theY);
	SetSize(theWidth, theHeight);
	SetText(theText);
	SetColor(theColor);
	SetAlpha(theAlpha);
	mVisible = true;
}

void GUIText::BreakOnLines() {
	mLines.clear();

	// break on lines, align to left corner of rectangle [mWidth; mHeight]
	int lastNotAlpha = -1;
	float stringWidth = 0.0f;
	int prevSubStringLocation = 0;
	float y = 0.0f;
	
	int stepX = 0, stepY = 0;
	int gWidth = 0, height = 0;
	gft_texcoord_t texCoords[4];

	size_t length = mTextUTF32.length();
	for (int i = 0; i < length; ++i) {
		char32_t symbol = mTextUTF32[i];

		gft_glyph_get_width(mFont->mFont, symbol, &gWidth);
		gft_glyph_get_caret_step_y(mFont->mFont, symbol, &stepY);

		if (symbol > 0 && symbol < 255) {
			if (isdigit(symbol) || ispunct(symbol) || isspace(symbol)) {
				lastNotAlpha = i;
			}
		}

		stringWidth += gWidth;
		if (stringWidth > mWidth || i == (length - 1) || symbol == '\n') {
			if (lastNotAlpha < 0 || i == (length - 1)) {
				mLines.push_back(GUITextLine(0.0f, y, mTextUTF32.substr(prevSubStringLocation, i - prevSubStringLocation + 1)));
				prevSubStringLocation = i;
			} else {
				mLines.push_back(GUITextLine(0.0f, y, mTextUTF32.substr(prevSubStringLocation, lastNotAlpha - prevSubStringLocation + 1)));
				i = lastNotAlpha + 1;
				prevSubStringLocation = i;
			}
			stringWidth = 0;
			lastNotAlpha = -1;
			y += stepY;
		}
	}

	// do alignment
	if (mTextAlign == ruTextAlignment::Center) {
		for (auto & line : mLines) {
			float width = 0.0f;
			for (int i = 0; i < line.mSubstring.size(); ++i) {
				char32_t symbol = line.mSubstring[i];
				gft_glyph_get_width(mFont->mFont, symbol, &gWidth);
				width += gWidth;
			}
			line.mX = (mWidth - width) * 0.5f;
			line.mY += (mHeight - mLines.size() * stepY) * 0.5f;
		}
	}
}

GUIText::~GUIText() {

}

shared_ptr<BitmapFont> GUIText::GetFont() {
	return mFont;
}

std::string & GUIText::GetText() {
	return mTextUTF8;
}

ruTextAlignment GUIText::GetTextAlignment() {
	return mTextAlign;
}

void GUIText::SetText(const string & text) {
	mTextUTF8 = text;

	// convert utf8 to utf 32 
	gft_symbol_t utf32buffer[16384] = { 0 }; // should be enough - !!! AWARE FIXED SIZE !!!

	if (gft_utf8_to_utf32(mTextUTF8.c_str(), utf32buffer, 16384)) {
		Log::Error("Unable to convert UTF8-string to UTF32-string. Buffer overflow!");
	}

	mTextUTF32 = (char32_t*)utf32buffer;	

	unsigned char * cc = (unsigned char *)mTextUTF8.c_str();

	BreakOnLines();
}
