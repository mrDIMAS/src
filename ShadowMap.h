#pragma once

#include "Common.h"
#include "Light.h"
#include "Shader.h"
#include "Mesh.h"
#include "Utility.h"

class ShadowMap {
private:
    IDirect3DTexture9 * spotShadowMap;
    IDirect3DCubeTexture9 * pointShadowMap;
    IDirect3DSurface9 * spotSurface;
    PixelShader * pixelShader;
    VertexShader * vertexShader;
public:
    ShadowMap() {
        // create shadow maps
        D3DXCreateTexture( g_device, g_width, g_height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &spotShadowMap );
        pointShadowMap = nullptr; // fix

        // get surfaces
        spotShadowMap->GetSurfaceLevel( 0, &spotSurface );

        // create shader to render shadowmaps
        string vertexShaderSource = 
            "float4x4 wvp : register( c0 );\n"

            "struct VSOutput {\n"
            "   float4 position : POSITION;\n"
            "   float4 screenPosition : TEXCOORD0;\n"
            "};\n"

            "VSOutput main( float4 position : POSITION ) {\n"
            "   VSOutput output = (VSOutput)0;\n"
            "   output.position = mul( position, wvp );\n"
            "   output.screenPosition = output.position;\n"
            "   return output;\n"
            "};\n";
        vertexShader = new VertexShader( vertexShaderSource );

        string pixelShaderSource = 
            "float4 main( float4 screenPosition : TEXCOORD0 ) : COLOR0 {\n"
            "   return float4( screenPosition.z / screenPosition.w, 0.0f, 0.0f, 1.0f );\n"
            "};\n";

        pixelShader = new PixelShader( pixelShaderSource );
    }

    ~ShadowMap() {
        spotShadowMap->Release();
        delete pixelShader;
        delete vertexShader;
    }

    void RenderSpotShadowMap( IDirect3DSurface9 * lastUsedRT, int rtIndex, Light * spotLight ) {
        IDirect3DStateBlock9 * state;
        g_device->CreateStateBlock( D3DSBT_ALL, &state );

        g_device->SetRenderTarget( 0, spotSurface );
        //g_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
        g_device->Clear( 0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
        
        g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
        g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
        g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        g_device->SetRenderState( D3DRS_ZENABLE, FALSE );
        g_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
        g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
        g_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

        pixelShader->Bind();
        vertexShader->Bind();

        spotLight->BuildSpotProjectionMatrix();
        // each group is a pair< IDirect3DTexture9*, vector< Mesh*>>
        for( auto meshGroupIter : Mesh::meshes ) { // brute force method 
            auto & group = meshGroupIter.second;
            for( auto mesh : group ) {
                D3DXMATRIX world, wvp; 
                GetD3DMatrixFromBulletTransform( mesh->parent->globalTransform, world );
                D3DXMatrixMultiplyTranspose( &wvp, &world, &spotLight->spotViewProjectionMatrix );
                //D3DXMatrixMultiplyTranspose( &wvp, &world, &g_camera->viewProjection );
                g_device->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 );
                mesh->BindBuffers();
                mesh->Render();
            };
        }

        state->Apply();
        state->Release();

        // revert to the last used render target
        g_device->SetRenderTarget( rtIndex, lastUsedRT );
    }

    void BindSpotShadowMap( int index ) {
        g_device->SetTexture( index, spotShadowMap );
    }

    void UnbindSpotShadowMap( int index ) {
        g_device->SetTexture( index, nullptr );
    }
};