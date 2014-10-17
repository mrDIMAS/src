#pragma once

#include "Shader.h"

class EffectsQuad {
public:
    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DVertexDeclaration9 * vertexDeclaration;

    struct QuadVertex {
        float x, y, z;
        float tx, ty;

        QuadVertex( float ax, float ay, float az, float atx, float aty );
    };

    VertexShader * vertexShader;

    D3DXMATRIX orthoProjection;
    D3DXHANDLE v2Proj;

    explicit EffectsQuad();
    virtual ~EffectsQuad();

    void Bind( );
    void Render();
};