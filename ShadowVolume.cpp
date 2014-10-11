#include "ShadowVolume.h"
#include "SceneNode.h"
#include "Light.h"

void ShadowVolume::Render()
{

}

void ShadowVolume::Update()
{

}

void ShadowVolume::CreateSilhouette()
{
  /*
  vector< Face > frontFaces;
  vector< Face > backFaces;

  btVector3 position = caster->parent->globalTransform.getOrigin();
  btVector3 lightPosition = light->base->globalTransform.getOrigin();

  
  for( int i = 0; i < caster->indices.size(); i += 3 )
  {
    VertexLink & vLinkA = caster->vertices[ caster->indices[ i + 0 ]];
    VertexLink & vLinkB = caster->vertices[ caster->indices[ i + 1 ]];
    VertexLink & vLinkC = caster->vertices[ caster->indices[ i + 2 ]];

    btVector3 vPosA = btVector3( vLinkA.x, vLinkA.y, vLinkA.z );
    btVector3 vPosB = btVector3( vLinkB.x, vLinkB.y, vLinkB.z );
    btVector3 vPosC = btVector3( vLinkC.x, vLinkC.y, vLinkC.z );

    btVector3 lightDirection = vPosA - lightPosition;
    btVector3 faceNormal = btVector3( caster->faceNormals[ i / 3 ].x, caster->faceNormals[ i / 3 ].y, caster->faceNormals[ i / 3 ].z );

    //if( faceNormal.dot( lightDirection ) )
  }*/
}

ShadowVolume::ShadowVolume( Light * theLight, Mesh * theCaster )
{
  caster = theCaster;
}

ShadowVolume::Face::Face( short theV1, short theV2, short theV3 )
{
  v1 = theV1;
  v2 = theV2;
  v3 = theV3;
}
