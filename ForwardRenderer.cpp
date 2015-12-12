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

    for( auto group : mRenderList ) {
        IDirect3DTexture9 * diffuseTexture = group.first;
        vector< Mesh* > & meshes = group.second;

        Engine::Instance().GetDevice()->SetTexture( 0, diffuseTexture );

        for( auto pMesh : meshes ) {
            D3DXMATRIX world, wvp;

			// draw instances
			for( auto pOwner : pMesh->GetOwners() ) {
				if( pOwner->IsVisible() ) {
					GetD3DMatrixFromBulletTransform( pOwner->mGlobalTransform, world );
					D3DXMatrixMultiplyTranspose( &wvp, &world, &Camera::msCurrentCamera->mViewProjection );

					Engine::Instance().GetDevice()->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 );
					Engine::Instance().GetDevice()->SetPixelShaderConstantF( 0, &pMesh->mOpacity, 1 );

					pMesh->Render();
				}
			}
        }
    }
}

void ForwardRenderer::RemoveMesh( Mesh * mesh ) {
    auto groupIter = mRenderList.find( mesh->mDiffuseTexture->GetInterface() );
    if( groupIter != mRenderList.end() ) {
        auto & group = groupIter->second;
        group.erase( find( group.begin(), group.end(), mesh ));
		if( group.size() == 0 ) {
			mRenderList.erase( groupIter );
		}
    }
}

void ForwardRenderer::AddMesh( Mesh * mesh ) {
    mRenderList[ mesh->mDiffuseTexture->GetInterface() ].push_back( mesh );
}

ForwardRenderer::~ForwardRenderer() {
	vector<Mesh*> meshList;
	for( auto group : mRenderList ) {
		vector< Mesh* > & meshes = group.second;
		for( auto pMesh : meshes ) {
			meshList.push_back( pMesh );//delete pMesh;
		}
	}
	for( auto pMesh : meshList ) {
		delete pMesh;
	}
}

ForwardRenderer::ForwardRenderer() {
    mVertexShader = make_shared<VertexShader>( "data/shaders/forwardTransparent.vso" );
    mPixelShader = make_shared<PixelShader>( "data/shaders/forwardTransparent.pso" );
}
