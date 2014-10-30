#pragma once

#include "Common.h"
#include "Light.h"
#include "Mesh.h"
#include "Utility.h"
#include "Shader.h"

class PointlightShadowMap {
private:
    IDirect3DSurface9 * defaultDepthStencil;
    IDirect3DSurface9 * depthStencil;
    IDirect3DCubeTexture9 * shadowCube;
    IDirect3DSurface9 * cubeFaces[ 6 ];
    vector<Mesh*> lightedMeshes;
    PixelShader * pixelShader;
    VertexShader * vertexShader;
    D3DXHANDLE vWorld;
    D3DXHANDLE vWVP;
    D3DXHANDLE pLightPosition;    
public:
    int iSize;
    explicit PointlightShadowMap( int faceSize = 256 );
    virtual ~PointlightShadowMap();
    void BindShadowCubemap( int level );
    void UnbindShadowCubemap( int level );
    void RenderPointShadowMap( IDirect3DSurface9 * prevRT, int prevRTNum, Light * light );
};