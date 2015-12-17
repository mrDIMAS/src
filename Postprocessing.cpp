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
#include "Postprocessing.h"
#include "Shader.h"
#include "Engine.h"
#include "Utility.h"
#include "Mesh.h"
#include "Camera.h"

Postprocessing::Postprocessing() {
	mMaskPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/postprocessingMask.pso" ));
	mBlurShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/postprocessing.pso" ));
	mBlurVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/postprocessing.vso" ));
	mBlurSkinVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/postprocessingSkin.vso" ));
	mEffectsQuad = unique_ptr<EffectsQuad>( new EffectsQuad );

	OnResetDevice();
}

Postprocessing::~Postprocessing() {
	OnLostDevice();
}

void Postprocessing::RenderMask() {
	Engine::I().GetDevice()->SetRenderTarget( 0, mBlurMaskSurface );
	Engine::I().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

	mMaskPixelShader->Bind();
	

	D3DXMATRIX world, vwp;
	for( auto pMeshGroup : Mesh::msMeshList ) {
		for( auto pMesh : pMeshGroup.second ) {
			for( auto pOwner : pMesh->GetOwners() ) {
				if( pOwner->mBlurAmount > 0.0f ) {
					bool visible = true;
					if( pOwner->mIsBone ) {
						visible = false;
					} else {
						visible = pOwner->IsVisible();
					}
					pOwner->mInFrustum |= Camera::msCurrentCamera->mFrustum.IsAABBInside( pMesh->GetBoundingBox(), ruVector3( pOwner->mGlobalTransform.getOrigin().m_floats ));
					if( visible && ( pOwner->mInFrustum || pOwner->mIsSkinned ) ) {
						if( pMesh->IsSkinned() ) {
							D3DXMatrixIdentity( &world );
							mBlurSkinVertexShader->Bind();
						} else {
							D3DXMATRIX scale;
							D3DXMatrixScaling( &scale, 1.07f, 1.07f, 1.07f );
							GetD3DMatrixFromBulletTransform( pOwner->mGlobalTransform, world );
							D3DXMatrixMultiply( &world, &scale,  &world );
							mBlurVertexShader->Bind();
						}
						D3DXMatrixMultiply( &vwp, &world, &Camera::msCurrentCamera->mViewProjection );

						// pass vertex shader matrices
						Engine::I().SetVertexShaderMatrix( 0, &vwp );

						SceneNode * parent = pOwner->mParent;
						pOwner->mParent = nullptr; 

						auto & bones = pMesh->GetBones();
						for( int i = 0; i < bones.size(); i++ ) {
							Mesh::Bone * bone = bones[i];
							bone->mNode->mIsBone = true;
							btTransform transform = (bone->mNode->mGlobalTransform * bone->mNode->mInvBoneBindTransform) * pOwner->CalculateGlobalTransform();
							GetD3DMatrixFromBulletTransform( transform, bones[i]->mMatrix );
							Engine::I().SetVertexShaderMatrix( 4 + i * 4, &bones[i]->mMatrix );
						}

						pOwner->mParent = parent;

						pMesh->Render();
					}
				}
			}
		}
	}
}

void Postprocessing::DoPostprocessing( IDirect3DSurface9 * renderTarget, IDirect3DTexture9 * frameTexture ) {
	 static float t = 0;
	 t += 0.025f;

	 Engine::I().GetDevice()->SetRenderTarget( 0, renderTarget );
	 Engine::I().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

	 Engine::I().GetDevice()->SetTexture( 0, frameTexture );
	 Engine::I().GetDevice()->SetTexture( 1, mBlurMask );

	 Engine::I().SetPixelShaderFloat3( 0, 1.0f / Engine::I().GetResolutionWidth(), 1.0f / Engine::I().GetResolutionHeight(), 0.0f );
	 Engine::I().SetPixelShaderFloat( 1, t );

	 mEffectsQuad->Bind();
	 mBlurShader->Bind();

	 mEffectsQuad->Render();
}

void Postprocessing::OnLostDevice() {
	mBlurMaskSurface->Release();
	mBlurMask->Release();
}

void Postprocessing::OnResetDevice() {
	int width = Engine::I().GetResolutionWidth();
	int height = Engine::I().GetResolutionHeight();
	Engine::I().GetDevice()->CreateTexture( width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mBlurMask, nullptr );
	mBlurMask->GetSurfaceLevel( 0, &mBlurMaskSurface );	
}
