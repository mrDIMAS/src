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

#include "ParticleSystemRenderer.h"
#include "Camera.h"
#include "Engine.h"
#include "SceneFactory.h"
#include "PointLight.h"
#include "SpotLight.h"

ParticleSystemRenderer::ParticleSystemRenderer() {
    mVertexShader = std::move( unique_ptr<VertexShader>( new VertexShader( "data/shaders/particle.vso" )));
    mPixelShader = std::move( unique_ptr<PixelShader>( new PixelShader( "data/shaders/particle.pso" )));
	mVertexShaderLighting = std::move( unique_ptr<VertexShader>( new VertexShader( "data/shaders/particleLighting.vso" )));

    D3DVERTEXELEMENT9 vdElem[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    pD3D->CreateVertexDeclaration( vdElem, &mVertexDeclaration );
}

ParticleSystemRenderer::~ParticleSystemRenderer() {
    mVertexDeclaration.Reset();
}

void ParticleSystemRenderer::Render() {
	pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pD3D->SetRenderState( D3DRS_STENCILENABLE, FALSE );

	static vector<weak_ptr<Light>> lightList;

    mPixelShader->Bind();

    pD3D->SetVertexDeclaration( mVertexDeclaration );
    mVertexShader->Bind();

	auto & particleSystems = SceneFactory::GetParticleSystemList();
    for( auto pWeak : particleSystems ) {
		shared_ptr<ParticleSystem> & particleEmitter = pWeak.lock();
		if( particleEmitter ) {
			if( !particleEmitter->IsVisible() || !particleEmitter->IsEnabled() || !particleEmitter->HasAliveParticles() ) {
				continue;
			}

			particleEmitter->Update();
			particleEmitter->Bind();

			shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
			if( camera ) {
				if( fabs( particleEmitter->GetDepthHack() ) > 0.001 ) {
					camera->EnterDepthHack( fabs( particleEmitter->GetDepthHack() ));
				}

				if( particleEmitter->mUseLighting ) {
					mVertexShaderLighting->Bind();
				}

				D3DXMATRIX mWVP;
				D3DXMatrixMultiply( &mWVP, &particleEmitter->mWorldTransform, &camera->mViewProjection );


				pD3D->SetVertexShaderConstantF( 0, &mWVP.m[0][0], 4 );
				pD3D->SetVertexShaderConstantF( 4, &particleEmitter->mWorldTransform.m[0][0], 4 );
				
				if( particleEmitter->mUseLighting ) {		
					lightList.clear();

					auto & pointLights = SceneFactory::GetPointLightList();
					for( auto & lWeak : pointLights ) {
						shared_ptr<PointLight> & pLight = lWeak.lock();
						if( pLight ) {
							if( (pLight->GetPosition() - particleEmitter->GetPosition()).Length() < ( 1.25f * pLight->GetRange() + particleEmitter->mBoundingRadius )) {
								lightList.push_back( pLight );
							}
						}
					}

					GPURegister ambientColorConstant = {
						pEngine->GetAmbientColor().x, pEngine->GetAmbientColor().y, pEngine->GetAmbientColor().z, lightList.size()
					};
					pD3D->SetVertexShaderConstantF( 57, (float*)&ambientColorConstant, 1 );

					int lightNumber = 0;
					for( auto & lWeak : lightList ) {
						shared_ptr<Light> & pLight = lWeak.lock();
						if( pLight ) {

							GPURegister lightPositionConstant = { pLight->GetPosition().x, pLight->GetPosition().y, pLight->GetPosition().z, 1.0f	};
							pD3D->SetVertexShaderConstantF( 9 + lightNumber, (float*)&lightPositionConstant, 1);

							GPURegister lightColorConstant = { pLight->GetColor().x, pLight->GetColor().y, pLight->GetColor().z, 1.0f };
							pD3D->SetVertexShaderConstantF( 25 + lightNumber, (float*)&lightColorConstant, 1);

							GPURegister lightRangeConstant = { pLight->GetRange(), 1.0f, 1.0f, 1.0f	};
							pD3D->SetVertexShaderConstantF( 41 + lightNumber, (float*)&lightRangeConstant, 1);

							lightNumber++;
							if( lightNumber >= 16 ) {
								break;
							}
						}
					}

					particleEmitter->Render();

				} else {
					particleEmitter->Render();
				}
		
				if( fabs( particleEmitter->GetDepthHack() ) > 0.001 ) {
					camera->LeaveDepthHack();
				}
			}
			// revert to ordinary shader
			if( particleEmitter->mUseLighting ) {
				mVertexShader->Bind();
			}
		}
    }
}