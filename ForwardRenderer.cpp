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
#include "Engine.h"
#include "ForwardRenderer.h"

void ForwardRenderer::Render() {
	pD3D->SetRenderState( D3DRS_ZENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	pD3D->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    mTransparentPixelShader->Bind();
    mTransparentVertexShader->Bind();

	for( auto texGroupPairIter = mRenderList.begin(); texGroupPairIter != mRenderList.end(); ) {
		auto & meshGroup = (*texGroupPairIter).second;
		if( meshGroup.size()) {
			for( auto meshIter = meshGroup.begin(); meshIter != meshGroup.end(); ) {
				if( (*meshIter).use_count() ) {				
					++meshIter;
				} else {
					meshIter = meshGroup.erase( meshIter );
				}
			}
			++texGroupPairIter;
		} else {
			texGroupPairIter = mRenderList.erase( texGroupPairIter );
		}
	}

    for( auto & texGroupPair : mRenderList ) {
        IDirect3DTexture9 * diffuseTexture = texGroupPair.first;
        auto & meshGroup = texGroupPair.second;
		if( meshGroup.size() == 0 ) {
			continue;
		}
        pD3D->SetTexture( 0, diffuseTexture );
        for( auto & meshIter = meshGroup.begin(); meshIter != meshGroup.end(); ) {
			shared_ptr<Mesh> pMesh = (*meshIter).lock();
			if( pMesh ) {
				
				// draw instances
				auto & owners = pMesh->GetOwners();
				for( auto ownerIter = owners.begin(); ownerIter != owners.end(); ) {
					shared_ptr<SceneNode> pOwner = (*ownerIter).lock();
					if( pOwner ) {
						if( pOwner->IsVisible() ) {
							D3DXMATRIX world, wvp;
							world = pOwner->GetWorldMatrix();
							shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
							if( camera ) {
								D3DXMatrixMultiplyTranspose( &wvp, &world, &camera->mViewProjection );
							}

							pD3D->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 );
							
							GPURegister psFloatConstants[] = {
								{ pMesh->GetOpacity(), 0.0f, 0.0f, 0.0f }
							};

							pD3D->SetPixelShaderConstantF( 0, (float*)psFloatConstants, 1 );


							pMesh->Render();
						}
						++ownerIter;
					} else {
						ownerIter = owners.erase( ownerIter );
					}
				}
				++meshIter;
			} else {
				meshIter = meshGroup.erase( meshIter );
			}
        }
    }
}

void ForwardRenderer::AddMesh( shared_ptr<Mesh> mesh ) {
	if( mesh->GetDiffuseTexture() ) {
		mRenderList[ mesh->GetDiffuseTexture()->GetInterface() ].push_back( mesh );
	}
}

ForwardRenderer::~ForwardRenderer() {

}

ForwardRenderer::ForwardRenderer() {
    mTransparentVertexShader = make_shared<VertexShader>( "data/shaders/forwardTransparent.vso" );
    mTransparentPixelShader = make_shared<PixelShader>( "data/shaders/forwardTransparent.pso" );
}
