#pragma once

class Texture {
private:
    explicit Texture( );
    IDirect3DTexture9 * mTexture;
    int mHeight;
    int mWidth;
    int mColorDepth;
public:
	int GetHeight();
	int GetWidth();
    string name;
    static unordered_map< string, Texture* > msTextureList;
    virtual ~Texture( );
    void Bind( int level );
    IDirect3DTexture9 * GetInterface();
    static Texture * Require( string file );
    static void DeleteAll();
};