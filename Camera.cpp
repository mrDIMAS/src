#include "SceneNode.h"
#include "AABB.h"
#include "Camera.h"
#include "Skybox.h"

Camera * g_camera = 0;

Camera * Camera::CastHandle( NodeHandle handle ) {
    SceneNode * n = SceneNode::CastHandle( handle );

    Camera * camera = dynamic_cast< Camera* >( n );

    return camera;
}

void Camera::Update() {
    btVector3 eye = globalTransform.getOrigin();
    btVector3 look = eye + globalTransform.getBasis().getColumn ( 2 );
    btVector3 up = globalTransform.getBasis().getColumn ( 1 );

    btVector3 normlook = globalTransform.getBasis().getColumn ( 2 ).normalized();

    float f_look[ 3 ];
    f_look[ 0 ] = normlook.x();
    f_look[ 1 ] = normlook.y();
    f_look[ 2 ] = normlook.z();

    float f_up[ 3 ] = { 0.0f, 1.0f, 0.0f };

    // sound listener
    pfSetListenerOrientation ( f_look, f_up );
    pfSetListenerPosition ( eye.x(), eye.y(), eye.z() );

    // view matrix
    D3DXVECTOR3 ep (  eye.x(),  eye.y(),  eye.z() );
    D3DXVECTOR3 lv ( look.x(), look.y(), look.z() );
    D3DXVECTOR3 uv (   up.x(),   up.y(),   up.z() );
    D3DXMatrixLookAtRH ( &view, &ep, &lv, &uv );

    CalculateProjectionMatrix();
    CalculateInverseViewProjection();
}

void Camera::SetSkyBox( const char * path ) {
    skybox = new Skybox( path );
}

void Camera::CalculateInverseViewProjection() {
    D3DXMatrixMultiply( &viewProjection, &view, &projection );
    D3DXMatrixInverse( &invViewProjection, 0, &viewProjection );
}

void Camera::CalculateProjectionMatrix() {
    D3DVIEWPORT9 vp;
    g_device->GetViewport( &vp );
    D3DXMatrixPerspectiveFovRH( &projection, fov * 3.14159 / 180.0f, (float)vp.Width / (float)vp.Height, nearZ, farZ );
}

void Camera::BuildFrustum() {
    D3DXMATRIX viewProjection;
    D3DXMatrixMultiply ( &viewProjection, &view, &projection );

    // Left plane
    frustumPlanes[0].a = viewProjection._14 + viewProjection._11;
    frustumPlanes[0].b = viewProjection._24 + viewProjection._21;
    frustumPlanes[0].c = viewProjection._34 + viewProjection._31;
    frustumPlanes[0].d = viewProjection._44 + viewProjection._41;

    // Right plane
    frustumPlanes[1].a = viewProjection._14 - viewProjection._11;
    frustumPlanes[1].b = viewProjection._24 - viewProjection._21;
    frustumPlanes[1].c = viewProjection._34 - viewProjection._31;
    frustumPlanes[1].d = viewProjection._44 - viewProjection._41;

    // Top plane
    frustumPlanes[2].a = viewProjection._14 - viewProjection._12;
    frustumPlanes[2].b = viewProjection._24 - viewProjection._22;
    frustumPlanes[2].c = viewProjection._34 - viewProjection._32;
    frustumPlanes[2].d = viewProjection._44 - viewProjection._42;

    // Bottom plane
    frustumPlanes[3].a = viewProjection._14 + viewProjection._12;
    frustumPlanes[3].b = viewProjection._24 + viewProjection._22;
    frustumPlanes[3].c = viewProjection._34 + viewProjection._32;
    frustumPlanes[3].d = viewProjection._44 + viewProjection._42;

    // Near plane
    frustumPlanes[4].a = viewProjection._13;
    frustumPlanes[4].b = viewProjection._23;
    frustumPlanes[4].c = viewProjection._33;
    frustumPlanes[4].d = viewProjection._43;

    // Far plane
    frustumPlanes[5].a = viewProjection._14 - viewProjection._13;
    frustumPlanes[5].b = viewProjection._24 - viewProjection._23;
    frustumPlanes[5].c = viewProjection._34 - viewProjection._33;
    frustumPlanes[5].d = viewProjection._44 - viewProjection._43;

    // Normalize planes
    for ( int i = 0; i < 6; i++ )
        D3DXPlaneNormalize ( &frustumPlanes[i], &frustumPlanes[i] );
}

Camera::~Camera() {
    if( skybox )
        delete skybox;
}

Camera::Camera( float fov ) {
    this->fov = fov;
    nearZ     = 0.1f;
    farZ      = 6000.0f;
    skybox    = nullptr;
    g_camera  = this;

    CalculateProjectionMatrix();
    D3DXMatrixLookAtRH( &view, &D3DXVECTOR3( 0, 100, 100 ), &D3DXVECTOR3( 0, 0, 0), &D3DXVECTOR3( 0, 1, 0 ));
}

/////////////////////////////////////////////////////////////
// API
/////////////////////////////////////////////////////////////

NodeHandle CreateCamera( float fov ) {
    return SceneNode::HandleFromPointer( new Camera( fov ) );
}

void SetFOV( NodeHandle camera, float fov ) {
    Camera::CastHandle( camera )->fov = fov;
}

void SetCamera( NodeHandle node ) {
    g_camera = Camera::CastHandle( node );
}

int SetSkybox( NodeHandle node, const char * path ) {
    Camera::CastHandle( node )->SetSkyBox( path );

    return 1;
}
