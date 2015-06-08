#pragma once

class Texture : public RendererComponent {
private:
    explicit Texture( );
    IDirect3DTexture9 * mTexture;
    int mHeight;
    int mWidth;
    int mColorDepth;
	string mName;
	void LoadFromFile( const string & file );
public:
	int GetHeight();
	int GetWidth();
    
    static unordered_map< string, Texture* > msTextureList;
    virtual ~Texture( );
    void Bind( int level );
    IDirect3DTexture9 * GetInterface();
    static Texture * Require( string file );
    static void DeleteAll();
	void OnResetDevice();
	void OnLostDevice();
};