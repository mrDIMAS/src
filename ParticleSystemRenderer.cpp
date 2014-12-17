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

#define PS_LIGHT_PER_PASS_COUNT 32

    string pixelSource =
        "sampler diffuseSampler : register( s0 );\n"

        "struct PS_INPUT {\n"
        "  float2 texcoord : TEXCOORD0;\n"
        "  float4 color : TEXCOORD1;\n"
        "  float3 worldPos : TEXCOORD2;\n"
        "  float4 viewPos : TEXCOORD3;\n"
        "};\n"
        // lighting
        "float3 lightDiffuseColor[ 32 ];\n"
        "float3 lightPosition[ 32 ];\n"
        "float lightRange[ 32 ];\n"
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
        "  return output ;"
        "};\n";

    pixelShader = new PixelShader( pixelSource );

    pColor = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightDiffuseColor" );
    pRange = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightRange" );
    pPosition = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightPosition" );
    pLightCount = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightCount" );
    pWithLight = pixelShader->GetConstantTable()->GetConstantByName( 0, "withLight" );

    D3DVERTEXELEMENT9 vdElem[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    CheckDXErrorFatal( gpDevice->CreateVertexDeclaration( vdElem, &vd ));
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
        if( !particleEmitter->GetBase()->IsVisible() || !particleEmitter->IsEnabled() || !particleEmitter->HasAliveParticles() ) {
            continue;
        }

        particleEmitter->Update();
        particleEmitter->Bind();

        D3DXMATRIX mWVP;
        D3DXMatrixMultiply( &mWVP, &particleEmitter->world, &g_camera->viewProjection );
        CheckDXErrorFatal( vertexShader->GetConstantTable()->SetMatrix( gpDevice, vWVP, &mWVP ));
        CheckDXErrorFatal( vertexShader->GetConstantTable()->SetMatrix( gpDevice, vWorld, &particleEmitter->world ));

        if( particleEmitter->IsLightAffects() ) {
            CheckDXErrorFatal( vertexShader->GetConstantTable()->SetInt( gpDevice, pWithLight, 1 ));
            // grab all light which have effect on this particle system
            affectedLights.clear();
            for( size_t j = 0; j < g_pointLightList.size(); j++ ) {
                Light * light = g_pointLightList.at( j );
                if( !light->IsVisible() ) {
                    continue;
                }
                if( light->GetRadius() < 0.1 ) {
                    continue;
                }
                affectedLights.push_back( light );
            }
            for( size_t j = 0; j < g_spotLightList.size(); j++ ) {
                Light * light = g_spotLightList.at( j );
                if( !light->IsVisible() ) {
                    continue;
                }
                if( light->GetRadius() < 0.1 ) {
                    continue;
                }
                affectedLights.push_back( light );
            }
            // set pixel shader constants
            CheckDXErrorFatal( vertexShader->GetConstantTable()->SetInt( gpDevice, pLightCount, affectedLights.size() ));
            int constantNum = 0;
            for( auto lit : affectedLights ) {
                D3DXHANDLE nPos = pixelShader->GetConstantTable()->GetConstantElement( pPosition, constantNum );
                CheckDXErrorFatal( pixelShader->GetConstantTable()->SetFloatArray( gpDevice, nPos, lit->globalTransform.getOrigin().m_floats, 3 ));
                CheckDXErrorFatal( pixelShader->GetConstantTable()->SetFloatArray( gpDevice, pixelShader->GetConstantTable()->GetConstantElement( pColor, constantNum ), lit->GetColor().elements, 3 ));
                CheckDXErrorFatal( pixelShader->GetConstantTable()->SetFloat( gpDevice, pixelShader->GetConstantTable()->GetConstantElement( pRange, constantNum ), lit->GetRadius() * lit->GetRadius() * lit->GetRadius() ));
                constantNum++;
            }
        } else {
            CheckDXErrorFatal( vertexShader->GetConstantTable()->SetInt( gpDevice, pWithLight, 0 ));
        }

        particleEmitter->Render();
    }
}