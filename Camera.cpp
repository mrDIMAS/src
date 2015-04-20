#include "Precompiled.h"

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
    D3DXMatrixLookAtRH ( &mView, &ep, &lv, &uv );

    CalculateProjectionMatrix();
    CalculateInverseViewProjection();

    mFrustum.Build( mViewProjection );

	ManagePath();
}

void Camera::SetSkyBox( const string & path ) {
    if( path.size() ) {
        mSkybox = new Skybox( path );
    } else {
        if( mSkybox ) {
            delete mSkybox;

            mSkybox = nullptr;
        }
    }
}

void Camera::CalculateInverseViewProjection() {
    D3DXMatrixMultiply( &mViewProjection, &mView, &mProjection );
    D3DXMatrixInverse( &invViewProjection, 0, &mViewProjection );
}

void Camera::CalculateProjectionMatrix() {
    D3DVIEWPORT9 vp;
    gpDevice->GetViewport( &vp );
    D3DXMatrixPerspectiveFovRH( &mProjection, mFov * 3.14159 / 180.0f, (float)vp.Width / (float)vp.Height, mNearZ, mFarZ );
}

Camera::~Camera() {
    if( mSkybox ) {
        delete mSkybox;
    }
	for( auto pPoint : mPath ) {
		delete pPoint;
	}
}

Camera::Camera( float fov ) {
    this->mFov = fov;
    mNearZ = 0.025f;
    mFarZ = 6000.0f;
    mSkybox = nullptr;
    g_camera = this;
    mInDepthHack = false;
	mPathNewPointDelta = 5.0f;
	mNearestPathPoint = new PathPoint;
	mNearestPathPoint->mPoint = GetPosition();
	mPath.push_back( mNearestPathPoint );
    CalculateProjectionMatrix();
    D3DXMatrixLookAtRH( &mView, &D3DXVECTOR3( 0, 100, 100 ), &D3DXVECTOR3( 0, 0, 0), &D3DXVECTOR3( 0, 1, 0 ));
}

void Camera::EnterDepthHack( float depth ) {
    if( !mInDepthHack ) {
        mDepthHackMatrix = mProjection;
    }
    mInDepthHack = true;
    mProjection._43 -= depth;
    CalculateInverseViewProjection();
}

void Camera::LeaveDepthHack() {
    mInDepthHack = false;
    mProjection = mDepthHackMatrix;
    CalculateInverseViewProjection();
}

void Camera::ManagePath() {
	if( mPath.size() > 64 ) {
		for( auto pPoint : mPath ) {
			delete pPoint;
		}
		float mRangeSum = 0;
		for( auto pLight : g_pointLightList ) {
			mRangeSum += pLight->GetRadius();
		}
		mPathNewPointDelta = ( mRangeSum / g_pointLightList.size() ) / 2;
		mLastPosition = ruVector3( FLT_MAX, FLT_MAX, FLT_MAX );
		mPath.clear();
	}
	
	ruVector3 position = GetPosition();
	if( (position - mLastPosition).Length2() > mPathNewPointDelta ) {
		mLastPosition = position;
		bool addNewPoint = true;
		float distToNearest = -FLT_MAX;
		for( auto pPoint : mPath ) {
			float dist = (position - pPoint->mPoint ).Length2();
			if( dist < distToNearest ) {
				mNearestPathPoint = pPoint;
				distToNearest = dist;
			}
			if( dist < mPathNewPointDelta ) {				
				addNewPoint = false;
			}
		}
		if( addNewPoint ) {
			PathPoint * pathPoint = new PathPoint;
			pathPoint->mPoint = position;
			mPath.push_back( pathPoint );
			mNearestPathPoint = pathPoint;
		}
	}
}


/////////////////////////////////////////////////////////////
// API
/////////////////////////////////////////////////////////////

ruNodeHandle ruCreateCamera( float fov ) {
    return SceneNode::HandleFromPointer( new Camera( fov ) );
}

void ruSetCameraFOV( ruNodeHandle camera, float fov ) {
    Camera::CastHandle( camera )->mFov = fov;
}

void ruSetActiveCamera( ruNodeHandle node ) {
    g_camera = Camera::CastHandle( node );
}

int ruSetCameraSkybox( ruNodeHandle node, const string & path ) {
    Camera::CastHandle( node )->SetSkyBox( path );

    return 1;
}
