#include "Precompiled.h"

#include "ParticleSystemRenderer.h"
#include "DeferredRenderer.h"
#include "Camera.h"
#include "Renderer.h"

ParticleSystemRenderer * g_particleSystemRenderer = 0;

ParticleSystemRenderer::ParticleSystemRenderer() {
    vertexShader = new VertexShader( "data/shaders/particle.vso", true );
    pixelShader = new PixelShader( "data/shaders/particle.pso", true );

    D3DVERTEXELEMENT9 vdElem[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    gpDevice->CreateVertexDeclaration( vdElem, &vd );
}

ParticleSystemRenderer::~ParticleSystemRenderer() {
    delete vertexShader;
    delete pixelShader;

    vd->Release();
}

void ParticleSystemRenderer::RenderAllParticleSystems() {
    pixelShader->Bind();

    gpDevice->SetVertexDeclaration( vd );
    vertexShader->Bind();
    for( auto particleEmitter : g_particleEmitters ) {
        if( !particleEmitter->GetOwner()->IsVisible() || !particleEmitter->IsEnabled() || !particleEmitter->HasAliveParticles() ) {
            continue;
        }

        particleEmitter->Update();
        particleEmitter->Bind();

		if( fabs( particleEmitter->props.depthHack ) > 0.001 ) {
			g_camera->EnterDepthHack( fabs( particleEmitter->props.depthHack ));
		}

        D3DXMATRIX mWVP;
        D3DXMatrixMultiply( &mWVP, &particleEmitter->mWorldTransform, &g_camera->mViewProjection );
        gpRenderer->SetVertexShaderMatrix( 0, &mWVP );

        particleEmitter->Render();

		if( fabs( particleEmitter->props.depthHack ) > 0.001 ) {
			g_camera->LeaveDepthHack();
		}
    }
}