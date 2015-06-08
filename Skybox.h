#pragma once



class Skybox : public RendererComponent {
public:
    Texture * up;
    Texture * left;
    Texture * right;
    Texture * forw;
    Texture * back;

    IDirect3DVertexBuffer9 * vbUp;
    IDirect3DVertexBuffer9 * vbLeft;
    IDirect3DVertexBuffer9 * vbRight;
    IDirect3DVertexBuffer9 * vbForw;
    IDirect3DVertexBuffer9 * vbBack;

    IDirect3DIndexBuffer9 * ib;

    explicit Skybox( const string & path );
    virtual ~Skybox();
    void CreateIndexBuffer( size_t size, IDirect3DIndexBuffer9 ** ib );
    void CreateVertexBuffer( size_t size, IDirect3DVertexBuffer9 ** vb );
    void WriteToVertexBuffer( IDirect3DVertexBuffer9 * vb, vector< Vertex > & vertices );
    void WriteToIndexBuffer( IDirect3DIndexBuffer9 * ib, vector< unsigned short > & indices );

    void Render( const btVector3 & pos );

	void OnResetDevice() {
		float size = 2048.0f;

		vector< Vertex > fV;
		vector< Vertex > bV;
		vector< Vertex > uV;
		vector< Vertex > rV;
		vector< Vertex > lV;

		fV.push_back ( Vertex ( -size,  size, -size, 0.0f, 0.0f ) );
		fV.push_back ( Vertex (  size,  size, -size, 1.0f, 0.0f ) );
		fV.push_back ( Vertex (  size, -size, -size, 1.0f, 1.0f ) );
		fV.push_back ( Vertex ( -size, -size, -size, 0.0f, 1.0f ) );

		bV.push_back ( Vertex (  size,  size,  size, 0.0f, 0.0f ) );
		bV.push_back ( Vertex ( -size,  size,  size, 1.0f, 0.0f ) );
		bV.push_back ( Vertex ( -size, -size,  size, 1.0f, 1.0f ) );
		bV.push_back ( Vertex (  size, -size,  size, 0.0f, 1.0f ) );

		rV.push_back ( Vertex (  size,  size, -size, 0.0f, 0.0f ) );
		rV.push_back ( Vertex (  size,  size,  size, 1.0f, 0.0f ) );
		rV.push_back ( Vertex (  size, -size,  size, 1.0f, 1.0f ) );
		rV.push_back ( Vertex (  size, -size, -size, 0.0f, 1.0f ) );

		lV.push_back ( Vertex ( -size,  size,  size, 0.0f, 0.0f ) );
		lV.push_back ( Vertex ( -size,  size, -size, 1.0f, 0.0f ) );
		lV.push_back ( Vertex ( -size, -size, -size, 1.0f, 1.0f ) );
		lV.push_back ( Vertex ( -size, -size,  size, 0.0f, 1.0f ) );

		uV.push_back ( Vertex ( -size,  size,  size, 0.0f, 0.0f ) );
		uV.push_back ( Vertex (  size,  size,  size, 1.0f, 0.0f ) );
		uV.push_back ( Vertex (  size,  size, -size, 1.0f, 1.0f ) );
		uV.push_back ( Vertex ( -size,  size, -size, 0.0f, 1.0f ) );

		CreateVertexBuffer( fV.size() * sizeof( Vertex ), &vbForw );
		WriteToVertexBuffer( vbForw, fV );

		CreateVertexBuffer( bV.size() * sizeof( Vertex ), &vbBack );
		WriteToVertexBuffer( vbBack, bV );

		CreateVertexBuffer( rV.size() * sizeof( Vertex ), &vbRight );
		WriteToVertexBuffer( vbRight, rV );

		CreateVertexBuffer( lV.size() * sizeof( Vertex ), &vbLeft );
		WriteToVertexBuffer( vbLeft, lV );

		CreateVertexBuffer( uV.size() * sizeof( Vertex ), &vbUp );
		WriteToVertexBuffer( vbUp, uV );

		vector< unsigned short > indices;
		indices.push_back ( 0 );
		indices.push_back ( 1 );
		indices.push_back ( 2 );
		indices.push_back ( 0 );
		indices.push_back ( 2 );
		indices.push_back ( 3 );

		CreateIndexBuffer( sizeof( Vertex ) * indices.size(), &ib );
		WriteToIndexBuffer( ib, indices );
	}
	void OnLostDevice() {
		vbUp->Release();
		vbLeft->Release();
		vbRight->Release();
		vbForw->Release();
		vbBack->Release();
		ib->Release();
	}
};
