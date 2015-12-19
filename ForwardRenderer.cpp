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

void ForwardRenderer::RenderMeshes() {
    mPixelShader->Bind();
    mVertexShader->Bind();

    for( auto texGroupPair : mRenderList ) {
        IDirect3DTexture9 * diffuseTexture = texGroupPair.first;
        auto & meshGroup = texGroupPair.second;
        Engine::I().GetDevice()->SetTexture( 0, diffuseTexture );
        for( auto meshIter = meshGroup.begin(); meshIter != meshGroup.end(); ) {
			shared_ptr<Mesh> pMesh = (*meshIter).lock();
			if( pMesh ) {
				D3DXMATRIX world, wvp;
				// draw instances
				auto & owners = pMesh->GetOwners();
				for( auto ownerIter = owners.begin(); ownerIter != owners.end(); ) {
					shared_ptr<SceneNode> pOwner = (*ownerIter).lock();
					if( pOwner ) {
						if( pOwner->IsVisible() ) {
							GetD3DMatrixFromBulletTransform( pOwner->mGlobalTransform, world );
							shared_ptr<Camera> camera = Camera::msCurrentCamera.lock();
							if( camera ) {
								D3DXMatrixMultiplyTranspose( &wvp, &world, &camera->mViewProjection );
							}
							Engine::I().SetVertexShaderMatrix( 0, &wvp );
							Engine::I().SetPixelShaderFloat( 0, pMesh->GetOpacity() );

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
    mRenderList[ mesh->GetDiffuseTexture()->GetInterface() ].push_back( mesh );
}

ForwardRenderer::~ForwardRenderer() {

}

ForwardRenderer::ForwardRenderer() {
    mVertexShader = make_shared<VertexShader>( "data/shaders/forwardTransparent.vso" );
    mPixelShader = make_shared<PixelShader>( "data/shaders/forwardTransparent.pso" );
}
