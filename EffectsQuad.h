#pragma once

#include "Shader.h"

class EffectsQuad {
public:
    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DVertexDeclaration9 * vertexDeclaration;

    struct QuadVertex {
        float x, y, z;
        float tx, ty;
    };

    bool debug;

    VertexShader * vertexShader;
    PixelShader * debugPixelShader;

    D3DXMATRIX orthoProjection;
    D3DXHANDLE v2Proj;

    explicit EffectsQuad( bool bDebug = false );
    virtual ~EffectsQuad();

    void SetSize( float width, float height );

    void Bind( );
    void Render();
};