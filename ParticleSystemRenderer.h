#pragma  once

#include "Shader.h"
#include "ParticleEmitter.h"

class ParticleSystemRenderer {
private:
    PixelShader * pixelShader;
    VertexShader * vertexShader;

    IDirect3DVertexDeclaration9 * vd;

    D3DXHANDLE vWVP;
	D3DXHANDLE vWorld;

    D3DXHANDLE pLightCount;
    D3DXHANDLE pColor;
    D3DXHANDLE pRange;
    D3DXHANDLE pPosition;
    D3DXHANDLE pWithLight;

    D3DXHANDLE pInvViewProj;
    D3DXHANDLE pThickness;
public:
    ParticleSystemRenderer();
    ~ParticleSystemRenderer();
    void RenderAllParticleSystems();
};