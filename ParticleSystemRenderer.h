#pragma  once

#include "Shader.h"
#include "ParticleEmitter.h"

class ParticleSystemRenderer {
private:
    PixelShader * pixelShader;
    VertexShader * vertexShader;

    IDirect3DVertexDeclaration9 * vd;
public:
    ParticleSystemRenderer();
    ~ParticleSystemRenderer();
    void RenderAllParticleSystems();
};