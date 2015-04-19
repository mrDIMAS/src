#include "SpotlightShadowMap.h"
#include "Renderer.h"

void SpotlightShadowMap::UnbindSpotShadowMap( int index ) {
    gpDevice->SetTexture( index, nullptr );
}

void SpotlightShadowMap::BindSpotShadowMap( int index ) {
    gpDevice->SetTexture( index, spotShadowMap );
}

void SpotlightShadowMap::RenderSpotShadowMap( IDirect3DSurface9 * lastUsedRT, int rtIndex, Light * spotLight ) {
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 0, spotSurface ));
    CheckDXErrorFatal( gpDevice->SetDepthStencilSurface( depthStencil ));
    CheckDXErrorFatal( gpDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 ));

    pixelShader->Bind();
    vertexShader->Bind();

    spotLight->BuildSpotProjectionMatrixAndFrustum();
    IDirect3DBaseTexture9 * prevZeroSamplerTexture = nullptr;
    CheckDXErrorFatal( gpDevice->GetTexture( 0, &prevZeroSamplerTexture ));

    for( auto meshGroupIter : Mesh::msMeshList ) {
        auto & group = meshGroupIter.second;
        CheckDXErrorFatal( gpDevice->SetTexture( 0, meshGroupIter.first ));
        for( auto mesh : group ) {
			for( auto pOwner : mesh->GetOwners() ) {
				// if owner of mesh is visible
				if( pOwner->IsVisible()) {
					// if light "sees" mesh, it can cast shadow
					if( spotLight->frustum.IsAABBInside( mesh->mAABB, pOwner->GetPosition())) {
						// if mesh in light range, it can cast shadow
						//if( (mesh->ownerNode->GetPosition() + mesh->aabb.center - spotLight->GetPosition()).Length2() < spotLight->radius * spotLight->radius ) {
						D3DXMATRIX world, wvp;
						GetD3DMatrixFromBulletTransform( pOwner->mGlobalTransform, world );
						D3DXMatrixMultiplyTranspose( &wvp, &world, &spotLight->spotViewProjectionMatrix );
						CheckDXErrorFatal( gpDevice->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 ));
						mesh->BindBuffers();
						mesh->Render();
						//}
					}
				}
			}
        };
    }

    CheckDXErrorFatal( gpDevice->SetTexture( 0, prevZeroSamplerTexture ));

    // revert to the last used render target
    CheckDXErrorFatal( gpDevice->SetRenderTarget( rtIndex, lastUsedRT ));
    CheckDXErrorFatal( gpDevice->SetDepthStencilSurface( defaultDepthStencil ));
}

SpotlightShadowMap::~SpotlightShadowMap() {
    spotSurface->Release();
    spotShadowMap->Release();
    depthStencil->Release();
    delete pixelShader;
    delete vertexShader;
}

SpotlightShadowMap::SpotlightShadowMap( float size ) {
    iSize = size;

    // create shadow maps
    CheckDXErrorFatal( D3DXCreateTexture( gpDevice, size, size, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &spotShadowMap ));

    // get surfaces
    CheckDXErrorFatal( spotShadowMap->GetSurfaceLevel( 0, &spotSurface ));

    vertexShader = new VertexShader( "data/shaders/spotShadowMap.vso", true );
    pixelShader = new PixelShader( "data/shaders/spotShadowMap.pso", true );

    CheckDXErrorFatal( gpDevice->GetDepthStencilSurface( &defaultDepthStencil ));
    CheckDXErrorFatal( gpDevice->CreateDepthStencilSurface( size, size, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &depthStencil, 0 ));
}
