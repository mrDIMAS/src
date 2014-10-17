#include "SSAO.h"

void SSAO::DoBlurAndAddAOToSourceMap( IDirect3DTexture9 * sourceMap ) {
    g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
    g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
    g_device->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_NOTEQUAL);
    g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
    g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ZENABLE, FALSE );

    g_device->SetTexture( 0, sourceMap );
    g_device->SetTexture( 1, aoTexture );

    g_device->SetRenderTarget( 0, blurRenderTarget );
    g_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 122, 0, 0 ), 1.0, 0 );

    screenQuad->Bind();

    blurPixelShader->Bind();

    blurPixelShader->GetConstantTable()->SetFloat( g_device, blurScreenWidth, g_width );
    blurPixelShader->GetConstantTable()->SetFloat( g_device, blurScreenHeight, g_height );

    screenQuad->Render();
}

void SSAO::FillAOMap() {
    g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
    g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
    g_device->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_NOTEQUAL);
    g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
    g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ZENABLE, FALSE );

    screenQuad->Bind();

    gBuffer->BindDepthMap( 0 );
    g_device->SetTexture( 1, rotateTexture );

    g_device->SetRenderTarget( 0, aoRenderTarget );
    g_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 122, 0 ), 1.0, 0 );

    aoPixelShader->Bind();

    aoPixelShader->GetConstantTable()->SetMatrix( g_device, aoInvViewProj, &g_camera->invViewProjection );

    aoPixelShader->GetConstantTable()->SetFloat( g_device, aoScreenWidth, g_width );
    aoPixelShader->GetConstantTable()->SetFloat( g_device, aoScreenHeight, g_height );

    aoPixelShader->GetConstantTable()->SetFloat( g_device, aoRotateWidth, 4 );
    aoPixelShader->GetConstantTable()->SetFloat( g_device, aoRotateHeight, 4 );

    static float radius = 0.12f;
    static float distScale = 0.55f;
    static float bias = 0.45f;

    aoPixelShader->GetConstantTable()->SetFloat( g_device, aoRadius, radius );
    aoPixelShader->GetConstantTable()->SetFloat( g_device, aoDistScale, distScale );
    aoPixelShader->GetConstantTable()->SetFloat( g_device, aoBias, bias );

    if( mi::KeyDown( mi::T ))
        radius += 0.02f;
    if( mi::KeyDown( mi::G ))
        if( radius > 0.02f )
            radius -= 0.02f;

    if( mi::KeyDown( mi::Y ))
        distScale += 0.02f;
    if( mi::KeyDown( mi::H ))
        if( distScale > 0.02f )
            distScale -= 0.02f;

    if( mi::KeyDown( mi::R ))
        bias += 0.02f;
    if( mi::KeyDown( mi::F ))
        if( bias > 0.02f )
            bias -= 0.02f;

    screenQuad->Render();
}

SSAO::SSAO( GBuffer * theGBuffer ) {
    gBuffer = theGBuffer;

    screenQuad = new EffectsQuad;

    // Create render targets
    D3DXCreateTexture( g_device, g_width, g_height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &aoTexture );

    aoTexture->GetSurfaceLevel( 0, &aoRenderTarget );

    D3DXCreateTexture( g_device, g_width, g_height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &blurTexture );
    blurTexture->GetSurfaceLevel( 0, &blurRenderTarget );

    // Create rotate map

    int rotateTexSize = 4;

    D3DXCreateTexture( g_device, rotateTexSize, rotateTexSize, 0, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &rotateTexture );

    RECT toLock = { 0, 0, 4, 4 };
    D3DLOCKED_RECT locked;

    rotateTexture->LockRect( 0, &locked, &toLock, 0 );

    for( int i = 0; i < rotateTexSize; i++ ) {
        for( int j = 0; j < rotateTexSize; j++ ) {
            unsigned char * data = reinterpret_cast< unsigned char * >( locked.pBits );

            data[ i * locked.Pitch + ( j + 0 ) ] = 0;
            data[ i * locked.Pitch + ( j + 1 ) ] = ( unsigned char)( frandom( -1.0f, 1.0f ) * 255.0f );
            data[ i * locked.Pitch + ( j + 2 ) ] = ( unsigned char)( frandom( -1.0f, 1.0f ) * 255.0f );
            data[ i * locked.Pitch + ( j + 3 ) ] = ( unsigned char)( frandom( -1.0f, 1.0f ) * 255.0f );
        };
    };

    rotateTexture->UnlockRect( 0 );

    // D3DXCreateTextureFromFileA( g_device, "data/textures/ssao.png", &rotateTexture );

    string aoSource =
        "texture depthTexture;\n"

        "sampler depthSampler : register(s0) = sampler_state\n"
        "{\n"
        "   texture = <depthTexture>;\n"
        "};\n"

        "texture rotateTexture;\n"

        "sampler rotateSampler : register(s1) = sampler_state\n"
        "{\n"
        "   texture = <rotateTexture>;\n"
        "};\n"

        "float4x4 invViewProj;\n"

        "float screenWidth;\n"
        "float screenHeight;\n"

        "float rotateMapWidth;\n"
        "float rotateMapHeight;\n"

        "float radius;\n"
        "float distScale;\n"
        "float bias;\n"

        "float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0\n "
        "{\n"
        "   const float zNear = 0.1f;"
        "   const float zFar = 6000.0f;"
        /*
        "   float4	rndTable[8];\n"

        "   rndTable[0] = float4 ( -0.5, -0.5, -0.5, 0.0 );\n"
        "   rndTable[1] = float4 (  0.5, -0.5, -0.5, 0.0 );\n"
        "   rndTable[2] = float4 ( -0.5,  0.5, -0.5, 0.0 );\n"
        "   rndTable[3] = float4 (  0.5,  0.5, -0.5, 0.0 );\n"
        "   rndTable[4] = float4 ( -0.5, -0.5,  0.5, 0.0 );\n"
        "   rndTable[5] = float4 (  0.5, -0.5,  0.5, 0.0 );\n"
        "   rndTable[6] = float4 ( -0.5,  0.5,  0.5, 0.0 );\n"
        "   rndTable[7] = float4 (  0.5,  0.5,  0.5, 0.0 );\n"
        */
        "   float3	rndTable[12];\n"

        "   rndTable[0] = float3 ( -0.326212, -0.405805, 0.0f );\n"
        "   rndTable[1] = float3 ( -0.840144, -0.073580, 0.0f );\n"
        "   rndTable[2] = float3 ( -0.695914,  0.457137, 0.0f );\n"
        "   rndTable[3] = float3 ( -0.203345,  0.620716, 0.0f );\n"
        "   rndTable[4] = float3 (  0.962340, -0.194983, 0.0f );\n"
        "   rndTable[5] = float3 (  0.473434, -0.480026, 0.0f );\n"
        "   rndTable[6] = float3 (  0.519456,  0.767022, 0.0f );\n"
        "   rndTable[7] = float3 (  0.185461, -0.893124, 0.0f );\n"
        "   rndTable[8] = float3 (  0.507431,  0.064425, 0.0f );\n"
        "   rndTable[9] = float3 (  0.896420,  0.412458, 0.0f );\n"
        "   rndTable[10] = float3 ( -0.321940, -0.932615, 0.0f );\n"
        "   rndTable[11] = float3 ( -0.791559, -0.597705, 0.0f );\n"

        "   float depth = tex2D( depthSampler, texCoord ).r;\n"
        "   float z = zFar*zNear/(depth * (zFar - zNear) - zFar);"

        "   float2 rotateMapCoord = float2( texCoord.x * screenWidth / rotateMapWidth, texCoord.y * screenHeight / rotateMapHeight );\n"
        "   float3 plane = tex2D( rotateSampler, rotateMapCoord ).xyz;\n"

        "   float attenuation = 0.0;\n"

        "   for( int i = 0; i < 12; i++ )\n"
        "   {\n"
        "     float3 sample = reflect( rndTable[ i ].xyz, plane );\n"

        "     float2 zSampleTexCoord = texCoord + ( radius * sample.xy ) / ( z * 0.65 );\n"
        "     float	zSample = tex2D( depthSampler, zSampleTexCoord ).r;\n"

        "     zSample = zFar*zNear/( zSample * (zFar - zNear) - zFar);"

        "     float	dist = max ( zSample - z , 0.0 ) / distScale;\n"
        "     float	occl = 50 * max ( dist * ( 500 - dist), 0.0 );\n"

        "     attenuation += 1.0 / ( 1.0 + occl*occl );\n"
        "   }\n"

        "   attenuation = clamp ( attenuation / 12.0 + bias, 0.0, 1.0 );\n"

        "   return float4( attenuation, attenuation, attenuation, 1.0f );\n"
        "};\n";

    aoPixelShader = new PixelShader( aoSource );

    aoInvViewProj = aoPixelShader->GetConstantTable()->GetConstantByName( 0, "invViewProj" );

    aoScreenWidth = aoPixelShader->GetConstantTable()->GetConstantByName( 0, "screenWidth" );
    aoScreenHeight = aoPixelShader->GetConstantTable()->GetConstantByName( 0, "screenHeight" );

    aoRotateWidth = aoPixelShader->GetConstantTable()->GetConstantByName( 0, "rotateMapWidth" );
    aoRotateHeight = aoPixelShader->GetConstantTable()->GetConstantByName( 0, "rotateMapHeight" );

    aoRadius = aoPixelShader->GetConstantTable()->GetConstantByName( 0, "radius" );
    aoDistScale = aoPixelShader->GetConstantTable()->GetConstantByName( 0, "distScale" );
    aoBias = aoPixelShader->GetConstantTable()->GetConstantByName( 0, "bias" );

    string blurSource =
        "texture sourceMap;\n"

        "sampler sourceSampler : register(s0) = sampler_state\n"
        "{\n"
        "   texture = <sourceMap>;\n"
        "};\n"

        "texture aoMap;\n"

        "sampler aoSampler : register(s1) = sampler_state\n"
        "{\n"
        "   texture = <aoMap>;\n"
        "};\n"

        "float screenWidth;\n"
        "float screenHeight;\n"

        "float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0\n"
        "{\n"
        "   float	h1 = 0.5 / screenWidth;\n"
        "   float	h2 = 0.5 / screenHeight;\n"
        "   float4 ao = float4( 0.0, 0.0, 0.0, 0.0 );\n"

        "   for ( int i = -1; i <= 1; i++ )\n"
        "     for ( int j = -1; j <= 1; j++ )\n"
        "       ao += tex2D ( aoSampler, texCoord + float2( (2*i+1)*h1, (2*j+1)*h2 ) );\n"

        //"   return tex2D ( sourceSampler, texCoord ) * pow ( ao / 9.0, 2.0 );\n"
        "   return float4 ( 1, 1, 1, 1 ) * pow ( ao / 9.0, 2.0 );\n"
        "};\n";

    blurPixelShader = new PixelShader( blurSource );

    blurScreenWidth = blurPixelShader->GetConstantTable()->GetConstantByName( 0, "screenWidth" );
    blurScreenHeight = blurPixelShader->GetConstantTable()->GetConstantByName( 0, "screenHeight" );
}
