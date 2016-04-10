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
class Camera;

class SceneNode : public virtual ruSceneNode, public RendererComponent, public std::enable_shared_from_this<SceneNode> {
protected:
	friend class SceneFactory;
	explicit SceneNode( );
    weak_ptr<SceneNode> mParent;
    weak_ptr<SceneNode> mScene;
    vector<shared_ptr<SceneNode>> mChildList;
    vector<shared_ptr<Mesh>> mMeshList;
    vector<shared_ptr<ruSound>> mSoundList;
	vector<unique_ptr<btTransform>> mKeyframeList;
    map<string,string> mProperties;
    btTransform mInverseBindTransform;    
    string mName;
    shared_ptr<ruSound> mHitSound;
    shared_ptr<ruSound> mIdleSound;
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
	bool mCollisionEnabled;
	ruVector2 mTexCoordFlow;
    ruContact mContactList[ BODY_MAX_CONTACTS ];
    int mContactCount;
    int mTotalFrameCount;
	void AutoName();
	// Objects of Bullet Physics does not fit into STL smartpointers, so bad
	vector<btRigidBody*> mBodyList;
	vector<btTriangleMesh*> mTrimeshList;
	btTransform mGlobalTransform;
	btTransform mLocalTransform;
public:
	static void UpdateContacts( );
	static shared_ptr<SceneNode> FindByName( const string & name );    
	static shared_ptr<SceneNode> LoadScene( const string & file );
	static shared_ptr<SceneNode> Find( const shared_ptr<SceneNode> parent, string childName );

	virtual void OnLostDevice();
	virtual void OnResetDevice();

    // Methods
    virtual ~SceneNode( );
	virtual void SetTexCoordFlow( const ruVector2 & flow ) {
		mTexCoordFlow = flow;
	}
	virtual ruVector2 GetTexCoordFlow( ) const {
		return mTexCoordFlow;
	}
	void AddMesh( const shared_ptr<Mesh> & mesh );
	virtual ruVector3 GetRotationAxis( );
	virtual float GetRotationAngle( );
    virtual void SetAnimation( ruAnimation * newAnim, bool dontAffectChilds = false );
    virtual ruAnimation * GetCurrentAnimation( );
    virtual void SetConvexBody( );
    virtual void SetBoxBody(  );
    virtual void SetSphereBody( );
    virtual void SetCylinderBody( );
    virtual void SetCapsuleBody( float height, float radius );
    virtual void SetAngularFactor( ruVector3 fact );
    virtual void SetTrimeshBody();
    virtual void Attach( const shared_ptr<ruSceneNode> & parent );
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
    virtual ruVector3 GetPosition( ) const;
    virtual int GetContactCount( );
    virtual void SetBody( btRigidBody * theBody );
    virtual ruContact GetContact( int num );
    virtual void ApplyProperties( );
    virtual void AttachSound( const shared_ptr<ruSound> & sound );
    virtual bool IsInsideNode( shared_ptr<ruSceneNode> n );
    virtual shared_ptr<ruSceneNode> GetChild( int i );
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
    virtual ruVector3 GetLookVector() const;
    virtual ruVector3 GetAbsoluteLookVector();
    virtual const string GetName();
    virtual ruVector3 GetRightVector() const;
    virtual ruVector3 GetUpVector() const;
    virtual btTransform GetGlobalTransform() const;
    virtual bool IsVisible();
	virtual bool IsInFrustum();
	virtual void SetAlbedo( float albedo );
	virtual bool IsSkinned() const;
	virtual float GetDepthHack() const;
	virtual float GetAlbedo() const;
	virtual bool IsBone() const;
    virtual ruVector3 GetLocalPosition();
	virtual ruVector3 GetLinearVelocity();
	virtual void SetLocalScale( ruVector3 scale );
	virtual void SetLocalPosition( ruVector3 pos );
	virtual void SetLocalRotation( ruQuaternion rot );
	virtual shared_ptr<ruSceneNode> GetParent();
	virtual void SetBlurAmount( float blurAmount );
	virtual float GetBlurAmount( );
	virtual D3DXMATRIX GetWorldMatrix();
	virtual void MakeBone();
	virtual int GetMeshCount() const;
	virtual void SetCollisionEnabled( bool state ) {
		mCollisionEnabled = state;
	}
	virtual bool IsCollisionEnabled( ) const {
		return mCollisionEnabled;
	}
	virtual btTransform GetLocalTransform();
	virtual btTransform GetInverseBindTransform();
	virtual btTransform GetRelativeTransform();
	virtual void CheckFrustum( Camera * pCamera );
	virtual shared_ptr<ruSceneNode> FindChild( const string & name );
	virtual shared_ptr<Mesh> GetMesh( int n );
	static shared_ptr<SceneNode> FindChildInNode( shared_ptr<SceneNode> node, const string & name );
};
