#pragma once


#include "Light.h"
#include "Camera.h"
#include "Mesh.h"
#include "Utility.h"
#include "Shader.h"

// must be used to render transparent faces only
// in other cases prefer DeferredRenderer
class ForwardRenderer {
private:
    map< IDirect3DTexture9*, vector< Mesh* > > mRenderList;
    shared_ptr<PixelShader> mPixelShader;
    shared_ptr<VertexShader> mVertexShader;
public:
    ForwardRenderer();
    ~ForwardRenderer();
    void AddMesh( Mesh * mesh );
    void RemoveMesh( Mesh * mesh );
    void RenderMeshes();
};