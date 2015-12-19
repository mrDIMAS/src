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
#include "DeferredRenderer.h"
#include "Camera.h"
#include "Engine.h"
#include "SceneFactory.h"

ParticleSystemRenderer::ParticleSystemRenderer() {
    vertexShader = new VertexShader( "data/shaders/particle.vso" );
    pixelShader = new PixelShader( "data/shaders/particle.pso" );
	vertexShaderLighting = new VertexShader( "data/shaders/particleLighting.vso" );

    D3DVERTEXELEMENT9 vdElem[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    Engine::I().GetDevice()->CreateVertexDeclaration( vdElem, &vd );
}

ParticleSystemRenderer::~ParticleSystemRenderer() {
    delete vertexShader;
    delete pixelShader;
	delete vertexShaderLighting;

    vd->Release();
}

void ParticleSystemRenderer::RenderAllParticleSystems() {
	static vector<weak_ptr<Light>> lightList;

    pixelShader->Bind();

    Engine::I().GetDevice()->SetVertexDeclaration( vd );
    vertexShader->Bind();

	auto & particleSystems = SceneFactory::GetParticleSystemList();
    for( auto pWeak : particleSystems ) {
		shared_ptr<ParticleSystem> & particleEmitter = pWeak.lock();
		if( particleEmitter ) {
			if( !particleEmitter->IsVisible() || !particleEmitter->IsEnabled() || !particleEmitter->HasAliveParticles() ) {
				continue;
			}

			particleEmitter->Update();
			particleEmitter->Bind();

			shared_ptr<Camera> camera = Camera::msCurrentCamera.lock();
			if( camera ) {
				if( fabs( particleEmitter->mDepthHack ) > 0.001 ) {
					camera->EnterDepthHack( fabs( particleEmitter->mDepthHack ));
				}

				if( particleEmitter->mUseLighting ) {
					vertexShaderLighting->Bind();
				}

				D3DXMATRIX mWVP;
				D3DXMatrixMultiply( &mWVP, &particleEmitter->mWorldTransform, &camera->mViewProjection );
				Engine::I().SetVertexShaderMatrix( 0, &mWVP );
				Engine::I().SetVertexShaderMatrix( 5, &particleEmitter->mWorldTransform );

				if( particleEmitter->mUseLighting ) {		
					Engine::I().SetVertexShaderFloat3( 61, Engine::I().GetAmbientColor().elements );
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

					Engine::I().SetVertexShaderFloat( 62, lightList.size() );
					int lightNumber = 0;
					for( auto & lWeak : lightList ) {
						shared_ptr<Light> & pLight = lWeak.lock();
						if( pLight ) {
							Engine::I().SetVertexShaderFloat3( 10 + lightNumber, pLight->GetPosition().elements );
							Engine::I().SetVertexShaderFloat3( 27 + lightNumber, pLight->GetColor().elements );
							Engine::I().SetVertexShaderFloat( 44 + lightNumber, pLight->GetRange() );
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
		
				if( fabs( particleEmitter->mDepthHack ) > 0.001 ) {
					camera->LeaveDepthHack();
				}
			}
			// revert to ordinary shader
			if( particleEmitter->mUseLighting ) {
				vertexShader->Bind();
			}
		}
    }
}