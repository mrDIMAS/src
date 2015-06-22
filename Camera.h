#pragma once

#include "SceneNode.h"
#include "Frustum.h"
#include "Light.h"

class Light;
class Skybox;

class Camera : public SceneNode {
public:
    float mFov;
    float mNearZ;
    float mFarZ;

    D3DXMATRIX mProjection;
    D3DXMATRIX mView;
    D3DXMATRIX mViewProjection;
    D3DXMATRIX invViewProjection;
    D3DXMATRIX mDepthHackMatrix;
    Frustum mFrustum;
    bool mInDepthHack;
    Skybox * mSkybox;

	// dynamic light caching
	class PathPoint {
	public:
		ruVector3 mPoint;
		// list of lights visible from that point, filled by occlusion queries
		vector<Light*> mVisibleLightList;
	};
	PathPoint * mDefaultPathPoint;
	PathPoint * mNearestPathPoint;
	ruVector3 mLastPosition;
	vector<PathPoint*> mPath;
	float mPathNewPointDelta;
	void ManagePath();

	static Camera * msCurrentCamera;
public:
    explicit Camera( float fov );
    virtual ~Camera();
    void CalculateProjectionMatrix();
    void CalculateInverseViewProjection();
    void SetSkyBox( Texture * up, Texture * left, Texture * right, Texture * forward, Texture * back );
    void Update();
    void EnterDepthHack( float depth );
    void LeaveDepthHack( );
    static Camera * CastHandle( ruNodeHandle handle );
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};