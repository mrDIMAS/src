#pragma once

#include "Common.h"

class Mesh;

class SceneNode {
public:
    friend class DeferredRenderer;
    SceneNode * mParent;
    SceneNode * mScene;
    vector<SceneNode*> mChildList;
    vector<Mesh*> mMeshList;
    vector<ruSoundHandle> mSoundList;
	vector<btTransform*> mKeyframeList;
    map<string,string> mProperties;
    btTransform mInvBoneBindTransform;
    btRigidBody * mBody;
    string mName;
    btTriangleMesh * trimesh;
    ruSoundHandle mHitSound;
    ruSoundHandle mIdleSound;
    float mAlbedo;
    float mDepthHack;
    ruAnimation * mCurrentAnimation;
    bool mAnimationEnabled;
    bool mSkinned;
    bool mInFrustum;
    bool mFrozen;
    bool mVisible;
    ruContact mContactList[ BODY_MAX_CONTACTS ];
    int mContactCount;
    int mTotalFrameCount;
public:
    // Components
    ParticleEmitter * particleEmitter;

    btTransform mGlobalTransform;
    btTransform mLocalTransform;

    // Methods
    explicit SceneNode();
    virtual ~SceneNode();

    void SetAnimation( ruAnimation * newAnim, bool dontAffectChilds = false );
    ruAnimation * GetCurrentAnimation( );

    void EraseChild( const SceneNode * child );
    void SetConvexBody( );
    void SetBoxBody(  );
    void SetSphereBody( );
    void SetCylinderBody( );
    void SetCapsuleBody( float height, float radius );
    void SetAngularFactor( ruVector3 fact );
    void SetTrimeshBody();
    void AttachTo( SceneNode * parent );
    btTransform & CalculateGlobalTransform();
    ruVector3 GetAABBMin( );
    ruVector3 GetAABBMax( );
    static SceneNode * Find( SceneNode * parent, string childName );
    int IsAnimationEnabled( );
    void PerformAnimation( );
    void Freeze( );
    void Unfreeze();
	ruVector3 GetTotalForce();;
	BodyType GetBodyType();
    void SetAnimationEnabled( bool state, bool dontAffectChilds = false );
    void Hide( );
    void Show( );
    string GetProperty( string propName );
    void UpdateSounds( );
    void SetLinearFactor( ruVector3 lin );
    ruVector3 GetPosition( );
    int GetContactCount( );
    void SetBody( btRigidBody * theBody );
    ruContact GetContact( int num );
    void ApplyProperties( );
    void AttachSound( ruSoundHandle sound );
    bool IsNodeInside( SceneNode * node );
    SceneNode * GetChild( int i );
    int GetCountChildren();
    void SetFriction( float friction );
    void SetDepthHack( float depthHack );
    void SetAnisotropicFriction( ruVector3 aniso );
    void Move( ruVector3 speed );
    void SetVelocity( ruVector3 velocity );
    void SetAngularVelocity( ruVector3 velocity );
    ruVector3 GetEulerAngles();
    ruQuaternion GetLocalRotation();
    void SetPosition( ruVector3 position );
    void SetDamping( float linearDamping, float angularDamping );
    void SetGravity( const ruVector3 & gravity );
    void SetMass( float mass );
    float GetMass();
    bool IsFrozen();
    void SetRotation( ruQuaternion rotation );
    ruVector3 GetLookVector();
    ruVector3 GetAbsoluteLookVector();
    const char * GetName();
    ruVector3 GetRightVector();
    ruVector3 GetUpVector();
    btTransform & GetGlobalTransform();
    bool IsRenderable();
    bool IsVisible();
    ruVector3 GetLocalPosition();
	void SetBodyLocalScaling( ruVector3 scale );
    static void UpdateContacts( );
    static SceneNode * FindByName( const char * name );
    static SceneNode * FindInObjectByName( SceneNode * node, const char * name );
    static SceneNode * LoadScene( const char * file );
    static SceneNode * CastHandle( ruNodeHandle handle );
    static ruNodeHandle HandleFromPointer( SceneNode * ptr );
    static void EraseUnusedNodes();
};