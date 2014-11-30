#include "ParticleSystemRenderer.h"
#include "DeferredRenderer.h"
#include "Light.h"
#include "Camera.h"

ParticleSystemRenderer * g_particleSystemRenderer = 0;

ParticleSystemRenderer::ParticleSystemRenderer() {
    string vertexSource =
        "float4x4 g_world;\n"
        "float4x4 g_WVP;\n"

        "struct VS_INPUT {\n"
        "  float4 position : POSITION;\n"
        "  float2 texcoord : TEXCOORD0;\n"
        "  float4 color : COLOR0;\n"
        "};\n"

        "struct VS_OUTPUT {\n"
        "  float4 position : POSITION;\n"
        "  float2 texcoord : TEXCOORD0;\n"
        "  float4 color : TEXCOORD1;\n"
        "  float3 worldPos : TEXCOORD2;\n"
        "  float4 viewPos : TEXCOORD3;\n"
        "};\n"

        "VS_OUTPUT main( VS_INPUT input ) {\n"
		"  VS_OUTPUT output = (VS_OUTPUT)0;\n"
        "  output.position = mul( input.position, g_WVP );\n"
		"  output.worldPos = mul( input.position, (float3x3)g_world );\n"
		"  output.texcoord = input.texcoord;\n"
		"  output.color = input.color;\n"
        "  output.viewPos = output.position;\n"
        "  return output;\n"
        "};\n";

    vertexShader = new VertexShader( vertexSource );

    vWVP = vertexShader->GetConstantTable()->GetConstantByName( 0, "g_WVP" );
    vWorld = vertexShader->GetConstantTable()->GetConstantByName( 0, "g_world" );

    string pixelSource =
        "sampler diffuseSampler : register( s0 );\n"
        "sampler depthSampler : register( s1 );\n"

        "struct PS_INPUT {\n"
        "  float2 texcoord : TEXCOORD0;\n"
        "  float4 color : TEXCOORD1;\n"
        "  float3 worldPos : TEXCOORD2;\n"
        "  float4 viewPos : TEXCOORD3;\n"
        "};\n"
		// lighting
        "float3 lightDiffuseColor[ 16 ];\n"
        "float3 lightPosition[ 16 ];\n"
        "float lightRange[ 16 ];\n"
        "int lightCount = 0;\n"
        "int withLight = 0;\n"       
		// soft particles props
		"float4x4 invViewProj;\n"
        "float thickness;"

		// entry point
		"float4 main(PS_INPUT input) : COLOR0 {\n"
        "  float4 output = float4( 1.0f, 1.0f, 1.0f, 1.0f );\n"
        // do simple diffuse lighting
		"  if( withLight ) {\n"
        "    output.xyz = float3( 0.0f, 0.0f, 0.0f );\n"
		"    for( int i = 0; i < lightCount; i++ ) {\n"
        "      float3 lightDir = lightPosition[i] - input.worldPos;\n"
        "      output += float4( ( lightDiffuseColor[i] * ( lightRange[i] / pow( dot(lightDir, lightDir ), 2 )) ).xyz, 0.0f );\n"
        "    }\n"
        "  }\n"
		// apply diffuse color
        "  output *= tex2D( diffuseSampler, input.texcoord ) * input.color;"
		// soft particles
		// restore position from depth
		"  input.viewPos /= input.viewPos.w;\n"
		"  float2 depthCoord = float2( input.viewPos.x * 0.5 + 0.5, -input.viewPos.y * 0.5 + 0.5);"
		"  float4 screenPosition;\n"
		"  screenPosition.x =  ( depthCoord.x * 2.0f - 1.0f );\n"
		"  screenPosition.y = -( depthCoord.y * 2.0f - 1.0f );\n"
		"  screenPosition.z = tex2D( depthSampler, depthCoord ).r;\n"
		"  screenPosition.w = 1.0f;\n"
		"  float4 p = mul( screenPosition, invViewProj );\n"
		"  p /= p.w;\n"
		// adjust alpha of the fragment accorrding to restored position
        "  output.a *= saturate( thickness * distance( p.xyz, input.worldPos ) );\n"
        "  return output ;"
		//"	return float4( 1,1,1,1);\n"
        "};\n";

    pixelShader = new PixelShader( pixelSource );

    pColor = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightDiffuseColor" );
    pRange = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightRange" );
    pPosition = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightPosition" );
    pLightCount = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightCount" );
    pWithLight = pixelShader->GetConstantTable()->GetConstantByName( 0, "withLight" );
    pInvViewProj = pixelShader->GetConstantTable()->GetConstantByName( 0, "invViewProj" );
    pThickness = pixelShader->GetConstantTable()->GetConstantByName( 0, "thickness" );

    D3DVERTEXELEMENT9 vdElem[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    CheckDXErrorFatal( g_device->CreateVertexDeclaration( vdElem, &vd ));
}

ParticleSystemRenderer::~ParticleSystemRenderer() {
    delete vertexShader;
    delete pixelShader;

    vd->Release();
}

void ParticleSystemRenderer::RenderAllParticleSystems() {
    IDirect3DStateBlock9 * state;
    CheckDXErrorFatal( g_device->CreateStateBlock( D3DSBT_ALL, &state ));
	
	CheckDXErrorFatal( g_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ));
	CheckDXErrorFatal( g_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ));
	CheckDXErrorFatal( g_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE ));
	
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE,  D3DMCS_COLOR1 ));
    CheckDXErrorFatal( g_device->SetRenderState ( D3DRS_ALPHATESTENABLE, FALSE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ));
	
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD ));

    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZENABLE, TRUE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));
	
    g_deferredRenderer->GetGBuffer()->BindDepthMap( 1 );

	pixelShader->Bind();
    CheckDXErrorFatal( pixelShader->GetConstantTable()->SetMatrix( g_device, pInvViewProj, &g_camera->invViewProjection ));

	g_device->SetVertexDeclaration( vd );
	vertexShader->Bind();
    for( auto particleEmitter : g_particleEmitters ) {
        if( !particleEmitter->GetBase()->IsVisible() || !particleEmitter->IsEnabled() || !particleEmitter->HasAliveParticles() ) {
            continue;
        }

        particleEmitter->Update();
        particleEmitter->Bind();

		D3DXMATRIX mWVP;
		D3DXMatrixMultiply( &mWVP, &particleEmitter->world, &g_camera->viewProjection );		
		CheckDXErrorFatal( vertexShader->GetConstantTable()->SetMatrix( g_device, vWVP, &mWVP ));
		CheckDXErrorFatal( vertexShader->GetConstantTable()->SetMatrix( g_device, vWorld, &particleEmitter->world ));

        CheckDXErrorFatal( pixelShader->GetConstantTable()->SetFloat( g_device, pThickness, particleEmitter->GetThickness() ));

        if( particleEmitter->IsLightAffects() ) {
            CheckDXErrorFatal( vertexShader->GetConstantTable()->SetInt( g_device, pWithLight, 1 ));

            int lightPerPass = 5;
            int passCount = 1;//ceil( (float)g_lights.size() / (float)lightPerPass );

            int cnt = 0;

            vector< Light*> affectedLights;

            // draw mesh with each light
            for( size_t j = 0; j < g_pointLights.size(); j++ ) {
                Light * light = g_pointLights.at( j );

                if( !light->IsVisible() ) {
                    continue;
                }

                if( light->GetRadius() < 0.1 ) {
                    continue;
                }

                affectedLights.push_back( light );
            }

            for( int passNum = 0; passNum < passCount; passNum++ ) {
                cnt = affectedLights.size() > (size_t)lightPerPass ? lightPerPass : affectedLights.size();

                CheckDXErrorFatal( vertexShader->GetConstantTable()->SetInt( g_device, pLightCount, cnt ));

                for( size_t j = 0; j < (size_t)cnt; j++ ) {
                    Light * lit = affectedLights.at( j );

                    D3DXHANDLE nPos = pixelShader->GetConstantTable()->GetConstantElement( pPosition, j );
                    CheckDXErrorFatal( pixelShader->GetConstantTable()->SetFloatArray( g_device, nPos, lit->globalTransform.getOrigin().m_floats, 3 ));
                    CheckDXErrorFatal( pixelShader->GetConstantTable()->SetFloatArray( g_device, pixelShader->GetConstantTable()->GetConstantElement( pColor, j ), lit->GetColor().elements, 3 ));
                    CheckDXErrorFatal( pixelShader->GetConstantTable()->SetFloat( g_device, pixelShader->GetConstantTable()->GetConstantElement( pRange, j ), lit->GetRadius() * lit->GetRadius() ));
                }

                particleEmitter->Render();

                for( size_t j = 0; j < (size_t)cnt; j++ ) {
                    affectedLights.erase( affectedLights.begin() );
                }
            }
        } else {
            CheckDXErrorFatal( vertexShader->GetConstantTable()->SetInt( g_device, pWithLight, 0 ));
			
            particleEmitter->Render();
        }

    }

    CheckDXErrorFatal( state->Apply());
    state->Release();
}