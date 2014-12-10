#include "PointlightShadowMap.h"

void PointlightShadowMap::RenderPointShadowMap( IDirect3DSurface9 * prevRT, int prevRTNum, Light * light )
{
    IDirect3DStateBlock9 * state;
    CheckDXErrorFatal( g_device->CreateStateBlock( D3DSBT_ALL, &state ));
    // set new suitable depth stencil surface
    CheckDXErrorFatal( g_device->SetDepthStencilSurface( depthStencil ));
    // set render states
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZENABLE, TRUE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ));

    // bind depth shaders
    pixelShader->Bind();
    vertexShader->Bind();
    // collect meshes that are in point light bounding sphere
    for( auto meshGroupIter : Mesh::meshes )   // brute force method
    {
        auto & group = meshGroupIter.second;
        for( auto mesh : group )
        {
            //if( mesh->ownerNode->IsVisible()) {
            //    if( (( mesh->aabb.center + mesh->ownerNode->GetPosition()) - light->GetRealPosition()).Length() < light->radius ) {
            lightedMeshes.push_back( mesh );
            //    }
            // }
        };
    }
    // render each face of the cube map
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovRH( &proj, SIMD_HALF_PI, 1.0f, 0.1f, 1024.0f );
    for( int i = 0; i < 6; i++ )
    {
        D3DXMATRIX view, viewProj;
        D3DXVECTOR3 lookAt, up;

        if( i == D3DCUBEMAP_FACE_POSITIVE_X )
        {
            lookAt = D3DXVECTOR3( -1, 0, 0 );
            up = D3DXVECTOR3( 0, -1, 0 );
        }
        if( i == D3DCUBEMAP_FACE_NEGATIVE_X )
        {
            lookAt = D3DXVECTOR3( 1, 0, 0 );
            up = D3DXVECTOR3( 0, -1, 0 );
        }
        if( i == D3DCUBEMAP_FACE_POSITIVE_Y )
        {
            lookAt = D3DXVECTOR3( 0, -1, 0 );
            up = D3DXVECTOR3( 0, 0, -1 );
        }
        if( i == D3DCUBEMAP_FACE_NEGATIVE_Y )
        {
            lookAt = D3DXVECTOR3( 0, 1, 0 );
            up = D3DXVECTOR3( 0, 0, 1 );
        }
        if( i == D3DCUBEMAP_FACE_POSITIVE_Z )
        {
            lookAt = D3DXVECTOR3( 0, 0, -1 );
            up = D3DXVECTOR3( 0, -1, 0 );
        }
        if( i == D3DCUBEMAP_FACE_NEGATIVE_Z )
        {
            lookAt = D3DXVECTOR3( 0, 0, 1 );
            up = D3DXVECTOR3( 0, -1, 0 );
        }
        lookAt += D3DXVECTOR3( light->globalTransform.getOrigin().m_floats );
        D3DXMatrixLookAtRH( &view, &D3DXVECTOR3( light->GetRealPosition().elements ), &lookAt, &up );
        D3DXMatrixMultiply( &viewProj, &view, &proj );
        // bind i-face of cube map
        CheckDXErrorFatal( g_device->SetRenderTarget( 0, cubeFaces[i] ));
        // clear it
        CheckDXErrorFatal( g_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 ));
        // render all lighted meshes
        for( auto mesh : lightedMeshes )
        {
            mesh->GetDiffuseTexture()->Bind( 0 );
            D3DXMATRIX world, wvp;
            GetD3DMatrixFromBulletTransform( mesh->ownerNode->globalTransform, world );
            D3DXMatrixMultiply( &wvp, &world, &viewProj );
            CheckDXErrorFatal( vertexShader->GetConstantTable()->SetMatrix( g_device, vWVP, &wvp ));
            CheckDXErrorFatal( vertexShader->GetConstantTable()->SetMatrix( g_device, vWorld, &world ));
            CheckDXErrorFatal( pixelShader->GetConstantTable()->SetFloatArray( g_device, pLightPosition, light->GetRealPosition().elements, 3 ));
            mesh->BindBuffers();
            mesh->Render();
        };
    }

    CheckDXErrorFatal( state->Apply());
    state->Release();

    // revert to the last used render target
    CheckDXErrorFatal( g_device->SetRenderTarget( prevRTNum, prevRT ));
    CheckDXErrorFatal( g_device->SetDepthStencilSurface( defaultDepthStencil ));

    lightedMeshes.clear();
}

void PointlightShadowMap::UnbindShadowCubemap( int level )
{
    CheckDXErrorFatal( g_device->SetTexture( level, nullptr ));
}

void PointlightShadowMap::BindShadowCubemap( int level )
{
    CheckDXErrorFatal( g_device->SetTexture( level, shadowCube ));
}

PointlightShadowMap::~PointlightShadowMap()
{
    for( int i = 0; i < 6; i++ )
        cubeFaces[i]->Release();
    shadowCube->Release();
    depthStencil->Release();
    delete pixelShader;
    delete vertexShader;
}

PointlightShadowMap::PointlightShadowMap( int faceSize )
{
    iSize = faceSize;

    CheckDXErrorFatal( D3DXCreateCubeTexture( g_device, faceSize, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &shadowCube ));
    CheckDXErrorFatal( shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_NEGATIVE_X, 0, &cubeFaces[ D3DCUBEMAP_FACE_NEGATIVE_X ] ));
    CheckDXErrorFatal( shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_POSITIVE_X, 0, &cubeFaces[ D3DCUBEMAP_FACE_POSITIVE_X ] ));
    CheckDXErrorFatal( shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_NEGATIVE_Y, 0, &cubeFaces[ D3DCUBEMAP_FACE_NEGATIVE_Y ] ));
    CheckDXErrorFatal( shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_POSITIVE_Y, 0, &cubeFaces[ D3DCUBEMAP_FACE_POSITIVE_Y ] ));
    CheckDXErrorFatal( shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_NEGATIVE_Z, 0, &cubeFaces[ D3DCUBEMAP_FACE_NEGATIVE_Z ] ));
    CheckDXErrorFatal( shadowCube->GetCubeMapSurface( D3DCUBEMAP_FACE_POSITIVE_Z, 0, &cubeFaces[ D3DCUBEMAP_FACE_POSITIVE_Z ] ));
    CheckDXErrorFatal( g_device->GetDepthStencilSurface( &defaultDepthStencil ));
    CheckDXErrorFatal( g_device->CreateDepthStencilSurface( faceSize, faceSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &depthStencil, nullptr ));

    // create shader to render shadowmaps
    string vertexShaderSource =
        "float4x4 wvp;\n"
        "float4x4 world;\n"

        "struct VSOutput {\n"
        "   float4 position : POSITION;\n"
        "   float2 texCoord : TEXCOORD0;\n"
        "   float3 worldPos : TEXCOORD1;\n"
        "};\n"

        "VSOutput main( float4 position : POSITION, float2 texCoord : TEXCOORD0 ) {\n"
        "   VSOutput output = (VSOutput)0;\n"
        "   output.position = mul( position, wvp );\n"
        "   output.worldPos = mul( position, world );\n"
        "   output.texCoord = texCoord;\n"
        "   return output;\n"
        "};\n";
    vertexShader = new VertexShader( vertexShaderSource );

    vWVP = vertexShader->GetConstantTable()->GetConstantByName( 0, "wvp" );
    vWorld = vertexShader->GetConstantTable()->GetConstantByName( 0, "world" );

    string pixelShaderSource =
        "sampler diffuseSampler : register( s0 );\n"
        "float3 lightPos;\n"
        "float4 main( float2 texCoord : TEXCOORD0, float3 worldPos : TEXCOORD1) : COLOR0 {\n"
        "   clip( tex2D( diffuseSampler, texCoord ).a - 0.1 );\n"
        "   float delta = lightPos - worldPos;\n"
        "   float lightToPixelSqrDist = dot( delta, delta );\n"
        "   return float4( lightToPixelSqrDist, 0.0f, 0.0f, 1.0f );\n"
        "};\n";

    pixelShader = new PixelShader( pixelShaderSource );
    pLightPosition = pixelShader->GetConstantTable()->GetConstantByName( 0, "lightPos" );
}






































/*
if( i == D3DCUBEMAP_FACE_POSITIVE_X ) {
lookAt = D3DXVECTOR3( 1, 0, 0 );
up = D3DXVECTOR3( 0, 1, 0 );
}
if( i == D3DCUBEMAP_FACE_NEGATIVE_X ) {
lookAt = D3DXVECTOR3( -1, 0, 0 );
up = D3DXVECTOR3( 0, 1, 0 );
}
if( i == D3DCUBEMAP_FACE_POSITIVE_Y ) {
lookAt = D3DXVECTOR3( 0, 1, 0 );
up = D3DXVECTOR3( 0, 0, -1 );
}
if( i == D3DCUBEMAP_FACE_NEGATIVE_Y ) {
lookAt = D3DXVECTOR3( 0, -1, 0 );
up = D3DXVECTOR3( 0, 0, 1 );
}
if( i == D3DCUBEMAP_FACE_POSITIVE_Z ) {
lookAt = D3DXVECTOR3( 0, 0, 1 );
up = D3DXVECTOR3( 0, 1, 0 );
}
if( i == D3DCUBEMAP_FACE_NEGATIVE_Z ) {
lookAt = D3DXVECTOR3( 0, 0, -1 );
up = D3DXVECTOR3( 0, 1, 0 );
}*/

/*
if( i == D3DCUBEMAP_FACE_POSITIVE_X ) {
lookAt = D3DXVECTOR3( 1, 0, 0 );
up = D3DXVECTOR3( 0, -1, 0 );
}
if( i == D3DCUBEMAP_FACE_NEGATIVE_X ) {
lookAt = D3DXVECTOR3( -1, 0, 0 );
up = D3DXVECTOR3( 0, -1, 0 );
}
if( i == D3DCUBEMAP_FACE_POSITIVE_Y ) {
lookAt = D3DXVECTOR3( 0, 1, 0 );
up = D3DXVECTOR3( 0, 0, -1 );
}
if( i == D3DCUBEMAP_FACE_NEGATIVE_Y ) {
lookAt = D3DXVECTOR3( 0, -1, 0 );
up = D3DXVECTOR3( 0, 0, 1 );
}
if( i == D3DCUBEMAP_FACE_POSITIVE_Z ) {
lookAt = D3DXVECTOR3( 0, 0, 1 );
up = D3DXVECTOR3( 0, -1, 0 );
}
if( i == D3DCUBEMAP_FACE_NEGATIVE_Z ) {
lookAt = D3DXVECTOR3( 0, 0, -1 );
up = D3DXVECTOR3( 0, -1, 0 );
}*/

