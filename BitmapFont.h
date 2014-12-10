#pragma once

#include "Common.h"
#include "fterrors.h"
#include "EffectsQuad.h"

class BitmapFont
{
public:
    FT_Face face;
    IDirect3DTexture9 * atlas;
    int atlasWidth, atlasHeight;
    int glyphSize;

    typedef struct CharMetrics_s
    {
        int advanceX;
        int advanceY;
        int offsetX;
        int offsetY;
        int bitmapWidth;
        int bitmapHeight;
        int bitmapTop;
        int bitmapLeft;

        ruVector2 texCoords[4]; // texture coords in atlas
    } CharMetrics;

    typedef struct ARGB8Pixel_s
    {
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char a;
    } ARGB8Pixel;

    static vector< BitmapFont* > fonts;
    vector< CharMetrics > charsMetrics;
    explicit BitmapFont( const char * file, int size );
    virtual ~BitmapFont( )
    {
        if( atlas )
            atlas->Release();
    }
    void RenderAtlas( EffectsQuad * quad );
    void RenderText( string text, int x, int y );
};