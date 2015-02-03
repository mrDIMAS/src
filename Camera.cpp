#include "SceneNode.h"
#include "AABB.h"
#include "Camera.h"
#include "Skybox.h"

Camera * g_camera = 0;

Camera * Camera::CastHandle( ruNodeHandle handle ) {
    SceneNode * n = SceneNode::CastHandle( handle );

    Camera * camera = dynamic_cast< Camera* >( n );

    return camera;
}

void Camera::Update() {
    btVector3 eye = mGlobalTransform.getOrigin();
    btVector3 look = eye + mGlobalTransform.getBasis().getColumn ( 2 );
    btVector3 up = mGlobalTransform.getBasis().getColumn ( 1 );

    btVector3 normlook = mGlobalTransform.getBasis().getColumn ( 2 ).normalized();

    float f_up[ 3 ] = { 0.0f, 1.0f, 0.0f };

    // sound listener
    pfSetListenerOrientation( normlook.m_floats, f_up );
    pfSetListenerPosition( eye.x(), eye.y(), eye.z() );

    // view matrix
    D3DXVECTOR3 ep (  eye.x(),  eye.y(),  eye.z() );
    D3DXVECTOR3 lv ( look.x(), look.y(), look.z() );
    D3DXVECTOR3 uv (   up.x(),   up.y(),   up.z() );
    D3DXMatrixLookAtRH ( &view, &ep, &lv, &uv );

    CalculateProjectionMatrix();
    CalculateInverseViewProjection();

    frustum.Build( viewProjection );
}

void Camera::SetSkyBox( const char * path ) {
    if( path ) {
        skybox = new Skybox( path );
    } else {
        if( skybox ) {
            delete skybox;

            skybox = nullptr;
        }
    }
}

void Camera::CalculateInverseViewProjection() {
    D3DXMatrixMultiply( &viewProjection, &view, &projection );
    D3DXMatrixInverse( &invViewProjection, 0, &viewProjection );
}

void Camera::CalculateProjectionMatrix() {
    D3DVIEWPORT9 vp;
    gpDevice->GetViewport( &vp );
    D3DXMatrixPerspectiveFovRH( &projection, fov * 3.14159 / 180.0f, (float)vp.Width / (float)vp.Height, nearZ, farZ );
}

Camera::~Camera() {
    if( skybox ) {
        delete skybox;
    }
}

Camera::Camera( float fov ) {
    this->fov = fov;
    nearZ = 0.1f;
    farZ = 6000.0f;
    skybox = nullptr;
    g_camera = this;
    inDepthHack = false;
    CalculateProjectionMatrix();
    D3DXMatrixLookAtRH( &view, &D3DXVECTOR3( 0, 100, 100 ), &D3DXVECTOR3( 0, 0, 0), &D3DXVECTOR3( 0, 1, 0 ));
}

void Camera::EnterDepthHack( float depth ) {
    if( !inDepthHack ) {
        depthHackMatrix = projection;
    }
    inDepthHack = true;
    projection._43 -= depth;
    CalculateInverseViewProjection();
}

void Camera::LeaveDepthHack() {
    inDepthHack = false;
    projection = depthHackMatrix;
    CalculateInverseViewProjection();
}


/////////////////////////////////////////////////////////////
// API
/////////////////////////////////////////////////////////////

ruNodeHandle ruCreateCamera( float fov ) {
    return SceneNode::HandleFromPointer( new Camera( fov ) );
}

void ruSetCameraFOV( ruNodeHandle camera, float fov ) {
    Camera::CastHandle( camera )->fov = fov;
}

void ruSetActiveCamera( ruNodeHandle node ) {
    g_camera = Camera::CastHandle( node );
}

int ruSetCameraSkybox( ruNodeHandle node, const char * path ) {
    Camera::CastHandle( node )->SetSkyBox( path );

    return 1;
}
