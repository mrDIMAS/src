#pragma once



class Mesh;

class ParticleEmitter;



class SceneNode : public RendererComponent {
public:
    friend class DeferredRenderer;
    SceneNode * mParent;
    SceneNode * mScene;
    vector<SceneNode*> mChildList;
    vector<Mesh*> mMeshList;
    vector<ruSound> mSoundList;
	vector<btTransform*> mKeyframeList;
    map<string,string> mProperties;
    btTransform mInvBoneBindTransform;    
    string mName;
    ruSound mHitSound;
    ruSound mIdleSound;
    float mAlbedo;
    float mDepthHack;
    ruAnimation * mCurrentAnimation;
    bool mIsSkinned;
    bool mInFrustum;
    bool mFrozen;
    bool mVisible;
	bool mIsBone;
    ruContact mContactList[ BODY_MAX_CONTACTS ];
    int mContactCount;
    int mTotalFrameCount;
	void AutoName();
	static vector< SceneNode* > msNodeList;

	vector<btRigidBody*> mBodyList;
	vector<btTriangleMesh*> mTrimeshList;
public:
    // Components
    ParticleEmitter * particleEmitter;

    btTransform mGlobalTransform;
    btTransform mLocalTransform;

	ruVector3 GetRotationAxis( );
	float GetRotationAngle( );

	virtual void OnLostDevice();
	virtual void OnResetDevice();
    // Methods
    explicit SceneNode();
	explicit SceneNode( const SceneNode & source );
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
	void AddForce( ruVector3 force );
	void AddForceAtPoint( ruVector3 force, ruVector3 point );
	void AddTorque( ruVector3 torque );
    btTransform & CalculateGlobalTransform();
    ruVector3 GetAABBMin( );
    ruVector3 GetAABBMax( );
    static SceneNode * Find( SceneNode * parent, string childName );
    void PerformAnimation( );
    void Freeze( );
    void Unfreeze();
	ruVector3 GetTotalForce();
	BodyType GetBodyType() const;
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
    void AttachSound( ruSound sound );
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
    const string & GetName();
    ruVector3 GetRightVector();
    ruVector3 GetUpVector();
    btTransform & GetGlobalTransform();
    bool IsRenderable();
    bool IsVisible();
    ruVector3 GetLocalPosition();
	ruVector3 GetLinearVelocity();
	void SetBodyLocalScaling( ruVector3 scale );
    static void UpdateContacts( );
    static SceneNode * FindByName( const string & name );
    static SceneNode * FindInObjectByName( SceneNode * node, const string & name );
    static SceneNode * LoadScene( const string & file );
    static SceneNode * CastHandle( ruSceneNode handle );
    static ruSceneNode HandleFromPointer( SceneNode * ptr );
    static void EraseUnusedNodes();
	void SetLocalPosition( ruVector3 pos );
	void SetLocalRotation( ruQuaternion rot );
	SceneNode * GetParent() {
		return mParent;
	}
};