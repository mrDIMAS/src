#include "Precompiled.h"
#include "Engine.h"
#include "Skybox.h"
#include "Texture.h"
#include "Vertex.h"

void Skybox::WriteToIndexBuffer( IDirect3DIndexBuffer9 * ib, vector< unsigned short > & indices ) {
    void * indexData = 0;
    CheckDXErrorFatal( ib->Lock( 0, 0, &indexData, 0 ));
    memcpy( indexData, &indices[ 0 ], indices.size() * sizeof( unsigned short ) );
    CheckDXErrorFatal( ib->Unlock());
}

void Skybox::WriteToVertexBuffer( IDirect3DVertexBuffer9 * vb, vector< Vertex > & vertices ) {
    void * vertexData = 0;
    CheckDXErrorFatal( vb->Lock( 0, 0, &vertexData, 0 ));
    memcpy( vertexData, &vertices[ 0 ], vertices.size() * sizeof( Vertex ) );
    CheckDXErrorFatal( vb->Unlock());
}

void Skybox::CreateVertexBuffer( size_t size, IDirect3DVertexBuffer9 ** vb ) {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateVertexBuffer( size,D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_DEFAULT, vb, 0 ));
}

void Skybox::CreateIndexBuffer( size_t size, IDirect3DIndexBuffer9 ** ib ) {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateIndexBuffer( size,D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, ib, 0 ));
}

Skybox::~Skybox() {
    OnLostDevice();
}

Skybox::Skybox( const string & path ) {
    string file = path;

    string leftPath = file;
    leftPath += "_l.jpg";
    left = Texture::Require( leftPath.c_str() );

    string rightPath = file;
    rightPath += "_r.jpg";
    right = Texture::Require( rightPath.c_str() );

    string frontPath = file;
    frontPath += "_f.jpg";
    forw = Texture::Require( frontPath.c_str() );

    string backPath = file;
    backPath += "_b.jpg";
    back = Texture::Require( backPath.c_str() );

    string upPath = file;
    upPath += "_u.jpg";
    up = Texture::Require( upPath.c_str() );

    OnResetDevice();
}

void Skybox::Render( const btVector3 & pos ) {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetVertexShader ( nullptr ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetPixelShader ( nullptr ));

    D3DXMATRIX view;
    D3DXMatrixTranslation ( &view, pos.x(), pos.y(), pos.z() );
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTransform ( D3DTS_WORLD, &view ));

    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetIndices( ib ));
    forw->Bind( 0 );
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetStreamSource( 0, vbForw, 0, sizeof( Vertex )));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 ));
	Engine::Instance().RegisterDIP();

    back->Bind( 0 );
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetStreamSource( 0, vbBack, 0, sizeof( Vertex )));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 ));
	Engine::Instance().RegisterDIP();

    right->Bind( 0 );
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetStreamSource( 0, vbRight, 0, sizeof( Vertex )));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 ));
	Engine::Instance().RegisterDIP();

    left->Bind( 0 );
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetStreamSource( 0, vbLeft, 0, sizeof( Vertex )));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 ));
	Engine::Instance().RegisterDIP();

    up->Bind( 0 );
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetStreamSource( 0, vbUp, 0, sizeof( Vertex )));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 ));
	Engine::Instance().RegisterDIP();
}
