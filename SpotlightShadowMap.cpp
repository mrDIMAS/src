/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#include "Precompiled.h"

#include "SpotlightShadowMap.h"
#include "Engine.h"

void SpotlightShadowMap::UnbindSpotShadowMap( int index ) {
    Engine::Instance().GetDevice()->SetTexture( index, nullptr );
}

void SpotlightShadowMap::BindSpotShadowMap( int index ) {
    Engine::Instance().GetDevice()->SetTexture( index, spotShadowMap );
}

void SpotlightShadowMap::RenderSpotShadowMap( IDirect3DSurface9 * lastUsedRT, int rtIndex, Light * spotLight ) {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, spotSurface );
    Engine::Instance().GetDevice()->SetDepthStencilSurface( depthStencil );
    Engine::Instance().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    pixelShader->Bind();
    vertexShader->Bind();

    spotLight->BuildSpotProjectionMatrixAndFrustum();
    IDirect3DBaseTexture9 * prevZeroSamplerTexture = nullptr;
    Engine::Instance().GetDevice()->GetTexture( 0, &prevZeroSamplerTexture );

    for( auto meshGroupIter : Mesh::msMeshList ) {
        auto & group = meshGroupIter.second;
        Engine::Instance().GetDevice()->SetTexture( 0, meshGroupIter.first );
        for( auto mesh : group ) {
			for( auto pOwner : mesh->GetOwners() ) {
				// if owner of mesh is visible
				if( pOwner->IsVisible()) {
					// if light "sees" mesh, it can cast shadow
					if( spotLight->mFrustum.IsAABBInside( mesh->mAABB, pOwner->GetPosition())) {
						// if mesh in light range, it can cast shadow
						//if( (mesh->ownerNode->GetPosition() + mesh->aabb.center - spotLight->GetPosition()).Length2() < spotLight->radius * spotLight->radius ) {
						D3DXMATRIX world, wvp;
						GetD3DMatrixFromBulletTransform( pOwner->mGlobalTransform, world );
						D3DXMatrixMultiplyTranspose( &wvp, &world, &spotLight->mSpotViewProjectionMatrix );
						Engine::Instance().GetDevice()->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 );

						mesh->Render();
						//}
					}
				}
			}
        };
    }

    Engine::Instance().GetDevice()->SetTexture( 0, prevZeroSamplerTexture );
	prevZeroSamplerTexture->Release();

    // revert to the last used render target
    Engine::Instance().GetDevice()->SetRenderTarget( rtIndex, lastUsedRT );
    Engine::Instance().GetDevice()->SetDepthStencilSurface( defaultDepthStencil );
}

SpotlightShadowMap::~SpotlightShadowMap() {
	OnLostDevice();
    delete pixelShader;
    delete vertexShader;
}

SpotlightShadowMap::SpotlightShadowMap( float size ) {
    iSize = static_cast<int>( size );
	Initialize();
	vertexShader = new VertexShader( "data/shaders/spotShadowMap.vso" );
	pixelShader = new PixelShader( "data/shaders/spotShadowMap.pso" );
}

void SpotlightShadowMap::OnLostDevice() {
	depthStencil->Release();
	spotSurface->Release();
	spotShadowMap->Release();	
	defaultDepthStencil->Release();
}

void SpotlightShadowMap::OnResetDevice() {
	Initialize();
}

void SpotlightShadowMap::Initialize() {
	// create shadow maps
	Engine::Instance().GetDevice()->CreateTexture( iSize, iSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &spotShadowMap, nullptr );

	// get surfaces
	spotShadowMap->GetSurfaceLevel( 0, &spotSurface );

	Engine::Instance().GetDevice()->GetDepthStencilSurface( &defaultDepthStencil );
	Engine::Instance().GetDevice()->CreateDepthStencilSurface( iSize, iSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &depthStencil, 0 );
}
