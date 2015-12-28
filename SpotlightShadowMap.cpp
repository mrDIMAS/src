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
    Engine::I().GetDevice()->SetTexture( index, nullptr );
}

void SpotlightShadowMap::BindSpotShadowMap( int index ) {
    Engine::I().GetDevice()->SetTexture( index, mShadowMap );
}

void SpotlightShadowMap::RenderSpotShadowMap( IDirect3DSurface9 * lastUsedRT, int rtIndex, const shared_ptr<SpotLight> & spotLight ) {
    Engine::I().GetDevice()->SetRenderTarget( 0, mRenderTarget );
    Engine::I().GetDevice()->SetDepthStencilSurface( mDepthStencil );
    Engine::I().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    mPixelShader->Bind();
    mVertexShader->Bind();

    spotLight->BuildSpotProjectionMatrixAndFrustum();
    IDirect3DBaseTexture9 * prevZeroSamplerTexture = nullptr;
    Engine::I().GetDevice()->GetTexture( 0, &prevZeroSamplerTexture );

	auto & meshMap = Mesh::GetMeshMap();
    for( auto & texGroupPair : meshMap ) {
        auto & group = texGroupPair.second;
        Engine::I().GetDevice()->SetTexture( 0, texGroupPair.first );
        for( auto meshIter = group.begin(); meshIter != group.end(); ) {
			shared_ptr<Mesh> mesh = (*meshIter).lock();
			if( mesh ) {
				auto & owners = mesh->GetOwners();
				for( auto ownerIter = owners.begin(); ownerIter != owners.end(); ) {
					shared_ptr<SceneNode> & pOwner = (*ownerIter).lock();
					if( pOwner ) {
						// if owner of mesh is visible
						if( pOwner->IsVisible()) {
							// if light "sees" mesh, it can cast shadow
							if( spotLight->GetFrustum().IsAABBInside( mesh->GetBoundingBox(), pOwner->GetPosition())) {
								D3DXMATRIX world, wvp;
								world = pOwner->GetWorldMatrix();
								D3DXMatrixMultiply( &wvp, &world, &spotLight->GetViewProjectionMatrix() );
								Engine::I().SetVertexShaderMatrix( 0, &wvp );
								mesh->Render();
							}
						}
						++ownerIter;
					} else {
						ownerIter = owners.erase( ownerIter );
					}
				}
				++meshIter;
			} else {
				meshIter = group.erase( meshIter );
			}
        };
    }

    Engine::I().GetDevice()->SetTexture( 0, prevZeroSamplerTexture );
	prevZeroSamplerTexture->Release();

    // revert to the last used render target
    Engine::I().GetDevice()->SetRenderTarget( rtIndex, lastUsedRT );
    Engine::I().GetDevice()->SetDepthStencilSurface( mDefaultDepthStencil );
}

SpotlightShadowMap::~SpotlightShadowMap() {
	OnLostDevice();
}

SpotlightShadowMap::SpotlightShadowMap( float size ) {
    iSize = static_cast<int>( size );
	Initialize();
	mVertexShader = std::move( unique_ptr<VertexShader>( new VertexShader( "data/shaders/spotShadowMap.vso" )));
	mPixelShader = std::move( unique_ptr<PixelShader>( new PixelShader( "data/shaders/spotShadowMap.pso" )));
}

void SpotlightShadowMap::OnLostDevice() {
	mDepthStencil.Reset();
	mRenderTarget.Reset();
	mShadowMap.Reset();	
	mDefaultDepthStencil.Reset();
}

void SpotlightShadowMap::OnResetDevice() {
	Initialize();
}

void SpotlightShadowMap::Initialize() {
	// create shadow maps
	Engine::I().GetDevice()->CreateTexture( iSize, iSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mShadowMap, nullptr );

	// get surfaces
	mShadowMap->GetSurfaceLevel( 0, &mRenderTarget );

	Engine::I().GetDevice()->GetDepthStencilSurface( &mDefaultDepthStencil );
	Engine::I().GetDevice()->CreateDepthStencilSurface( iSize, iSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &mDepthStencil, 0 );
}
