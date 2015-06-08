#pragma once

class CubeTexture : public RendererComponent {
private:
	string mSourceName;
	void Load();
public:
    IDirect3DCubeTexture9 * cubeTexture;
    explicit CubeTexture( string fn );
    virtual ~CubeTexture();
    static CubeTexture * Require( string fn );
    static map< string, CubeTexture*> all;
	void OnResetDevice();
	void OnLostDevice();
};