/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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
class SceneFactory;

#include "Engine.h"

class SceneNode : public virtual ISceneNode, public std::enable_shared_from_this<SceneNode> {
protected:
	SceneFactory * const mFactory;
	friend class SceneFactory;
	friend class Physics; // so many friends :)
	weak_ptr<SceneNode> mParent;
	weak_ptr<SceneNode> mScene;
	vector<shared_ptr<SceneNode>> mChildren;
	vector<shared_ptr<Mesh>> mMeshList;
	vector<shared_ptr<ISound>> mSoundList;
	vector<unique_ptr<btTransform>> mKeyframeList;
	map<string, string> mProperties;
	btTransform mInverseBindTransform;
	string mTag;
	string mName;
	shared_ptr<ISound> mHitSound;
	shared_ptr<ISound> mIdleSound;
	float mAlbedo;
	float mDepthHack;
	float mBlurAmount;
	bool mIsSkinned;
	bool mInFrustum;
	bool mStatic;
	bool mFrozen;
	bool mVisible;
	bool mAnimationOverride;
	bool mIsBone;
	bool mIsMoving;
	Vector3 mLastPosition;
	bool mCollisionEnabled;
	Vector2 mTexCoordFlow;
	Contact mContactList[BODY_MAX_CONTACTS];
	int mContactCount;
	int mTotalFrameCount;
	void AutoName();
	bool mShadowCastEnabled;
	bool mVegetation;
	bool mAnimationBlendingEnabled;

	// animation stuff
	Animation * mCurrentAnimation;
	Animation * mLastAnimation;
	Animation * mTransitionAnimation;
	int mTransitionFrames; // in update frames, not animation
	int mCurrentTransitionFrame;

	// Objects of Bullet Physics does not fit into STL smartpointers, so bad
	vector<btRigidBody*> mBodyList;
	vector<btTriangleMesh*> mTrimeshList;

public:
	btTransform mGlobalTransform;
	btTransform mLocalTransform;
	SceneNode(SceneFactory * factory);

	// overload new/delete to be sure that scene node always be 16-aligned
	static void * operator new(size_t size) {
		return _mm_malloc(size, 16);
	}
	static void operator delete(void * ptr) {
		return _mm_free(ptr);
	}

	bool mTwoSidedLighting;

	// Internal static methods
	static void UpdateContacts();


	// Internal methods
	void AddMesh(const shared_ptr<Mesh> & mesh);
	virtual btTransform & CalculateGlobalTransform();
	virtual void PerformAnimation();
	virtual void UpdateSounds();
	virtual void SetBody(btRigidBody * theBody);
	virtual void ApplyProperties();
	virtual void AttachSound(const shared_ptr<ISound> & sound);
	virtual btTransform GetGlobalTransform() const;
	virtual D3DXMATRIX GetWorldMatrix();
	virtual void MakeBone();
	virtual btTransform GetLocalTransform();
	virtual btTransform GetInverseBindTransform();
	virtual btTransform GetRelativeTransform();
	virtual void CheckFrustum(Camera * pCamera);
	virtual shared_ptr<Mesh> GetMesh(int n);
	virtual btRigidBody * GetDynamicBody();
	virtual bool IsMoving() const;
	static shared_ptr<SceneNode> FindChildInNode(shared_ptr<SceneNode> node, const string & name);
	static shared_ptr<SceneNode> FindChildInNodeNonRecursive(const shared_ptr<SceneNode> parent, string childName);

	// API Methods
	virtual ~SceneNode();
	virtual SceneFactory * const GetFactory() const override final;
	virtual KeyFrame GetKeyFrame(int n) const override;
	virtual void SetTexCoordFlow(const Vector2 & flow) override;
	virtual Vector2 GetTexCoordFlow() const override;
	virtual Vector3 GetRotationAxis() override;
	virtual float GetRotationAngle() override;
	virtual void SetAnimation(Animation * newAnim, bool dontAffectChilds = false) override;
	virtual Animation * GetCurrentAnimation() override;
	virtual void SetConvexBody() override final;
	virtual void SetBoxBody() override final;
	virtual void SetSphereBody() override final;
	virtual void SetCylinderBody() override final;
	virtual void SetCapsuleBody(float height, float radius) override final;
	virtual void SetAngularFactor(Vector3 fact) override;
	virtual void SetTrimeshBody() override final;
	virtual void Attach(const shared_ptr<ISceneNode> & parent) override;
	virtual void Detach() override;
	virtual void AddForce(Vector3 force) override;
	virtual void AddForceAtPoint(Vector3 force, Vector3 point) override;
	virtual void AddTorque(Vector3 torque) override;
	virtual Vector3 GetAABBMin() override;
	virtual Vector3 GetAABBMax() override;
	virtual int GetTextureCount() override;
	virtual shared_ptr<ITexture> GetTexture(int n) override;
	virtual int GetTotalAnimationFrameCount() override;
	virtual void Freeze() override;
	virtual bool IsStatic() override;
	virtual void Unfreeze() override;
	virtual Vector3 GetTotalForce() override;
	virtual BodyType GetBodyType() const override;
	virtual void Hide() override;
	virtual void Show() override;
	virtual bool IsDynamic() override;
	virtual void SetName(const string & name) override;
	virtual string GetProperty(string propName) override;
	virtual void SetLinearFactor(Vector3 lin) override;
	virtual Vector3 GetPosition() const override;
	virtual int GetContactCount() override;
	virtual Contact GetContact(int num) override;
	virtual bool IsInsideNode(shared_ptr<ISceneNode> n) override;
	virtual shared_ptr<ISceneNode> GetChild(int i) override;
	virtual int GetCountChildren() override;
	virtual void SetFriction(float friction) override;
	virtual void SetDepthHack(float depthHack) override;
	virtual void SetAnisotropicFriction(Vector3 aniso) override;
	virtual void Move(Vector3 speed) override;
	virtual void SetVelocity(Vector3 velocity) override;
	virtual void SetAngularVelocity(Vector3 velocity) override;
	virtual Vector3 GetEulerAngles() override;
	virtual Quaternion GetLocalRotation() override;
	virtual void SetPosition(Vector3 position) override;
	virtual void SetDamping(float linearDamping, float angularDamping) override;
	virtual void SetGravity(const Vector3 & gravity) override;
	virtual void SetMass(float mass) override;
	virtual float GetMass() override;
	virtual bool IsFrozen() override;
	virtual void SetRotation(Quaternion rotation) override;
	virtual Vector3 GetLookVector() const override;
	virtual Vector3 GetAbsoluteLookVector() override;
	virtual const string GetName() override;
	virtual Vector3 GetRightVector() const override;
	virtual Vector3 GetUpVector() const override;
	virtual bool IsVisible() override;
	virtual bool IsInFrustum() override;
	virtual void SetAlbedo(float albedo) override;
	virtual bool IsSkinned() const override;
	virtual string GetTag() const override;
	virtual void SetTag(const string & tag) override;
	virtual float GetDepthHack() const override;
	virtual float GetAlbedo() const override;
	virtual bool IsBone() const override;
	virtual Vector3 GetLocalPosition() override;
	virtual Vector3 GetLinearVelocity() override;
	virtual void SetLocalScale(Vector3 scale) override;
	virtual void SetLocalPosition(Vector3 pos) override;
	virtual void SetLocalRotation(Quaternion rot) override;
	virtual shared_ptr<ISceneNode> GetParent() override;
	virtual void SetBlurAmount(float blurAmount) override;
	virtual float GetBlurAmount() override;
	virtual int GetMeshCount() const override;
	virtual void SetCollisionEnabled(bool state) override;
	virtual bool IsCollisionEnabled() const override;
	virtual shared_ptr<ISceneNode> FindChild(const string & name) override;
	virtual void SetShadowCastEnabled(bool state) override;
	virtual bool IsShadowCastEnabled() const override;
	virtual void SetVegetation(bool state) override;
	virtual bool IsVegetation() const override;
	virtual void SetAnimationOverride(bool state) override;
	virtual bool IsAnimationOverride() const override;
	virtual void SetAnimationBlendingEnabled(bool state) override;
	virtual bool IsAnimationBlendingEnabled(bool state) const override;
	virtual void SetOpacity(float opacity) override;
};

