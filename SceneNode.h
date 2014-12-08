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
	vector<ruSoundHandle> sounds;
	vector<btTransform*> keyframes;

	map<string,string> properties;

    btTransform invBoneBindTransform;

    btRigidBody * body;   
    
    string name;     
    
    btTriangleMesh * trimesh;   
    
    ruSoundHandle hitSound;
    ruSoundHandle idleSound;

    float albedo;
    float fDepthHack;
	
	ruAnimation * currentAnimation;

	bool animationEnabled;
	bool skinned;
	bool inFrustum;
	bool frozen;
	bool visible;

	ruContact contacts[ BODY_MAX_CONTACTS ];

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

	void SetAnimation( ruAnimation * newAnim, bool dontAffectChilds = false );
	ruAnimation * GetCurrentAnimation( ) {
		return currentAnimation;
	}

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

    static void UpdateContacts( );
    static SceneNode * FindByName( const char * name );
    static SceneNode * FindInObjectByName( SceneNode * node, const char * name );
    static SceneNode * LoadScene( const char * file );
    static SceneNode * CastHandle( ruNodeHandle handle );
    static ruNodeHandle HandleFromPointer( SceneNode * ptr );
    static void EraseUnusedNodes();
};