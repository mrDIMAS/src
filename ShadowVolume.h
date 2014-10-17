#pragma once

#include "Common.h"
#include "Mesh.h"

class ShadowVolume {
private:
    class Face {
    private:
        short v1, v2, v3;
    public:

        Face( short theV1, short theV2, short theV3 );
    };

    IDirect3DVertexBuffer9 * vertexBuffer;
    Mesh * caster;
    Light * light;
public:
    ShadowVolume( Light * theLight, Mesh * theCaster );

    void CreateSilhouette();

    void Update();

    void Render();
};