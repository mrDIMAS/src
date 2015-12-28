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
#include "MultipleRTDeferredRenderer.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "Utility.h"
#include "Camera.h"

void MultipleRTDeferredRenderer::OnEnd() {
    mGBuffer->UnbindRenderTargets();
}

void MultipleRTDeferredRenderer::RenderMesh( shared_ptr<Mesh> mesh ) {
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		D3DXMATRIX world, vwp;
		auto & owners = mesh->GetOwners();
		for( auto ownerIter = owners.begin(); ownerIter != owners.end();  ) {
			shared_ptr<SceneNode> & pOwner = (*ownerIter).lock();
			if( pOwner ) {
				bool visible = true;
				if( pOwner->IsBone() ) {
					visible = false;
				} else {
					visible = pOwner->IsVisible();
				}
				if( visible && ( pOwner->IsInFrustum() || pOwner->IsSkinned() ) ) {
					if( fabs( pOwner->GetDepthHack() ) > 0.001 ) {
						camera->EnterDepthHack( fabs( pOwner->GetDepthHack() ) );
					}			
					if( mesh->IsSkinned() ) {
						D3DXMatrixIdentity( &world );
					} else {
						world = pOwner->GetWorldMatrix();
					}
					D3DXMatrixMultiply( &vwp, &world, &camera->mViewProjection );
					// pass albedo
					Engine::I().SetPixelShaderFloat( 0, pOwner->GetAlbedo() );
					// pass far z plane
					Engine::I().SetPixelShaderFloat( 1, camera->mFarZ );
					// pass vertex shader matrices
					Engine::I().SetVertexShaderMatrix( 0, &world );
					Engine::I().SetVertexShaderMatrix( 5, &vwp );
		
					// for parallax
					Engine::I().SetVertexShaderVector3( 10, camera->GetPosition() );
			
					auto & bones = mesh->GetBones();
					int n = 0;
					for( auto * bone : bones ) {
						shared_ptr<SceneNode> boneNode = bone->mNode.lock();
						if( boneNode ) {
							bone->mMatrix = TransformToMatrix( boneNode->GetRelativeTransform() * pOwner->GetLocalTransform() );
							Engine::I().SetVertexShaderMatrix( 11 + n * 4, &bone->mMatrix );
						}
						n++;
					}
					
					mesh->Render();
					if( pOwner->GetDepthHack() ) {
						camera->LeaveDepthHack();
					}
				}
				++ownerIter;
			} else {
				ownerIter = owners.erase( ownerIter );
			}
		}
	}
}

void MultipleRTDeferredRenderer::BeginFirstPass() {
    mGBuffer->BindRenderTargets();
    Engine::I().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
}

MultipleRTDeferredRenderer::MultipleRTDeferredRenderer( ) {
	// Parallax occlusion mapping shaders
	mVertexShaderPOM = std::move( unique_ptr<VertexShader>( new VertexShader( "data/shaders/deferredGBufferPOM.vso" )));
	mPixelShaderPOM = std::move( unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredGBufferPOM.pso" )));
	// Standard GBuffer shader
	mVertexShader = std::move( unique_ptr<VertexShader>( new VertexShader( "data/shaders/deferredGBuffer.vso" )));
	mPixelShader = std::move( unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredGBuffer.pso" )));
	// Standard GBuffer shader with skinning
	mVertexShaderSkin = std::move( unique_ptr<VertexShader>( new VertexShader( "data/shaders/deferredGBufferSkin.vso" )));
	mPixelShaderSkin = std::move( unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredGBufferSkin.pso" )));
}

MultipleRTDeferredRenderer::~MultipleRTDeferredRenderer() {

}

void MultipleRTDeferredRenderer::BindGenericSkinShaders() {
	mPixelShaderSkin->Bind();
	mVertexShaderSkin->Bind();
}

void MultipleRTDeferredRenderer::BindGenericShaders() {
	mPixelShader->Bind();
	mVertexShader->Bind();
}

void MultipleRTDeferredRenderer::BindParallaxShaders() {
	mPixelShaderPOM->Bind();
	mVertexShaderPOM->Bind();
}
