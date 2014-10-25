#include "DeferredRenderer.h"
#include "Light.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Camera.h"
#include "Skybox.h"
#include "Utility.h"
#include "Renderer.h"

DeferredRenderer * g_deferredRenderer = 0;

bool g_fxaaEnabled = true;

DeferredRenderer::DeferredRenderer() {
    effectsQuad = new EffectsQuad;

    CreateBoundingVolumes();

    fxaa = new FXAA;
    gBuffer = new GBuffer;
    pass2SpotLight = new Pass2SpotLight;
    pass2AmbientLight = new Pass2AmbientLight;
    pass2PointLight = new Pass2PointLight;
    bvRenderer = new BoundingVolumeRenderingShader;
    shadowMap = new ShadowMap;
    //ssao = new SSAO( gBuffer );
}

GBuffer * DeferredRenderer::GetGBuffer() {
    return gBuffer;
}

struct XYZNormalVertex {
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
};

void DeferredRenderer::CreateBoundingVolumes() {
    int quality = 6;

    D3DXCreateSphere( g_device, 1.0, quality, quality, &icosphere, 0 );

    D3DXCreateCylinder( g_device, 0.0f, 1.0f, 1.0f, quality, quality, &cone, 0 );

    // rotate cylinder on 90 degrees
    XYZNormalVertex * data;
    cone->LockVertexBuffer( 0, (void**)&data );
    D3DXMATRIX tran;
    D3DXMatrixTranslation( &tran, 0, -0.5, 0 );
    D3DXMATRIX rot90;
    D3DXMatrixRotationAxis( &rot90, &D3DXVECTOR3( 1, 0, 0 ), SIMD_HALF_PI );
    D3DXMATRIX transform;
    D3DXMatrixMultiply( &transform, &rot90, &tran );

    for( int i = 0; i < cone->GetNumVertices(); i++ ) {
        XYZNormalVertex * v = &data[ i ];

        D3DXVec3TransformCoord( &v->p, &v->p, &transform );
    }

    cone->UnlockVertexBuffer();
}





////////////////////////////////////////////////////////////
// Point Light Subclass
////////////////////////////////////////////////////////////
DeferredRenderer::Pass2PointLight::Pass2PointLight() {
    string pixelSourcePassTwo =
        "sampler depthSampler : register(s0);\n"
        "sampler normalSampler : register(s1);\n"
        "sampler diffuseSampler : register(s2);\n"
        "sampler pointSampler : register(s3);\n"
#ifndef USE_R32F_DEPTH
        // ONLY for A8R8G8B8 depth
        "float unpackFloatFromVec4i ( const float4 value )\n"
        "{\n"
        "  const float4 bitSh = float4 ( 1.0 / (256.0*256.0*256.0), 1.0 / (256.0*256.0), 1.0 / 256.0, 1.0 );\n"

        "  return dot ( value, bitSh );\n"
        "}\n"
#endif
        // light props
        "float3 lightPos;\n"
        "float lightRange;\n"
        "float3 lightColor;\n"

        // camera props
        "float3 cameraPosition;\n"

        "int usePointTexture = false;\n"

        "float4x4 invViewProj;\n"
        "float spotAngle;\n"
        "float4 main( float2 texcoord : TEXCOORD0 ) : COLOR0\n"
        "{\n"
        // get diffuse color from diffuse map
        "   float4 diffuseTexel = tex2D( diffuseSampler, texcoord );\n"
#ifdef USE_R32F_DEPTH
        "   float depth = tex2D( depthSampler, texcoord ).r;\n"
#else
        "   float depth = unpackFloatFromVec4i( tex2D( depthSampler, texcoord ));\n"
#endif
        "   float3 n = tex2D( normalSampler, texcoord ).xyz;\n"

        // unpack normal from [0;1] to [-1,1]
        "   n.xyz = normalize(2 * n.xyz - 1.0f);\n"

        "   float4 screenPosition;\n"
        "   screenPosition.x = texcoord.x * 2.0f - 1.0f;\n"
        "   screenPosition.y = -(texcoord.y * 2.0f - 1.0f);\n"
        "   screenPosition.z = depth;\n"
        "   screenPosition.w = 1.0f;\n"

        "   float4 p = mul( screenPosition, invViewProj );\n"
        "   p /= p.w;\n"

        "   float3 lightDirection = lightPos - p;"
        "   float3 l = normalize( lightDirection );\n"

        // specular
        "   float3 v = normalize( cameraPosition - p );\n"
        "   float3 r = reflect( -v, n );\n"
        "   float spec = pow( saturate( dot( l, r ) ), 40.0 );\n"

        // point texture( cube )
        "   float4 pointTexel = float4( 1.0f, 1.0f, 1.0f, 1.0f );\n"
        "   if( usePointTexture ) {"
        "       pointTexel = texCUBE( pointSampler, l );\n"
        "   };\n"
        // diffuse
        "   float diff = saturate(dot( l, n ));\n"

        "   float falloff = lightRange / pow( dot( lightDirection, lightDirection ), 2 );\n"

        "   float o = clamp( falloff, 0.0, 1.2 ) * (  diff + spec  );\n"

        "   return pointTexel * float4( lightColor.x * diffuseTexel.x * o, lightColor.y * diffuseTexel.y * o, lightColor.z * diffuseTexel.z * o, 1.0f );\n"

        //" return float4( 1, 0, 0, 1 );\n"
        "};\n";


    pixelShader = new PixelShader( pixelSourcePassTwo );

    hLightPos = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightPos" );
    hLightRange = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightRange" );
    hCameraPos = pixelShader->GetConstantTable()->GetConstantByName( 0, "cameraPosition" );
    hInvViewProj = pixelShader->GetConstantTable()->GetConstantByName( 0, "invViewProj" );
    hLightColor = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightColor" );
    hUsePointTexture = pixelShader->GetConstantTable()->GetConstantByName( 0, "usePointTexture" );
}

void DeferredRenderer::Pass2PointLight::Bind( D3DXMATRIX & invViewProj ) {
    BindShader();

    pixelShader->GetConstantTable()->SetFloatArray( g_device, hCameraPos, g_camera->globalTransform.getOrigin().m_floats, 3 );
    pixelShader->GetConstantTable()->SetMatrix( g_device, hInvViewProj, &invViewProj );
}

void DeferredRenderer::Pass2PointLight::BindShader( ) {
    pixelShader->Bind();
}

void DeferredRenderer::Pass2PointLight::SetLight( Light * light ) {
    pixelShader->GetConstantTable()->SetFloatArray( g_device, hLightPos, light->globalTransform.getOrigin().m_floats, 3 );
    pixelShader->GetConstantTable()->SetFloat( g_device, hLightRange, powf( light->GetRadius(), 4 ) );
    pixelShader->GetConstantTable()->SetFloatArray( g_device, hLightColor, light->GetColor().elements, 3 );

    if( light->pointTexture ) {
        g_device->SetTexture( 3, light->pointTexture->cubeTexture );
        pixelShader->GetConstantTable()->SetInt( g_device, hUsePointTexture, 1 );
    } else {
        g_device->SetTexture( 3, 0 );
        pixelShader->GetConstantTable()->SetInt( g_device, hUsePointTexture, 0 );
    }
}

DeferredRenderer::Pass2PointLight::~Pass2PointLight() {
    delete pixelShader;
}

////////////////////////////////////////////////////////////
// Ambient Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::Pass2AmbientLight::Pass2AmbientLight() {
    string source =
        "sampler diffuseSampler : register(s2);\n"

        "float3 ambientColor;\n"

        "float4 main( float2 texcoord : TEXCOORD0 ) : COLOR0\n"
        "{\n"
        "   float4 diffuseTexel = tex2D( diffuseSampler, texcoord );\n"

        "   float albedo = diffuseTexel.a;\n"

        "   float4 intensity = clamp( float4( ambientColor.x + albedo, ambientColor.y + albedo, ambientColor.z + albedo, 1.0f ), 0.0f, 1.0f );\n"

        "   return intensity * diffuseTexel;\n"
        "};\n";

    pixelShader = new PixelShader( source );

    hAmbientColor = pixelShader->GetConstantTable()->GetConstantByName( 0, "ambientColor" );
}

void DeferredRenderer::Pass2AmbientLight::Bind( ) {
    pixelShader->Bind();

    pixelShader->GetConstantTable()->SetFloatArray( g_device, hAmbientColor, g_ambientColor.elements, 3 );
}

DeferredRenderer::Pass2AmbientLight::~Pass2AmbientLight() {
    delete pixelShader;
}

////////////////////////////////////////////////////////////
// Spot Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::Pass2SpotLight::Pass2SpotLight( ) {
    string spotSource =
        "sampler depthSampler : register(s0);\n"
        "sampler normalSampler : register(s1);\n"
        "sampler diffuseSampler : register(s2);\n"
        "sampler spotSampler : register(s3);\n"
        "sampler shadowSampler : register(s4);\n"
#ifndef USE_R32F_DEPTH
        "float unpackFloatFromVec4i ( const float4 value )\n"
        "{\n"
        "  const float4 bitSh = float4 ( 1.0 / (256.0*256.0*256.0), 1.0 / (256.0*256.0), 1.0 / 256.0, 1.0 );\n"

        "  return dot ( value, bitSh );\n"
        "}\n"
#endif

        // light props
        "float3 lightPos;\n"
        "float lightRange;\n"
        "float3 lightColor;\n"

        // camera props
        "float3 cameraPosition;\n"

        "float4x4 invViewProj;\n"
        "int useSpotTexture = false;\n"
        "int useShadows = false;\n"
        "float4x4 spotViewProjMatrix;\n"
        "float4x4 cameraView;\n"
        "float innerAngle;\n"
        "float outerAngle;\n"
        "float3 direction;\n"

        "float4 main( float2 texcoord : TEXCOORD0 ) : COLOR0\n"
        "{\n"
        // get diffuse color from diffuse map
        "   float4 diffuseTexel = tex2D( diffuseSampler, texcoord );\n"
#ifdef USE_R32F_DEPTH
        "   float depth = tex2D( depthSampler, texcoord ).r;\n"
#else
        "   float depth = unpackFloatFromVec4i( tex2D( depthSampler, texcoord ));\n"
#endif
        "   float3 n = tex2D( normalSampler, texcoord ).xyz;\n"

        // unpack normal from [0;1] to [-1,1]
        "   n.xyz = normalize(2 * n.xyz - 1.0f);\n"

        "   float4 screenPosition;\n"
        "   screenPosition.x =    texcoord.x * 2.0f - 1.0f;\n"
        "   screenPosition.y = -( texcoord.y * 2.0f - 1.0f );\n"
        "   screenPosition.z = depth;\n"
        "   screenPosition.w = 1.0f;\n"

        "   float4 p = mul( screenPosition, invViewProj );\n"
        "   p /= p.w;\n"

        "   float4 projPos = mul( float4( p.xyz, 1 ), spotViewProjMatrix );\n"
        "   projPos.xyz /= projPos.w;\n"
        "   float2 projTexCoords = projPos.xy * 0.5 + 0.5;\n"

        // spot texture
        "   float4 spotTextureTexel = float4( 1, 1, 1, 1 );\n"

        "   if( useSpotTexture )\n"
        "     spotTextureTexel = tex2D( spotSampler, projTexCoords );\n "


        // light calculations
        "   float3 lightDirection = lightPos - p;"
        "   float3 l = normalize( lightDirection );\n"

        // shadow
        "   float shadowMult = 1.0f;\n"
        "   float shadowDepth = tex2D( shadowSampler, projTexCoords ).r;\n"
        "   shadowDepth = 1 - ( 1 - shadowDepth ) * 25;\n"
        "   if( shadowDepth > depth ) {\n"
        "       shadowMult = 0.05f;\n"
        "   };\n"

        // specular
        "   float3 v = normalize( cameraPosition - p );\n"
        "   float3 r = reflect( -v, n );\n"
        "   float spec = pow( saturate( dot( l, r ) ), 40.0 );\n"

        // diffuse
        "   float diff = saturate(dot( l, n ));\n"
        "   float falloff = lightRange / pow( dot( lightDirection, lightDirection ), 2.3 );\n"

        // spot
        "   float spotAngleCos = dot( direction, l ) ;\n"
        "   float spot = smoothstep( outerAngle - 0.08, 1.0f , spotAngleCos );\n"
        "   float o = clamp( falloff * spot, 0.0, 2.0 ) * (  diff + spec  );\n"

        //"   return spotTextureTexel * float4( lightColor.x * diffuseTexel.x * o, lightColor.y * diffuseTexel.y * o, lightColor.z * diffuseTexel.z * o, 1.0f );\n"
        "   return float4( shadowDepth, shadowDepth, shadowDepth, 1.0f );\n"
        "};\n";


    pixelShader = new PixelShader( spotSource );

    hLightPos = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightPos" );
    hLightRange = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightRange" );
    hCameraPos = pixelShader->GetConstantTable()->GetConstantByName( 0, "cameraPosition" );
    hInvViewProj = pixelShader->GetConstantTable()->GetConstantByName( 0, "invViewProj" );
    hLightColor = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightColor" );
    hCameraView = pixelShader->GetConstantTable()->GetConstantByName( 0, "cameraView" );

    hInnerAngle = pixelShader->GetConstantTable()->GetConstantByName( 0, "innerAngle" );
    hOuterAngle = pixelShader->GetConstantTable()->GetConstantByName( 0, "outerAngle" );
    hDirection = pixelShader->GetConstantTable()->GetConstantByName( 0, "direction" );
    hUseSpotTexture = pixelShader->GetConstantTable()->GetConstantByName( 0, "useSpotTexture" );
    hSpotViewProjMatrix = pixelShader->GetConstantTable()->GetConstantByName( 0, "spotViewProjMatrix" );
    hUseShadows = pixelShader->GetConstantTable()->GetConstantByName( 0, "useShadows" );
}

void DeferredRenderer::Pass2SpotLight::BindShader( ) {
    pixelShader->Bind();
}

void DeferredRenderer::Pass2SpotLight::Bind( D3DXMATRIX & invViewProj ) {
    BindShader();

    pixelShader->GetConstantTable()->SetFloatArray( g_device, hCameraPos, g_camera->globalTransform.getOrigin().m_floats, 3 );
    pixelShader->GetConstantTable()->SetMatrix( g_device, hInvViewProj, &invViewProj );
}

void DeferredRenderer::Pass2SpotLight::SetLight( Light * lit ) {
    btVector3 direction = ( lit->globalTransform.getBasis() * btVector3( 0, 1, 0 )).normalize();
    pixelShader->GetConstantTable()->SetFloatArray( g_device, hLightPos, lit->globalTransform.getOrigin().m_floats, 3 );
    pixelShader->GetConstantTable()->SetFloat( g_device, hLightRange, powf( lit->GetRadius(), 4 ));
    pixelShader->GetConstantTable()->SetFloatArray( g_device, hLightColor, lit->GetColor().elements, 3 );
    pixelShader->GetConstantTable()->SetFloat( g_device, hInnerAngle, lit->GetCosHalfInnerAngle() );
    pixelShader->GetConstantTable()->SetFloat( g_device, hOuterAngle, lit->GetCosHalfOuterAngle() );
    pixelShader->GetConstantTable()->SetFloatArray( g_device, hDirection, direction.m_floats, 3 );
    pixelShader->GetConstantTable()->SetInt( g_device, hUseShadows, g_useShadows ? 1 : 0 );
    if( lit->spotTexture || g_useShadows ) {
        lit->BuildSpotProjectionMatrix();
        if( lit->spotTexture ) {
            lit->spotTexture->Bind( 3 );
            pixelShader->GetConstantTable()->SetInt( g_device, hUseSpotTexture, 1 );
        }
        pixelShader->GetConstantTable()->SetMatrix( g_device, hCameraView, &g_camera->viewProjection );
        pixelShader->GetConstantTable()->SetMatrix( g_device, hSpotViewProjMatrix, &lit->spotViewProjectionMatrix );        
    } else {
        g_device->SetTexture( 3, nullptr );
        pixelShader->GetConstantTable()->SetInt( g_device, hUseSpotTexture, 0 );
    }
}

DeferredRenderer::Pass2SpotLight::~Pass2SpotLight() {
    delete pixelShader;
}

DeferredRenderer::~DeferredRenderer() {
    if( gBuffer ) {
        delete gBuffer;
    }

    if( icosphere ) {
        icosphere->Release();
    }

    if( cone ) {
        cone->Release();
    }

    if( effectsQuad ) {
        delete effectsQuad;
    }

    if( pass2SpotLight ) {
        delete pass2SpotLight;
    }

    if( pass2AmbientLight ) {
        delete pass2AmbientLight;
    }

    if( pass2PointLight ) {
        delete pass2PointLight;
    }

    if( bvRenderer ) {
        delete bvRenderer;
    }

    if( shadowMap ) {
        delete shadowMap;
    }

    if( fxaa ) {
        delete fxaa;
    }
}

//////////////////////////////////////////////////////////////////////////
// Bounding volume rendering shader
//
// Bounding volume for a light can be a sphere for point light
// a oriented cone for a spot light
DeferredRenderer::BoundingVolumeRenderingShader::BoundingVolumeRenderingShader() {
    string vertexSourcePassOne =
        "float4x4 worldViewProj;\n"

        "float4 main( float4 position : POSITION ) : POSITION\n"
        "{\n"
        "  return mul(position, worldViewProj);\n"
        "};\n";

    vs = new VertexShader( vertexSourcePassOne );

    vWVP = vs->GetConstantTable()->GetConstantByName( 0, "worldViewProj" );

    string pixelSourcePassOne =
        "float4 main( ) : COLOR0\n"
        "{\n"
        "   return float4( 1.0f, 1.0f, 1.0f, 1.0f );\n"
        "};\n";

    ps = new PixelShader( pixelSourcePassOne );

    D3DVERTEXELEMENT9 vd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        D3DDECL_END()
    };

    g_device->CreateVertexDeclaration( vd, &vertexDeclaration ) ;
}

DeferredRenderer::BoundingVolumeRenderingShader::~BoundingVolumeRenderingShader() {
    delete ps;
    delete vs;
    vertexDeclaration->Release();
}

void DeferredRenderer::BoundingVolumeRenderingShader::Bind() {
    ps->Bind();
    vs->Bind();

    g_device->SetVertexDeclaration( vertexDeclaration );
}

void DeferredRenderer::BoundingVolumeRenderingShader::SetTransform( D3DXMATRIX & wvp ) {
    vs->GetConstantTable()->SetMatrix( g_device, vWVP, &wvp );
}

void DeferredRenderer::RenderIcosphereIntoStencilBuffer( float lightRadius, const btVector3 & lightPosition ) {
    float scl = 2.5f * lightRadius;

    D3DXMATRIX world;

    world._11 = scl;
    world._12 = 0.0f;
    world._13 = 0.0f;
    world._14 = 0.0f;
    world._21 = 0.0f;
    world._22 = scl;
    world._23 = 0.0f;
    world._24 = 0.0f;
    world._31 = 0.0f;
    world._32 = 0.0f;
    world._33 = scl;
    world._34 = 0.0f;
    world._41 = lightPosition.x();
    world._42 = lightPosition.y();
    world._43 = lightPosition.z();
    world._44 = 1.0f;

    bvRenderer->Bind();

    D3DXMATRIX wvp;
    D3DXMatrixMultiply( &wvp, &world, &g_camera->viewProjection );

    bvRenderer->SetTransform( wvp );

    // disable draw into color buffer
    g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );

    g_device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
    g_device->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

    g_device->SetRenderState( D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS );
    g_device->SetRenderState( D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP );

    // draw a sphere bounds light into stencil buffer
    icosphere->DrawSubset( 0 );
}

void DeferredRenderer::RenderConeIntoStencilBuffer( Light * lit ) {
    float height = lit->GetRadius() * 2.05;
    float radius = height * sinf( ( lit->GetOuterAngle() * 0.75f ) * SIMD_PI / 180.0f );
    D3DXMATRIX scale;
    D3DXMatrixScaling( &scale, radius, height, radius );
    D3DXMATRIX world;
    GetD3DMatrixFromBulletTransform( lit->globalTransform, world );
    D3DXMatrixMultiply( &world, &scale, &world );

    bvRenderer->Bind();

    D3DXMATRIX wvp;
    D3DXMatrixMultiply( &wvp, &world, &g_camera->viewProjection );

    bvRenderer->SetTransform( wvp );

    // disable draw into color buffer
    g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );

    g_device->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

    g_device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
    g_device->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR );

    g_device->SetRenderState( D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS );
    g_device->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_INCR );


    // draw a sphere bounds light into stencil buffer
    cone->DrawSubset( 0 );
}

void DeferredRenderer::RenderScreenQuad() {
    // enable draw into color buffer
    g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
    // draw a screen quad
    g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
    g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
    // quad render
    effectsQuad->Render();
}

void DeferredRenderer::ConfigureStencilBuffer() {
    g_device->SetRenderState( D3DRS_STENCILREF, 0x0 );
    g_device->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
    g_device->SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);

    // setup stencil buffer
    g_device->SetRenderState( D3DRS_STENCILENABLE, TRUE );
    g_device->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );
    g_device->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );

    g_device->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR );
    g_device->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_INCR );
}

void DeferredRenderer::EndFirstPassAndDoSecondPass() {
    OnEnd();

    if( g_fxaaEnabled ) {
        fxaa->BeginDrawIntoTexture();
    } else {
        gBuffer->BindBackSurfaceAsRT();
    }

    g_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    IDirect3DStateBlock9 * state;
    g_device->CreateStateBlock( D3DSBT_ALL, &state );

    gBuffer->BindTextures();

    g_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    g_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    g_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // first of all render the skybox
    if( g_camera->skybox ) {
        g_camera->skybox->Render( g_camera->globalTransform.getOrigin() );
    }

    // then render fullscreen quad with ambient lighting

    pass2AmbientLight->Bind();
    effectsQuad->Bind();
    effectsQuad->Render();

    ConfigureStencilBuffer();

    // Render point lights
    for( unsigned int i = 0; i < g_pointLights.size(); i++ ) {
        Light * light = g_pointLights.at( i );

        btVector3 lightPos = light->globalTransform.getOrigin();
        RenderIcosphereIntoStencilBuffer( light->GetRadius(), lightPos );

        pass2PointLight->Bind( g_camera->invViewProjection );
        pass2PointLight->SetLight( light );

        effectsQuad->Bind();

        RenderScreenQuad();
    }

    g_device->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
    g_device->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );

    g_device->SetSamplerState( 4, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
    g_device->SetSamplerState( 4, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );
    // Render spot lights
    for( unsigned int i = 0; i < g_spotLights.size(); i++ ) {
        Light * light = g_spotLights.at( i );
        
        if( g_useShadows ) {
            shadowMap->UnbindSpotShadowMap( 4 );
            shadowMap->RenderSpotShadowMap( fxaa ? fxaa->renderTarget : gBuffer->backSurface, 0, light );
            shadowMap->BindSpotShadowMap( 4 );
        }

        RenderConeIntoStencilBuffer( light );
        
        pass2SpotLight->Bind( g_camera->invViewProjection );
        pass2SpotLight->SetLight( light );

        effectsQuad->Bind();

        RenderScreenQuad();
    }

    if( g_fxaaEnabled ) {
        // ssao->FillAOMap();
        // ssao->DoBlurAndAddAOToSourceMap( fxaa->texture );
        // fxaa->DoAntialiasing( ssao->blurTexture );

        fxaa->DoAntialiasing( fxaa->texture );
    }

    state->Apply();
    state->Release();
}
