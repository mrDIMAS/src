#include "Precompiled.h"

#include "ParticleSystemRenderer.h"
#include "DeferredRenderer.h"
#include "Camera.h"
#include "Engine.h"

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

    Engine::Instance().GetDevice()->CreateVertexDeclaration( vdElem, &vd );
}

ParticleSystemRenderer::~ParticleSystemRenderer() {
    delete vertexShader;
    delete pixelShader;
	delete vertexShaderLighting;

    vd->Release();
}

void ParticleSystemRenderer::RenderAllParticleSystems() {
	static vector<Light*> lightList;

    pixelShader->Bind();

    Engine::Instance().GetDevice()->SetVertexDeclaration( vd );
    vertexShader->Bind();
    for( auto particleEmitter : ParticleEmitter::msParticleEmitters ) {
        if( !particleEmitter->GetOwner()->IsVisible() || !particleEmitter->IsEnabled() || !particleEmitter->HasAliveParticles() ) {
            continue;
        }

		particleEmitter->Update();
		particleEmitter->Bind();

		if( fabs( particleEmitter->props.depthHack ) > 0.001 ) {
			Camera::msCurrentCamera->EnterDepthHack( fabs( particleEmitter->props.depthHack ));
		}

		if( particleEmitter->props.useLighting ) {
			vertexShaderLighting->Bind();
		}

		D3DXMATRIX mWVP;
		D3DXMatrixMultiply( &mWVP, &particleEmitter->mWorldTransform, &Camera::msCurrentCamera->mViewProjection );
		Engine::Instance().SetVertexShaderMatrix( 0, &mWVP );
		Engine::Instance().SetVertexShaderMatrix( 5, &particleEmitter->mWorldTransform );

		if( particleEmitter->props.useLighting ) {		
			Engine::Instance().SetVertexShaderFloat3( 61, Engine::Instance().GetAmbientColor().elements );
			lightList.clear();
			for( auto pLight : Light::msPointLightList ) {
				if( (pLight->GetRealPosition() - particleEmitter->GetOwner()->GetPosition()).Length() < ( 1.25f * pLight->GetRadius() + particleEmitter->props.boundingRadius )) {
					lightList.push_back( pLight );
				}
			}
			Engine::Instance().SetVertexShaderFloat( 62, lightList.size() );
			int lightNumber = 0;
			for( auto pLight : lightList ) {
				Engine::Instance().SetVertexShaderFloat3( 10 + lightNumber, pLight->GetRealPosition().elements );
				Engine::Instance().SetVertexShaderFloat3( 27 + lightNumber, pLight->GetColor().elements );
				Engine::Instance().SetVertexShaderFloat( 44 + lightNumber, pLight->GetRadius() );
				lightNumber++;
				if( lightNumber >= 16 ) {
					break;
				}
			}

			particleEmitter->Render();

		} else {
			particleEmitter->Render();
		}
		
		if( fabs( particleEmitter->props.depthHack ) > 0.001 ) {
			Camera::msCurrentCamera->LeaveDepthHack();
		}

		// revert to ordinary shader
		if( particleEmitter->props.useLighting ) {
			vertexShader->Bind();
		}
    }
}