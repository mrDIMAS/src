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

bool IsTextureFormatOk( D3DFORMAT TextureFormat )
{
    return SUCCEEDED( g_d3d->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, TextureFormat) );
}

DeferredRenderer::DeferredRenderer()
{
    effectsQuad = new EffectsQuad;
    debugQuad = new EffectsQuad( true );
    CreateBoundingVolumes();
    SetRenderingQuality( 1 );
    fxaa = new FXAA;
    gBuffer = new GBuffer;
    pass2SpotLight = new Pass2SpotLight;
    pass2AmbientLight = new Pass2AmbientLight;
    pass2PointLight = new Pass2PointLight;
    bvRenderer = new BoundingVolumeRenderingShader;
    spotShadowMap = new SpotlightShadowMap;
    pointShadowMap = new PointlightShadowMap;
    // check support of floating-point textures first
    if( IsTextureFormatOk( D3DFMT_A16B16G16R16 ))
        hdrRenderer = new HDRRenderer( D3DFMT_A16B16G16R16 );
    else
        hdrRenderer = nullptr;
}

DeferredRenderer::~DeferredRenderer()
{
    icosphere->Release();
    cone->Release();
    delete gBuffer;
    delete effectsQuad;
    delete pass2SpotLight;
    delete pass2AmbientLight;
    delete pass2PointLight;
    delete bvRenderer;
    delete spotShadowMap;
    delete fxaa;
    delete pointShadowMap;
    delete hdrRenderer;
    delete debugQuad;
}

GBuffer * DeferredRenderer::GetGBuffer()
{
    return gBuffer;
}

struct XYZNormalVertex
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
};

void DeferredRenderer::CreateBoundingVolumes()
{
    int quality = 6;

    D3DXCreateSphere( g_pDevice, 1.0, quality, quality, &icosphere, 0 );

    D3DXCreateCylinder( g_pDevice, 0.0f, 1.0f, 1.0f, quality, quality, &cone, 0 );

    // rotate cylinder on 90 degrees
    XYZNormalVertex * data;
    cone->LockVertexBuffer( 0, (void**)&data );
    D3DXMATRIX tran;
    D3DXMatrixTranslation( &tran, 0, -0.5, 0 );
    D3DXMATRIX rot90;
    D3DXMatrixRotationAxis( &rot90, &D3DXVECTOR3( 1, 0, 0 ), SIMD_HALF_PI );
    D3DXMATRIX transform;
    D3DXMatrixMultiply( &transform, &rot90, &tran );

    for( int i = 0; i < cone->GetNumVertices(); i++ )
    {
        XYZNormalVertex * v = &data[ i ];

        D3DXVec3TransformCoord( &v->p, &v->p, &transform );
    }

    cone->UnlockVertexBuffer();
}

////////////////////////////////////////////////////////////
// Point Light Subclass
////////////////////////////////////////////////////////////
DeferredRenderer::Pass2PointLight::Pass2PointLight()
{
    pixelShader = new PixelShader( "data/shaders/deferredPointLightHQ.pso", true );
    pixelShaderLQ = new PixelShader( "data/shaders/deferredPointLightLQ.pso", true );
}

void DeferredRenderer::Pass2PointLight::Bind( D3DXMATRIX & invViewProj )
{
    BindShader();
    g_renderer->SetPixelShaderFloat3( 8, g_camera->globalTransform.getOrigin().m_floats );
    g_renderer->SetPixelShaderMatrix( 0, &invViewProj );
}

void DeferredRenderer::Pass2PointLight::BindShader( )
{
    if( g_deferredRenderer->renderQuality > 0 )
        pixelShader->Bind();
    else
        pixelShaderLQ->Bind();
}

void DeferredRenderer::Pass2PointLight::SetLight( Light * light )
{
    g_renderer->SetPixelShaderFloat3( 5, light->GetRealPosition().elements ); // position
    g_renderer->SetPixelShaderFloat3( 6, light->GetColor().elements ); // color
    g_renderer->SetPixelShaderFloat( 7, powf( light->GetRadius(), 4 )); // range
    g_renderer->SetPixelShaderFloat( 9, g_hdrEnabled ? light->brightness : 1.0f ); // brightness
    if( light->pointTexture )
    {
        g_pDevice->SetTexture( 3, light->pointTexture->cubeTexture );
        g_renderer->SetPixelShaderBool( 0, 1 );
    }
    else
    {
        g_pDevice->SetTexture( 3, 0 );
        g_renderer->SetPixelShaderBool( 0, 0 );
    }
}

DeferredRenderer::Pass2PointLight::~Pass2PointLight()
{
    delete pixelShader;
}

////////////////////////////////////////////////////////////
// Ambient Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::Pass2AmbientLight::Pass2AmbientLight()
{
    pixelShader = new PixelShader( "data/shaders/deferredAmbientLight.pso", true );
}

void DeferredRenderer::Pass2AmbientLight::Bind( )
{
    pixelShader->Bind();
    g_renderer->SetPixelShaderFloat3( 0, g_ambientColor.elements );
}

DeferredRenderer::Pass2AmbientLight::~Pass2AmbientLight()
{
    delete pixelShader;
}

////////////////////////////////////////////////////////////
// Spot Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::Pass2SpotLight::Pass2SpotLight( )
{
    pixelShader = new PixelShader( "data/shaders/deferredSpotLight.pso", true );
}

void DeferredRenderer::Pass2SpotLight::BindShader( )
{
    pixelShader->Bind();
}

void DeferredRenderer::Pass2SpotLight::Bind( D3DXMATRIX & invViewProj )
{
    BindShader();

    g_renderer->SetPixelShaderFloat3( 13, g_camera->globalTransform.getOrigin().m_floats );
    g_renderer->SetPixelShaderMatrix( 0, &invViewProj );
}

void DeferredRenderer::Pass2SpotLight::SetLight( Light * lit )
{
    btVector3 direction = ( lit->globalTransform.getBasis() * btVector3( 0, 1, 0 )).normalize();
    // position
    g_renderer->SetPixelShaderFloat3( 10, lit->GetRealPosition().elements );
    // range
    g_renderer->SetPixelShaderFloat( 14, powf( lit->GetRadius(), 4 ));
    // color
    g_renderer->SetPixelShaderFloat3( 11, lit->GetColor().elements );
    // inner angle
    g_renderer->SetPixelShaderFloat( 15, lit->GetCosHalfInnerAngle() );
    // outer angle
    g_renderer->SetPixelShaderFloat( 16, lit->GetCosHalfOuterAngle() );
    // direction
    g_renderer->SetPixelShaderFloat3( 12, direction.m_floats );
    // use shadows
    g_renderer->SetPixelShaderBool( 1, g_useSpotLightShadows ? TRUE : FALSE );
    // brightness
    g_renderer->SetPixelShaderFloat( 17, (g_hdrEnabled ? lit->brightness : 1.0f) );
    if( lit->spotTexture || g_useSpotLightShadows )
    {
        lit->BuildSpotProjectionMatrixAndFrustum();
        if( lit->spotTexture )
        {
            lit->spotTexture->Bind( 3 );
            // use spot texture
            g_renderer->SetPixelShaderBool( 0, TRUE );
        }
        // spot view matrix
        g_renderer->SetPixelShaderMatrix( 5, &lit->spotViewProjectionMatrix );
    }
    else
    {
        g_pDevice->SetTexture( 3, nullptr );
        // use spot texture
        if( !lit->spotTexture )
            g_renderer->SetPixelShaderBool( 0, FALSE );
    }
}

DeferredRenderer::Pass2SpotLight::~Pass2SpotLight()
{
    delete pixelShader;
}

//////////////////////////////////////////////////////////////////////////
// Bounding volume rendering shader
//
// Bounding volume for a light can be a sphere for point light
// a oriented cone for a spot light
DeferredRenderer::BoundingVolumeRenderingShader::BoundingVolumeRenderingShader()
{
    vs = new VertexShader( "data/shaders/boundingVolume.vso", true );
    ps = new PixelShader( "data/shaders/boundingVolume.pso", true );
    D3DVERTEXELEMENT9 vd[ ] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        D3DDECL_END()
    };
    g_pDevice->CreateVertexDeclaration( vd, &vertexDeclaration ) ;
}

DeferredRenderer::BoundingVolumeRenderingShader::~BoundingVolumeRenderingShader()
{
    delete ps;
    delete vs;
    vertexDeclaration->Release();
}

void DeferredRenderer::BoundingVolumeRenderingShader::Bind()
{
    ps->Bind();
    vs->Bind();

    g_pDevice->SetVertexDeclaration( vertexDeclaration );
}

void DeferredRenderer::BoundingVolumeRenderingShader::SetTransform( D3DXMATRIX & wvp )
{
    g_renderer->SetVertexShaderMatrix( 0, &wvp );
}

void DeferredRenderer::RenderIcosphereIntoStencilBuffer( Light * pLight )
{
	ruVector3 realPosition = pLight->GetRealPosition();
    float scl = 2.5f * pLight->radius;
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
    world._41 = realPosition.x;
    world._42 = realPosition.y;
    world._43 = realPosition.z;
    world._44 = 1.0f;
    bvRenderer->Bind();
    D3DXMATRIX wvp;
    D3DXMatrixMultiply( &wvp, &world, &g_camera->viewProjection );
    bvRenderer->SetTransform( wvp );
    icosphere->DrawSubset( 0 );
}

void DeferredRenderer::RenderConeIntoStencilBuffer( Light * lit )
{
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
    cone->DrawSubset( 0 );
}

void DeferredRenderer::EndFirstPassAndDoSecondPass()
{
    OnEnd();

    if( hdrRenderer && g_hdrEnabled )
        hdrRenderer->SetAsRenderTarget();
    else
    {
        if( g_fxaaEnabled )
            fxaa->BeginDrawIntoTexture();
        else
            gBuffer->BindBackSurfaceAsRT();
    }

    g_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    gBuffer->BindTextures();
	/*
    if( g_camera->skybox )
        g_camera->skybox->Render( g_camera->globalTransform.getOrigin() );
	*/
	

    pass2AmbientLight->Bind();
    effectsQuad->Bind();
    effectsQuad->Render();

    // Render point lights
    for( auto pLight : g_pointLights )
    {
        if( g_usePointLightShadows )
        {
            IDirect3DSurface9 * prevSurface = nullptr;
            if( hdrRenderer && g_hdrEnabled )
                prevSurface = hdrRenderer->hdrSurface;
            else if( g_fxaaEnabled )
                prevSurface = fxaa->renderTarget;
            else
                prevSurface = gBuffer->backSurface;
            pointShadowMap->UnbindShadowCubemap( 4 );
            pointShadowMap->RenderPointShadowMap( prevSurface, 0, pLight );
            pointShadowMap->BindShadowCubemap( 4 );
        }
		

		g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
		g_pDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
		g_pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
        RenderIcosphereIntoStencilBuffer( pLight );

        pass2PointLight->Bind( g_camera->invViewProjection );
        pass2PointLight->SetLight( pLight );

        effectsQuad->Bind();

		g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
		g_pDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
		g_pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
		// quad render
		effectsQuad->Render();
    }
	
    // Render spot lights
    for( auto pLight : g_spotLights )
    {
        if( g_useSpotLightShadows )
        {
            IDirect3DSurface9 * prevSurface = nullptr;
            if( hdrRenderer && g_hdrEnabled )
                prevSurface = hdrRenderer->hdrSurface;
            else if( g_fxaaEnabled )
                prevSurface = fxaa->renderTarget;
            else
                prevSurface = gBuffer->backSurface;
            spotShadowMap->UnbindSpotShadowMap( 4 );

			g_pDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
			g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			g_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

            spotShadowMap->RenderSpotShadowMap( prevSurface, 0, pLight );

			g_pDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
			g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			g_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

            spotShadowMap->BindSpotShadowMap( 4 );
        }

		g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
		g_pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
		g_pDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

        RenderConeIntoStencilBuffer( pLight );

        pass2SpotLight->Bind( g_camera->invViewProjection );
        pass2SpotLight->SetLight( pLight );
		
		g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
		g_pDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
		g_pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
		effectsQuad->Bind();
		effectsQuad->Render();
    }
	
    if( hdrRenderer && g_hdrEnabled )
    {
        hdrRenderer->CalculateFrameLuminance( );
        if( g_fxaaEnabled )
        {
            hdrRenderer->DoToneMapping( fxaa->renderTarget );
            fxaa->DoAntialiasing( fxaa->texture );
        }
        else
            hdrRenderer->DoToneMapping( gBuffer->backSurface );
    }
    else
    {
        if( g_fxaaEnabled ) {
			CheckDXErrorFatal( g_pDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE ));
			CheckDXErrorFatal( g_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE ));
            fxaa->DoAntialiasing( fxaa->texture );
		}
    }
}

void DeferredRenderer::SetPointLightShadowMapSize( int size )
{
    if( size != pointShadowMap->iSize )
    {
        if( pointShadowMap )
            delete pointShadowMap;
        pointShadowMap = new PointlightShadowMap( size );
    }
}

void DeferredRenderer::SetSpotLightShadowMapSize( int size )
{
    if( size != spotShadowMap->iSize )
    {
        if( spotShadowMap )
            delete spotShadowMap;
        spotShadowMap = new SpotlightShadowMap( size );
    }
}

void DeferredRenderer::SetRenderingQuality( char quality )
{
	renderQuality = quality;
	if( renderQuality < 0 )
		renderQuality = 0;
	if( renderQuality > 1 )
		renderQuality = 1;
}
