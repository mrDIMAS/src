#include "ForwardRenderer.h"

ForwardRenderer * g_forwardRenderer = nullptr;

void ForwardRenderer::RenderMeshes() {
    mPixelShader->Bind();
    mVertexShader->Bind();

    for( auto group : mRenderList ) {
        IDirect3DTexture9 * diffuseTexture = group.first;
        vector< Mesh* > & meshes = group.second;

        gpDevice->SetTexture( 0, diffuseTexture );

        for( auto pMesh : meshes ) {
            D3DXMATRIX world, wvp;

			// draw instances
			for( auto pOwner : pMesh->GetOwners() ) {
				GetD3DMatrixFromBulletTransform( pOwner->mGlobalTransform, world );
				D3DXMatrixMultiplyTranspose( &wvp, &world, &g_camera->mViewProjection );

				CheckDXErrorFatal( gpDevice->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 ));
				CheckDXErrorFatal( gpDevice->SetPixelShaderConstantF( 0, &pMesh->mOpacity, 1 ));

				pMesh->BindBuffers();
				pMesh->Render();
			}
        }
    }
}

void ForwardRenderer::RemoveMesh( Mesh * mesh ) {
    auto groupIter = mRenderList.find( mesh->mDiffuseTexture->GetInterface() );
    if( groupIter != mRenderList.end() ) {
        auto & group = groupIter->second;
        group.erase( find( group.begin(), group.end(), mesh ));
    }
}

void ForwardRenderer::AddMesh( Mesh * mesh ) {
    mRenderList[ mesh->mDiffuseTexture->GetInterface() ].push_back( mesh );
}

ForwardRenderer::~ForwardRenderer() {
    delete mPixelShader;
    delete mVertexShader;
}

ForwardRenderer::ForwardRenderer() {
    mVertexShader = new VertexShader( "data/shaders/forwardTransparent.vso", true );
    mPixelShader = new PixelShader( "data/shaders/forwardTransparent.pso", true );
}
