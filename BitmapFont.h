#pragma once


#include "fterrors.h"
#include "EffectsQuad.h"

struct CharMetrics {
	int advanceX;
	int advanceY;
	int bitmapTop;
	int bitmapLeft;
	ruVector2 texCoords[4]; // texture coords in atlas
};

struct BGRA8Pixel {
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
};

class BitmapFont : public RendererComponent {
private:
	IDirect3DTexture9 * atlas;
	vector< CharMetrics > charsMetrics;
	string mSourceFile;
	int glyphSize;
	void Create( );
public:
    static vector< BitmapFont* > fonts;   
    explicit BitmapFont( const string & file, int size );
    virtual ~BitmapFont( );
	int GetGlyphSize();
	void BindAtlasToLevel( int level );
	CharMetrics * GetCharacterMetrics( int charNum );
	void OnResetDevice();
	void OnLostDevice();
};