/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/
#pragma once

class Mesh;

class SceneNode : public virtual ruSceneNode, public RendererComponent {
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
	float mBlurAmount;
    ruAnimation * mCurrentAnimation;
    bool mIsSkinned;
    bool mInFrustum;
	bool mStatic;
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
    btTransform mGlobalTransform;
    btTransform mLocalTransform;

	virtual void OnLostDevice();
	virtual void OnResetDevice();

    // Methods
    explicit SceneNode( );
	explicit SceneNode( const SceneNode & source );
    virtual ~SceneNode( );

	virtual ruVector3 GetRotationAxis( );
	virtual float GetRotationAngle( );
    virtual void SetAnimation( ruAnimation * newAnim, bool dontAffectChilds = false );
    virtual ruAnimation * GetCurrentAnimation( );
    virtual void EraseChild( const SceneNode * child );
    virtual void SetConvexBody( );
    virtual void SetBoxBody(  );
    virtual void SetSphereBody( );
    virtual void SetCylinderBody( );
    virtual void SetCapsuleBody( float height, float radius );
    virtual void SetAngularFactor( ruVector3 fact );
    virtual void SetTrimeshBody();
    virtual void Attach( ruSceneNode * parent );
	virtual void Detach();
	virtual void AddForce( ruVector3 force );
	virtual void AddForceAtPoint( ruVector3 force, ruVector3 point );
	virtual void AddTorque( ruVector3 torque );
    virtual btTransform & CalculateGlobalTransform();
    virtual ruVector3 GetAABBMin( );
    virtual ruVector3 GetAABBMax( );    
    virtual void PerformAnimation( );
	virtual int GetTextureCount();
	virtual shared_ptr<ruTexture> GetTexture( int n );
	virtual int GetTotalAnimationFrameCount();
    virtual void Freeze( );
	virtual bool IsStatic();
    virtual void Unfreeze();
	virtual ruVector3 GetTotalForce();
	virtual BodyType GetBodyType() const;
    virtual void Hide( );
    virtual void Show( );
	virtual bool IsDynamic();
	virtual void SetName( const string & name );
    virtual string GetProperty( string propName );
    virtual void UpdateSounds( );
    virtual void SetLinearFactor( ruVector3 lin );
    virtual ruVector3 GetPosition( );
    virtual int GetContactCount( );
    virtual void SetBody( btRigidBody * theBody );
    virtual ruContact GetContact( int num );
    virtual void ApplyProperties( );
    virtual void AttachSound( ruSound sound );
    virtual bool IsInsideNode( ruSceneNode * n );
    virtual SceneNode * GetChild( int i );
    virtual int GetCountChildren();
    virtual void SetFriction( float friction );
    virtual void SetDepthHack( float depthHack );
    virtual void SetAnisotropicFriction( ruVector3 aniso );
    virtual void Move( ruVector3 speed );
    virtual void SetVelocity( ruVector3 velocity );
    virtual void SetAngularVelocity( ruVector3 velocity );
    virtual ruVector3 GetEulerAngles();
    virtual ruQuaternion GetLocalRotation();
    virtual void SetPosition( ruVector3 position );
    virtual void SetDamping( float linearDamping, float angularDamping );
    virtual void SetGravity( const ruVector3 & gravity );
    virtual void SetMass( float mass );
    virtual float GetMass();
    virtual bool IsFrozen();
    virtual void SetRotation( ruQuaternion rotation );
    virtual ruVector3 GetLookVector();
    virtual ruVector3 GetAbsoluteLookVector();
    virtual const string & GetName();
    virtual ruVector3 GetRightVector();
    virtual ruVector3 GetUpVector();
    virtual btTransform & GetGlobalTransform();
    virtual bool IsRenderable();
    virtual bool IsVisible();
	virtual bool IsInFrustum();
	virtual void SetAlbedo( float albedo );
    virtual ruVector3 GetLocalPosition();
	virtual ruVector3 GetLinearVelocity();
	virtual void SetLocalScale( ruVector3 scale );
	virtual void SetLocalPosition( ruVector3 pos );
	virtual void SetLocalRotation( ruQuaternion rot );
	virtual SceneNode * GetParent();
	virtual void SetBlurAmount( float blurAmount );
	virtual float GetBlurAmount( );

    static void UpdateContacts( );
    static SceneNode * FindByName( const string & name );
    SceneNode * FindChild( const string & name );
    static SceneNode * LoadScene( const string & file );
    static void EraseUnusedNodes();
	static SceneNode * Find( SceneNode * parent, string childName );
};