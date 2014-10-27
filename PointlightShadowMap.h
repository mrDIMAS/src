#pragma once

#include "Common.h"
#include "Light.h"
#include "Mesh.h"
#include "Utility.h"
#include "Shader.h"

class PointlightShadowMap {
private:
    IDirect3DSurface9 * defaultDepthStencil;
    IDirect3DSurface9 * depthStencil;
    IDirect3DCubeTexture9 * shadowCube;
    IDirect3DSurface9 * cubeFaces[ 6 ];
    vector<Mesh*> lightedMeshes;
    PixelShader * pixelShader;
    VertexShader * vertexShader;
public:
    explicit PointlightShadowMap( int faceSize = 256 ) {
        D3DXCreateCubeTexture( g_device, faceSize, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &shadowCube );
        shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_NEGATIVE_X, 0, &cubeFaces[ D3DCUBEMAP_FACE_NEGATIVE_X ] );
        shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_POSITIVE_X, 0, &cubeFaces[ D3DCUBEMAP_FACE_POSITIVE_X ] );
        shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_NEGATIVE_Y, 0, &cubeFaces[ D3DCUBEMAP_FACE_NEGATIVE_Y ] );
        shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_POSITIVE_Y, 0, &cubeFaces[ D3DCUBEMAP_FACE_POSITIVE_Y ] );
        shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_NEGATIVE_Z, 0, &cubeFaces[ D3DCUBEMAP_FACE_NEGATIVE_Z ] );
        shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_POSITIVE_Z, 0, &cubeFaces[ D3DCUBEMAP_FACE_POSITIVE_Z ] );
        g_device->GetDepthStencilSurface( &defaultDepthStencil );
        g_device->CreateDepthStencilSurface( faceSize, faceSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &depthStencil, nullptr );

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
    }

    ~PointlightShadowMap() {
        for( int i = 0; i < 6; i++ ) {
            cubeFaces[i]->Release();
        }
        shadowCube->Release();
        depthStencil->Release();
    }

    void BindShadowCubemap( int level ) {
        g_device->SetTexture( level, shadowCube );
    }

    void UnbindShadowCubemap( int level ) {
        g_device->SetTexture( level, nullptr );
    }

    void RenderPointShadowMap( IDirect3DSurface9 * prevRT, int prevRTNum, Light * light ) {
        IDirect3DStateBlock9 * state;
        g_device->CreateStateBlock( D3DSBT_ALL, &state );
        // set new suitable depth stencil surface
        g_device->SetDepthStencilSurface( depthStencil );
        // set render states
        g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
        g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
        g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        g_device->SetRenderState( D3DRS_ZENABLE, TRUE );
        g_device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
        g_device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
        // bind depth shaders
        pixelShader->Bind();
        vertexShader->Bind();
        // collect meshes that are in point light bounding sphere
        for( auto meshGroupIter : Mesh::meshes ) { // brute force method 
            auto & group = meshGroupIter.second;
            for( auto mesh : group ) {
                if( mesh->ownerNode->IsVisible()) {
                    if( (( mesh->aabb.center + mesh->ownerNode->GetPosition()) - light->GetPosition()).Length() < light->radius ) {
                        lightedMeshes.push_back( mesh );
                    }
                }
            };
        }
        // render each face of the cube map
        D3DXMATRIX proj;
        D3DXMatrixPerspectiveFovRH( &proj, SIMD_HALF_PI, 1.0f, 0.1f, 6000.0f );
        for( int i = 0; i < 6; i++ ) {
            D3DXMATRIX view, viewProj;
            D3DXVECTOR3 lookAt, up;
            if( i == D3DCUBEMAP_FACE_POSITIVE_X ) {
                lookAt = D3DXVECTOR3( 1, 0, 0 );
                up = D3DXVECTOR3( 0, 1, 0 );
            }
            if( i == D3DCUBEMAP_FACE_NEGATIVE_X ) {
                lookAt = D3DXVECTOR3( -1, 0, 0 );
                up = D3DXVECTOR3( 0, 1, 0 );
            }
            if( i == D3DCUBEMAP_FACE_POSITIVE_Y ) {
                lookAt = D3DXVECTOR3( 0, 1, 0 );
                up = D3DXVECTOR3( 0, 0, -1 );
            }
            if( i == D3DCUBEMAP_FACE_NEGATIVE_Y ) {
                lookAt = D3DXVECTOR3( 0, -1, 0 );
                up = D3DXVECTOR3( 0, 0, 1 );
            }
            if( i == D3DCUBEMAP_FACE_POSITIVE_Z ) {
                lookAt = D3DXVECTOR3( 0, 0, 1 );
                up = D3DXVECTOR3( 0, 1, 0 );
            }
            if( i == D3DCUBEMAP_FACE_NEGATIVE_Z ) {
                lookAt = D3DXVECTOR3( 0, 0, -1 );
                up = D3DXVECTOR3( 0, 1, 0 );
            }
            D3DXMatrixLookAtRH( &view, &D3DXVECTOR3( light->globalTransform.getOrigin().m_floats ), &lookAt, &up );
            D3DXMatrixMultiply( &viewProj, &view, &proj );
            // bind i-face of cube map
            g_device->SetRenderTarget( 0, cubeFaces[i] );
            // clear it
            g_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
            // render all lighted meshes 
            for( auto mesh : lightedMeshes ) {
                D3DXMATRIX world, wvp; 
                GetD3DMatrixFromBulletTransform( mesh->ownerNode->globalTransform, world );
                D3DXMatrixMultiplyTranspose( &wvp, &world, &viewProj );
                g_device->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 );
                mesh->BindBuffers();
                mesh->Render();
            };            
        }

        state->Apply();
        state->Release();

        // revert to the last used render target
        g_device->SetRenderTarget( prevRTNum, prevRT );
        g_device->SetDepthStencilSurface( defaultDepthStencil );

        lightedMeshes.clear();
    }
};