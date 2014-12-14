#pragma once

#include "Shader.h"
#include "GBuffer.h"
#include "EffectsQuad.h"
#include "FXAA.h"
#include "SpotlightShadowMap.h"
#include "PointlightShadowMap.h"
#include "HDR.h"

class Mesh;

class DeferredRenderer
{
public:
    class BoundingVolumeRenderingShader
    {
    private:
        VertexShader * vs;
        PixelShader * ps;

        IDirect3DVertexDeclaration9 * vertexDeclaration;

        D3DXHANDLE vWVP;
    public:
        BoundingVolumeRenderingShader();
        ~BoundingVolumeRenderingShader();

        void Bind();
        void SetTransform( D3DXMATRIX & wvp );
    };

    BoundingVolumeRenderingShader * bvRenderer;

    ID3DXMesh * icosphere;
    ID3DXMesh * cone;


    // Ambient Light
    class Pass2AmbientLight
    {
    private:
        PixelShader * pixelShader;
    public:
        Pass2AmbientLight();
        ~Pass2AmbientLight();
        void Bind( );
    };

    // Point Light
    class Pass2PointLight
    {
    private:
        PixelShader * pixelShader;
        PixelShader * pixelShaderLQ;
    public:
        Pass2PointLight();
        ~Pass2PointLight();

        void Bind( D3DXMATRIX & invViewProj );
        void BindShader( );
        void SetLight( Light * lit );
    };

    // Spot Light
    class Pass2SpotLight
    {
    private:
        PixelShader * pixelShader;
    public:
        Pass2SpotLight( );
        ~Pass2SpotLight();
        void Bind( D3DXMATRIX & invViewProj );
        void BindShader( );
        void SetLight( Light * lit );
    };

    EffectsQuad * effectsQuad;
    EffectsQuad * debugQuad;

    GBuffer * gBuffer;

    Pass2AmbientLight * pass2AmbientLight;
    Pass2PointLight * pass2PointLight;
    Pass2SpotLight * pass2SpotLight;

    FXAA * fxaa;
    SpotlightShadowMap * spotShadowMap;
    PointlightShadowMap * pointShadowMap;
    HDRRenderer * hdrRenderer;

    // renderQuality can be 0 (lowest) or 1 (highest)
    char renderQuality;

    void CreateBoundingVolumes();

    void RenderIcosphereIntoStencilBuffer( Light * pLight );
    void RenderConeIntoStencilBuffer( Light * lit );
    void RenderMeshShadow( Mesh * mesh );
public:
    explicit DeferredRenderer();
    virtual ~DeferredRenderer();

    GBuffer * GetGBuffer();

    void SetRenderingQuality( char quality );
    virtual void BeginFirstPass() = 0;
    virtual void RenderMesh( Mesh * mesh ) = 0;
    virtual void OnEnd() = 0;

    void SetSpotLightShadowMapSize( int size );

    void SetPointLightShadowMapSize( int size );
    void EndFirstPassAndDoSecondPass();
};
