#pragma once

#include "Common.h"

class Texture
{
private:
    explicit Texture( );
    IDirect3DTexture9 * texture;
    int height;
    int width;
    int bpp;
public:
    string name;
    static unordered_map< string, Texture* > msTextureList;
    virtual ~Texture( );
    void Bind( int level );
    IDirect3DTexture9 * GetInterface();
    static Texture * Require( string file );
    static void DeleteAll();
};