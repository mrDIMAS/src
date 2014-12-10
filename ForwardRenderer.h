#pragma once

#include "Common.h"
#include "Light.h"
#include "Camera.h"
#include "Mesh.h"
#include "Utility.h"
#include "Shader.h"

// must be used to render transparent faces only
// in other cases prefer DeferredRenderer
class ForwardRenderer
{
private:
    map< IDirect3DTexture9*, vector< Mesh* > > renderList;
    PixelShader * pixelShader;
    VertexShader * vertexShader;
public:
    ForwardRenderer();
    ~ForwardRenderer();
    void AddMesh( Mesh * mesh );
    void RemoveMesh( Mesh * mesh );
    void RenderMeshes();
};