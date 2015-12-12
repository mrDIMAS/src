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

void MultipleRTDeferredRenderer::RenderMesh( Mesh * mesh ) {
    D3DXMATRIX world, vwp;
	for( auto pOwner : mesh->GetOwners() ) {
		bool visible = true;
		if( pOwner->mIsBone ) {
			visible = false;
		} else {
			visible = pOwner->IsVisible();
		}
		pOwner->mInFrustum |= Camera::msCurrentCamera->mFrustum.IsAABBInside( mesh->mAABB, ruVector3( pOwner->mGlobalTransform.getOrigin().m_floats ));
		if( visible && ( pOwner->mInFrustum || pOwner->mIsSkinned ) ) {
			if( fabs( pOwner->mDepthHack ) > 0.001 ) {
				Camera::msCurrentCamera->EnterDepthHack( fabs( pOwner->mDepthHack ) );
			}			
			if( mesh->mSkinned ) {
				D3DXMatrixIdentity( &world );
			} else {
				GetD3DMatrixFromBulletTransform( pOwner->mGlobalTransform, world );
			}
			D3DXMatrixMultiply( &vwp, &world, &Camera::msCurrentCamera->mViewProjection );
			// pass albedo
			Engine::Instance().SetPixelShaderFloat( 0, pOwner->mAlbedo );
			// pass far z plane
			Engine::Instance().SetPixelShaderFloat( 1, Camera::msCurrentCamera->mFarZ );
			// pass vertex shader matrices
			Engine::Instance().SetVertexShaderMatrix( 0, &world );
			Engine::Instance().SetVertexShaderMatrix( 5, &vwp );
		
			// for parallax
			Engine::Instance().SetVertexShaderVector3( 10, Camera::msCurrentCamera->GetPosition() );
			
			SceneNode * parent = pOwner->mParent; // HAX!
			pOwner->mParent = nullptr; // HAX!

			auto & bones = mesh->GetBones();
			for( int i = 0; i < bones.size(); i++ ) {
				Mesh::Bone * bone = bones[i];
				bone->mNode->mIsBone = true;
				btTransform transform = (bone->mNode->mGlobalTransform * bone->mNode->mInvBoneBindTransform) * pOwner->CalculateGlobalTransform();
				GetD3DMatrixFromBulletTransform( transform, bones[i]->mMatrix );
				Engine::Instance().SetVertexShaderMatrix( 11 + i * 4, &bones[i]->mMatrix );
			}

			pOwner->mParent = parent;

			mesh->Render();
			if( pOwner->mDepthHack ) {
				Camera::msCurrentCamera->LeaveDepthHack();
			}
		}
	}
}

void MultipleRTDeferredRenderer::BeginFirstPass() {
    mGBuffer->BindRenderTargets();
    Engine::Instance().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
}

MultipleRTDeferredRenderer::MultipleRTDeferredRenderer( ) {
	// Parallax occlusion mapping shaders
	mVertexShaderPOM = new VertexShader( "data/shaders/deferredGBufferPOM.vso" );
	mPixelShaderPOM = new PixelShader( "data/shaders/deferredGBufferPOM.pso" );
	// Standard GBuffer shader
	mVertexShader = new VertexShader( "data/shaders/deferredGBuffer.vso" );
	mPixelShader = new PixelShader( "data/shaders/deferredGBuffer.pso" );
	// Standard GBuffer shader with skinning
	mVertexShaderSkin = new VertexShader( "data/shaders/deferredGBufferSkin.vso" );
	mPixelShaderSkin = new PixelShader( "data/shaders/deferredGBufferSkin.pso" );
}

MultipleRTDeferredRenderer::~MultipleRTDeferredRenderer() {
    delete mVertexShader;
    delete mPixelShader;
	delete mVertexShaderPOM;
	delete mPixelShaderPOM;
	delete mPixelShaderSkin;
	delete mVertexShaderSkin;
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
