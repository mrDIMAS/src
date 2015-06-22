#pragma once



class Skybox : public RendererComponent {
private:
	struct SkyVertex {
		float x, y, z;
		float tx, ty;
	};
	Texture * mTextures[5];
	IDirect3DVertexBuffer9 * mVertexBuffer;
	IDirect3DIndexBuffer9 * mIndexBuffer;
	IDirect3DVertexDeclaration9 * mVertexDeclaration;
public:
    explicit Skybox( Texture * up, Texture * left, Texture * right, Texture * forward, Texture * back );
    virtual ~Skybox();
    void Render( );
	void OnResetDevice();
	void OnLostDevice();
};
