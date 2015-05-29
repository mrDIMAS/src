#include "Precompiled.h"

#include "ParticleSystemRenderer.h"
#include "DeferredRenderer.h"
#include "Camera.h"
#include "Engine.h"

ParticleSystemRenderer::ParticleSystemRenderer() {
    vertexShader = new VertexShader( "data/shaders/particle.vso" );
    pixelShader = new PixelShader( "data/shaders/particle.pso" );

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

    vd->Release();
}

void ParticleSystemRenderer::RenderAllParticleSystems() {
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

        D3DXMATRIX mWVP;
        D3DXMatrixMultiply( &mWVP, &particleEmitter->mWorldTransform, &Camera::msCurrentCamera->mViewProjection );
        Engine::Instance().SetVertexShaderMatrix( 0, &mWVP );

        particleEmitter->Render();

		if( fabs( particleEmitter->props.depthHack ) > 0.001 ) {
			Camera::msCurrentCamera->LeaveDepthHack();
		}
    }
}