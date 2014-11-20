#include "SpotlightShadowMap.h"
#include "Renderer.h"

void SpotlightShadowMap::UnbindSpotShadowMap( int index ) {
    g_device->SetTexture( index, nullptr );
}

void SpotlightShadowMap::BindSpotShadowMap( int index ) {
    g_device->SetTexture( index, spotShadowMap );
}

void SpotlightShadowMap::RenderSpotShadowMap( IDirect3DSurface9 * lastUsedRT, int rtIndex, Light * spotLight ) {
    IDirect3DStateBlock9 * state;
    CheckDXError( g_device->CreateStateBlock( D3DSBT_ALL, &state ));
    CheckDXError( g_device->SetRenderTarget( 0, spotSurface ));
    CheckDXError( g_device->SetDepthStencilSurface( depthStencil ));
    CheckDXError( g_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 ));
    CheckDXError( g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF ));
    CheckDXError( g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE ));
    CheckDXError( g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ));
    CheckDXError( g_device->SetRenderState( D3DRS_ZENABLE, TRUE ));
    CheckDXError( g_device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ));
    CheckDXError( g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));
    CheckDXError( g_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ));

    pixelShader->Bind();
    vertexShader->Bind();

    spotLight->BuildSpotProjectionMatrixAndFrustum();
    IDirect3DBaseTexture9 * prevZeroSamplerTexture = nullptr;
    CheckDXError( g_device->GetTexture( 0, &prevZeroSamplerTexture ));

    for( auto meshGroupIter : Mesh::meshes ) {
        auto & group = meshGroupIter.second;
        CheckDXError( g_device->SetTexture( 0, meshGroupIter.first ));
        for( auto mesh : group ) {
            // if owner of mesh is visible
            if( mesh->ownerNode->IsVisible()) {
                // if light "sees" mesh, it can cast shadow
                if( spotLight->frustum.IsAABBInside( mesh->aabb, mesh->ownerNode->GetPosition())) {
                    // if mesh in light range, it can cast shadow
                    //if( (mesh->ownerNode->GetPosition() + mesh->aabb.center - spotLight->GetPosition()).Length2() < spotLight->radius * spotLight->radius ) {
                        D3DXMATRIX world, wvp; 
                        GetD3DMatrixFromBulletTransform( mesh->ownerNode->globalTransform, world );
                        D3DXMatrixMultiplyTranspose( &wvp, &world, &spotLight->spotViewProjectionMatrix );
                        CheckDXError( g_device->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 ));
                        mesh->BindBuffers();
                        mesh->Render();
                    //}
                } 
            }
        };
    }

    CheckDXError( g_device->SetTexture( 0, prevZeroSamplerTexture ));

    CheckDXError( state->Apply());
    state->Release();

    // revert to the last used render target
    CheckDXError( g_device->SetRenderTarget( rtIndex, lastUsedRT ));
    CheckDXError( g_device->SetDepthStencilSurface( defaultDepthStencil ));
}

SpotlightShadowMap::~SpotlightShadowMap() {
    spotShadowMap->Release();
    depthStencil->Release();
    delete pixelShader;
    delete vertexShader;
}

SpotlightShadowMap::SpotlightShadowMap( float size ) {
    iSize = size;

    // create shadow maps
    CheckDXError( D3DXCreateTexture( g_device, size, size, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &spotShadowMap ));

    // get surfaces
    CheckDXError( spotShadowMap->GetSurfaceLevel( 0, &spotSurface ));

    // create shader to render shadowmaps
    string vertexShaderSource = 
        "float4x4 wvp : register( c0 );\n"

        "struct VSOutput {\n"
        "   float4 position : POSITION;\n"
        "   float4 screenPosition : TEXCOORD0;\n"
        "   float2 texCoord : TEXCOORD1;\n"
        "};\n"

        "VSOutput main( float4 position : POSITION, float2 texCoord : TEXCOORD0 ) {\n"
        "   VSOutput output = (VSOutput)0;\n"
        "   output.position = mul( position, wvp );\n"
        "   output.screenPosition = output.position;\n"
        "   output.texCoord = texCoord;\n"
        "   return output;\n"
        "};\n";
    vertexShader = new VertexShader( vertexShaderSource );

    string pixelShaderSource = 
        "sampler diffuseSampler : register( s0 );\n"
        "float4 main( float4 screenPosition : TEXCOORD0, float2 texCoord : TEXCOORD1 ) : COLOR0 {\n"
        "   clip( tex2D( diffuseSampler, texCoord ).a - 0.1 );\n"
        "   return float4( screenPosition.z / screenPosition.w, 0.0f, 0.0f, 1.0f );\n"
        "};\n";

    pixelShader = new PixelShader( pixelShaderSource );

    CheckDXError( g_device->GetDepthStencilSurface( &defaultDepthStencil ));
    CheckDXError( g_device->CreateDepthStencilSurface( size, size, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &depthStencil, 0 ));
}
