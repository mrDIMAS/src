#include "ParticleSystemRenderer.h"
#include "DeferredRenderer.h"
#include "Light.h"
#include "Camera.h"

ParticleSystemRenderer * g_particleSystemRenderer = 0;

ParticleSystemRenderer::ParticleSystemRenderer() {
    string vertexSource =
        "float4x4 g_world;\n"
        "float4x4 g_view;\n"
        "float4x4 g_projection;\n"
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

        "VS_OUTPUT main(VS_INPUT input) {\n"
        "  VS_OUTPUT output;\n"
        "  float4x4 worldViewProj = mul(mul(g_world, g_view), g_projection);\n"
        "  output.position = mul(input.position, worldViewProj);\n"
        "  output.texcoord = input.texcoord;\n"
        "  output.color = input.color;\n"
        "  output.worldPos = mul(input.position, g_world );\n"
        "  output.viewPos = output.position;\n"
        "  return output;\n"
        "};\n";

    vertexShader = new VertexShader( vertexSource );

    vView = vertexShader->GetConstantTable()->GetConstantByName( 0, "g_view" );
    vProj = vertexShader->GetConstantTable()->GetConstantByName( 0, "g_projection" );
    vWorld = vertexShader->GetConstantTable()->GetConstantByName( 0, "g_world" );

    string pixelSource =
        "texture g_diffuseTexture ;\n"

        "sampler texsampler : register( s0 ) = sampler_state {\n"
        "  Texture = <g_diffuseTexture>;\n"
        "};\n"

        "texture g_depthTexture ;\n"

        "sampler depthSampler : register( s1 ) = sampler_state {\n"
        "  Texture = <g_depthTexture>;\n"
        "};\n"

        "struct PS_INPUT {\n"
        "  float2 texcoord : TEXCOORD0;\n"
        "  float4 color : TEXCOORD1;\n"
        "  float3 worldPos : TEXCOORD2;\n"
        "  float4 viewPos : TEXCOORD3;\n"
        "};\n"

        "#define MAX_LIGHTS 5\n"

        "float3 lightDiffuseColor[ MAX_LIGHTS ];\n"
        "float3 lightPosition[ MAX_LIGHTS ];\n"
        "float  lightRange[ MAX_LIGHTS ];\n"

        "int litNum = 0;\n"
        "int withLight = 0;\n"

        "float4x4 invViewProj;\n"

        "float thickness;"

        "float unpackFloatFromVec4i ( const float4 value )\n"
        "{\n"
        "  const float4 bitSh = float4 ( 1.0 / (256.0*256.0*256.0), 1.0 / (256.0*256.0), 1.0 / 256.0, 1.0 );\n"

        "  return dot ( value, bitSh );\n"
        "}\n"

        "float4 main(PS_INPUT input) : COLOR0\n"
        "{\n"

        "  input.viewPos /= input.viewPos.w;\n"
        "  float2 depthCoord = float2( input.viewPos.x  * 0.5 + 0.5, -input.viewPos.y * 0.5 + 0.5);\n"

        // diffuse color is product of texture diffuse color and color
        "  float4 diffuse = tex2D( texsampler, input.texcoord ) * input.color;\n"

        // get depth from depth map
        "  float depth = unpackFloatFromVec4i( tex2D( depthSampler, depthCoord ));\n"
        // restore position from depth
        "  float4 screenPosition;\n"
        "  screenPosition.x = depthCoord.x * 2.0f - 1.0f;\n"
        "  screenPosition.y = -(depthCoord.y * 2.0f - 1.0f);\n"
        "  screenPosition.z = depth;\n"
        "  screenPosition.w = 1.0f;\n"

        "  float4 p = mul( screenPosition, invViewProj );\n"
        "  p /= p.w;\n"

        "  float4 output = float4( 1, 1, 1, 1 );\n"

        // do simple lighting if it's needed
        "  if( withLight )\n"
        "  {\n"

        "    output = float4( 0, 0, 0, 0 );\n"

        "    for( int i = 0; i < litNum; i++ )\n"
        "    {\n"
        "      float3 lightDir = lightPosition[ i ] - input.worldPos;\n"
        "      output += float4( (lightDiffuseColor[ i ] * ( lightRange[ i ] / pow( dot(lightDir, lightDir ), 2 )) ).xyz, 0 );\n"
        "    }\n"

        "    output.w = 1.0;\n"

        "  }\n"

        "  output *= diffuse;"

        "  output.a *= saturate( thickness * distance( p.xyz, input.worldPos ) );\n"

        "  return output ;"
        "};\n";

    pixelShader = new PixelShader( pixelSource );

    pColor = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightDiffuseColor" );
    pRange = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightRange" );
    pPosition = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightPosition" );
    pLightCount = pixelShader->GetConstantTable()->GetConstantByName( 0, "litNum" );
    pWithLight = pixelShader->GetConstantTable()->GetConstantByName( 0, "withLight" );
    pInvViewProj = pixelShader->GetConstantTable()->GetConstantByName( 0, "invViewProj" );
    pThickness = pixelShader->GetConstantTable()->GetConstantByName( 0, "thickness" );

    D3DVERTEXELEMENT9 vdElem[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    g_device->CreateVertexDeclaration( vdElem, &vd );
}

ParticleSystemRenderer::~ParticleSystemRenderer() {
    delete vertexShader;
    delete pixelShader;

    vd->Release();
}

void ParticleSystemRenderer::RenderAllParticleSystems() {
    IDirect3DStateBlock9 * state;
    g_device->CreateStateBlock( D3DSBT_ALL, &state );

    vertexShader->Bind();
    pixelShader->Bind();
    g_device->SetVertexDeclaration( vd );

    g_device->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE,  D3DMCS_COLOR1 );

    g_device->SetRenderState ( D3DRS_ALPHATESTENABLE, FALSE );

    g_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

    g_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
    g_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_device->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);

    g_device->SetRenderState(D3DRS_ZENABLE, TRUE );
    g_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE );

    g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    vertexShader->GetConstantTable()->SetMatrix( g_device, vView, &g_camera->view );
    vertexShader->GetConstantTable()->SetMatrix( g_device, vProj, &g_camera->projection );

    g_deferredRenderer->GetGBuffer()->BindDepthMap( 1 );

    D3DXMATRIX viewProj;
    D3DXMatrixMultiply( &viewProj, &g_camera->view, &g_camera->projection );

    D3DXMATRIX invViewProj;
    D3DXMatrixInverse( &invViewProj, 0, &viewProj );

    pixelShader->GetConstantTable()->SetMatrix( g_device, pInvViewProj, &invViewProj );

    for( auto it = g_particleEmitters.begin(); it != g_particleEmitters.end(); ++it ) {
        ParticleEmitter * ps = *it;

        if( !ps->GetBase()->IsVisible() ) {
            continue;
        }

        if( !ps->IsEnabled() ) {
            continue;
        }

        if( !ps->HasAliveParticles() ) {
            continue;
        }

        ps->Update();

        ps->Bind();

        vertexShader->GetConstantTable()->SetMatrix( g_device, vWorld, &ps->GetWorldTransform() );
        pixelShader->GetConstantTable()->SetFloat( g_device, pThickness, ps->GetThickness() );

        if( ps->IsLightAffects() ) {
            vertexShader->GetConstantTable()->SetInt( g_device, pWithLight, 1 );

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

                vertexShader->GetConstantTable()->SetInt( g_device, pLightCount, cnt );

                for( size_t j = 0; j < (size_t)cnt; j++ ) {
                    Light * lit = affectedLights.at( j );

                    D3DXHANDLE nPos = pixelShader->GetConstantTable()->GetConstantElement( pPosition, j );
                    pixelShader->GetConstantTable()->SetFloatArray( g_device, nPos, lit->globalTransform.getOrigin().m_floats, 3 );
                    pixelShader->GetConstantTable()->SetFloatArray( g_device, pixelShader->GetConstantTable()->GetConstantElement( pColor, j ), lit->GetColor().elements, 3 );
                    pixelShader->GetConstantTable()->SetFloat( g_device, pixelShader->GetConstantTable()->GetConstantElement( pRange, j ), lit->GetRadius() * lit->GetRadius() );
                }

                ps->Render();

                for( size_t j = 0; j < (size_t)cnt; j++ ) {
                    affectedLights.erase( affectedLights.begin() );
                }
            }
        } else {
            vertexShader->GetConstantTable()->SetInt( g_device, pWithLight, 0 );

            ps->Render();
        }

    }

    state->Apply();
    state->Release();
}