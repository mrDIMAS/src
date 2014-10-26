#pragma once

#include "Shader.h"
#include "GBuffer.h"
#include "EffectsQuad.h"
#include "FXAA.h"
#include "SSAO.h"
#include "ShadowMap.h"

class Mesh;

class DeferredRenderer {
public:
    class BoundingVolumeRenderingShader {
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
    class Pass2AmbientLight {
    private:
        PixelShader * pixelShader;

        D3DXHANDLE hAmbientColor;

    public:
        Pass2AmbientLight();
        ~Pass2AmbientLight();

        void Bind( );
    };

    // Point Light
    class Pass2PointLight {
    private:
        D3DXHANDLE hLightPos;
        D3DXHANDLE hLightRange;
        D3DXHANDLE hCameraPos;
        D3DXHANDLE hInvViewProj;
        D3DXHANDLE hLightColor;

        D3DXHANDLE hUsePointTexture;

        PixelShader * pixelShader;
    public:
        Pass2PointLight();
        ~Pass2PointLight();

        void Bind( D3DXMATRIX & invViewProj );
        void BindShader( );
        void SetLight( Light * lit );
    };

    // Spot Light
    class Pass2SpotLight {
    private:
        PixelShader * pixelShader;

        D3DXHANDLE hLightPos;
        D3DXHANDLE hLightRange;
        D3DXHANDLE hCameraPos;
        D3DXHANDLE hInvViewProj;
        D3DXHANDLE hLightColor;

        D3DXHANDLE hInnerAngle;
        D3DXHANDLE hOuterAngle;
        D3DXHANDLE hDirection;

        D3DXHANDLE hUseSpotTexture;
        D3DXHANDLE hSpotViewProjMatrix;
        D3DXHANDLE hUseShadows;
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
    SSAO * ssao;
    ShadowMap * shadowMap;

    void CreateBoundingVolumes();

    void RenderIcosphereIntoStencilBuffer( float lightRadius, const btVector3 & lightPosition );
    void RenderConeIntoStencilBuffer( Light * lit );

    void RenderScreenQuad();
    void RenderMeshShadow( Mesh * mesh );
    void ConfigureStencilBuffer();
public:
    explicit DeferredRenderer();
    virtual ~DeferredRenderer();

    GBuffer * GetGBuffer();

    virtual void BeginFirstPass() = 0;
    virtual void RenderMesh( Mesh * mesh ) = 0;
    virtual void OnEnd() = 0;

    void EndFirstPassAndDoSecondPass();
};
