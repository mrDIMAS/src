#pragma once

#include "Common.h"

class Mesh;

class SceneNode {
public:
	friend class DeferredRenderer;

    volatile int memoryTag;

    SceneNode * parent;
	SceneNode * scene; 

    vector<SceneNode*> childs;
	vector<Mesh*> meshes;  
	vector<SoundHandle> sounds;
	vector<btTransform*> keyframes;

	map<string,string> properties;

    btTransform invBoneBindTransform;

    btRigidBody * body;   
    
    string name;     
    
    btTriangleMesh * trimesh;   
    
    SoundHandle hitSound;
    SoundHandle idleSound;

    float albedo;
    float fDepthHack;
	
	Animation * currentAnimation;

	bool animationEnabled;
	bool skinned;
	bool inFrustum;
	bool frozen;
	bool visible;

	Contact contacts[ BODY_MAX_CONTACTS ];

	int numContacts;
	int totalFrames;
public:
    // Components
    ParticleEmitter * particleEmitter;

    btTransform globalTransform;
    btTransform localTransform;

    // Methods
    explicit SceneNode();
    virtual ~SceneNode();

	void SetAnimation( Animation * newAnim, bool dontAffectChilds = false );
	Animation * GetCurrentAnimation( ) {
		return currentAnimation;
	}

    void EraseChild( const SceneNode * child );
    void SetConvexBody( );
    void SetBoxBody(  );
    void SetSphereBody( );
    void SetCylinderBody( );
    void SetCapsuleBody( float height, float radius );
    void SetAngularFactor( Vector3 fact );
    void SetTrimeshBody();
    void AttachTo( SceneNode * parent );
    btTransform & CalculateGlobalTransform();
    Vector3 GetAABBMin( );
    Vector3 GetAABBMax( );
    static SceneNode * Find( SceneNode * parent, string childName );
    int IsAnimationEnabled( );
    void PerformAnimation( );
    void Freeze( );
    void Unfreeze();
    void SetAnimationEnabled( bool state, bool dontAffectChilds = false );
    void Hide( );
    void Show( );
    string GetProperty( string propName );
    void UpdateSounds( );
    void SetLinearFactor( Vector3 lin );
    Vector3 GetPosition( );
    int GetContactCount( );
    void SetBody( btRigidBody * theBody );
    Contact GetContact( int num );
    void ApplyProperties( );
    void AttachSound( SoundHandle sound );
    bool IsNodeInside( SceneNode * node );
    SceneNode * GetChild( int i );
    int GetCountChildren();
    void SetFriction( float friction );
    void SetDepthHack( float depthHack );
    void SetAnisotropicFriction( Vector3 aniso );
    void Move( Vector3 speed );
    void SetVelocity( Vector3 velocity );
    void SetAngularVelocity( Vector3 velocity );
    Vector3 GetEulerAngles();
    Quaternion GetLocalRotation();
    void SetPosition( Vector3 position );
    void SetDamping( float linearDamping, float angularDamping );
    void SetGravity( const Vector3 & gravity );
    void SetMass( float mass );
    float GetMass();
    bool IsFrozen();
    void SetRotation( Quaternion rotation );
    Vector3 GetLookVector();
	Vector3 GetAbsoluteLookVector();
    const char * GetName();
    Vector3 GetRightVector();
    Vector3 GetUpVector();
    btTransform & GetGlobalTransform();
    bool IsRenderable();
    bool IsVisible();
    Vector3 GetLocalPosition();

    static void UpdateContacts( );
    static SceneNode * FindByName( const char * name );
    static SceneNode * FindInObjectByName( SceneNode * node, const char * name );
    static SceneNode * LoadScene( const char * file );
    static SceneNode * CastHandle( NodeHandle handle );
    static NodeHandle HandleFromPointer( SceneNode * ptr );
    static void EraseUnusedNodes();
};