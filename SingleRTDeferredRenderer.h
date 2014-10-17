#pragma once

#include "DeferredRenderer.h"

// For video card's that does not suppot MRT( like fucking GMA3150 )
class SingleRTDeferredRenderer : public DeferredRenderer {
public:
    class Pass1FillDiffuseMap {
    private:
        VertexShader * vs;
        PixelShader * ps;

        D3DXHANDLE vWVP;
    public:
        Pass1FillDiffuseMap();
        ~Pass1FillDiffuseMap();

        void Bind();
        void SetTransform( D3DXMATRIX & wvp );
    };

    class Pass1FillNormalMap {
    public:
        VertexShader * vs;
        PixelShader * ps;

        D3DXHANDLE vWVP;
        D3DXHANDLE vWorld;
    public:
        Pass1FillNormalMap();
        ~Pass1FillNormalMap();

        void Bind();
        void SetTransform( D3DXMATRIX & world, D3DXMATRIX & wvp );
    };

    class Pass1FillDepthMap {
    private:
        VertexShader * vs;
        PixelShader * ps;

        D3DXHANDLE vWVP;
    public:
        Pass1FillDepthMap();
        ~Pass1FillDepthMap();

        void Bind();
        void SetTransform( D3DXMATRIX & wvp );
    };


    Pass1FillDiffuseMap * pass1FillDiffuseMap;
    Pass1FillNormalMap * pass1FillNormalMap;
    Pass1FillDepthMap * pass1FillDepthMap;

    void SetDiffusePass();
    void SetNormalPass();
    void SetDepthPass();
    SingleRTDeferredRenderer();
    ~SingleRTDeferredRenderer();
    void BeginFirstPass();
    void RenderMesh( Mesh * mesh );
    void OnEnd();
};
