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

class SceneNode : public virtual ruSceneNode, public std::enable_shared_from_this<SceneNode> {
protected:
	SceneFactory * const mFactory;
	friend class SceneFactory;
	friend class Physics; // so many friends :)
	weak_ptr<SceneNode> mParent;
	weak_ptr<SceneNode> mScene;
	vector<shared_ptr<SceneNode>> mChildren;
	vector<shared_ptr<Mesh>> mMeshList;
	vector<shared_ptr<ruSound>> mSoundList;
	vector<unique_ptr<btTransform>> mKeyframeList;
	map<string, string> mProperties;
	btTransform mInverseBindTransform;
	string mTag;
	string mName;
	shared_ptr<ruSound> mHitSound;
	shared_ptr<ruSound> mIdleSound;
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
	ruVector3 mLastPosition;
	bool mCollisionEnabled;
	ruVector2 mTexCoordFlow;
	ruContact mContactList[BODY_MAX_CONTACTS];
	int mContactCount;
	int mTotalFrameCount;
	void AutoName();
	bool mShadowCastEnabled;
	bool mVegetation;
	bool mAnimationBlendingEnabled;

	// animation stuff
	ruAnimation * mCurrentAnimation;
	ruAnimation * mLastAnimation;
	ruAnimation * mTransitionAnimation;
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
	virtual void AttachSound(const shared_ptr<ruSound> & sound);
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
	virtual SceneFactory * const GetFactory() const override final {
		return mFactory;
	}
	virtual ruKeyFrame GetKeyFrame(int n) const override;
	virtual void SetTexCoordFlow(const ruVector2 & flow) override;
	virtual ruVector2 GetTexCoordFlow() const override;
	virtual ruVector3 GetRotationAxis() override;
	virtual float GetRotationAngle() override;
	virtual void SetAnimation(ruAnimation * newAnim, bool dontAffectChilds = false) override;
	virtual ruAnimation * GetCurrentAnimation() override;
	virtual void SetConvexBody() override;
	virtual void SetBoxBody() override;
	virtual void SetSphereBody() override;
	virtual void SetCylinderBody() override;
	virtual void SetCapsuleBody(float height, float radius) override;
	virtual void SetAngularFactor(ruVector3 fact) override;
	virtual void SetTrimeshBody() override;
	virtual void Attach(const shared_ptr<ruSceneNode> & parent) override;
	virtual void Detach() override;
	virtual void AddForce(ruVector3 force) override;
	virtual void AddForceAtPoint(ruVector3 force, ruVector3 point) override;
	virtual void AddTorque(ruVector3 torque) override;
	virtual ruVector3 GetAABBMin() override;
	virtual ruVector3 GetAABBMax() override;
	virtual int GetTextureCount() override;
	virtual shared_ptr<ruTexture> GetTexture(int n) override;
	virtual int GetTotalAnimationFrameCount() override;
	virtual void Freeze() override;
	virtual bool IsStatic() override;
	virtual void Unfreeze() override;
	virtual ruVector3 GetTotalForce() override;
	virtual BodyType GetBodyType() const override;
	virtual void Hide() override;
	virtual void Show() override;
	virtual bool IsDynamic() override;
	virtual void SetName(const string & name) override;
	virtual string GetProperty(string propName) override;
	virtual void SetLinearFactor(ruVector3 lin) override;
	virtual ruVector3 GetPosition() const override;
	virtual int GetContactCount() override;
	virtual ruContact GetContact(int num) override;
	virtual bool IsInsideNode(shared_ptr<ruSceneNode> n) override;
	virtual shared_ptr<ruSceneNode> GetChild(int i) override;
	virtual int GetCountChildren() override;
	virtual void SetFriction(float friction) override;
	virtual void SetDepthHack(float depthHack) override;
	virtual void SetAnisotropicFriction(ruVector3 aniso) override;
	virtual void Move(ruVector3 speed) override;
	virtual void SetVelocity(ruVector3 velocity) override;
	virtual void SetAngularVelocity(ruVector3 velocity) override;
	virtual ruVector3 GetEulerAngles() override;
	virtual ruQuaternion GetLocalRotation() override;
	virtual void SetPosition(ruVector3 position) override;
	virtual void SetDamping(float linearDamping, float angularDamping) override;
	virtual void SetGravity(const ruVector3 & gravity) override;
	virtual void SetMass(float mass) override;
	virtual float GetMass() override;
	virtual bool IsFrozen() override;
	virtual void SetRotation(ruQuaternion rotation) override;
	virtual ruVector3 GetLookVector() const override;
	virtual ruVector3 GetAbsoluteLookVector() override;
	virtual const string GetName() override;
	virtual ruVector3 GetRightVector() const override;
	virtual ruVector3 GetUpVector() const override;
	virtual bool IsVisible() override;
	virtual bool IsInFrustum() override;
	virtual void SetAlbedo(float albedo) override;
	virtual bool IsSkinned() const override;
	virtual string GetTag() const override;
	virtual void SetTag(const string & tag) override;
	virtual float GetDepthHack() const override;
	virtual float GetAlbedo() const override;
	virtual bool IsBone() const override;
	virtual ruVector3 GetLocalPosition() override;
	virtual ruVector3 GetLinearVelocity() override;
	virtual void SetLocalScale(ruVector3 scale) override;
	virtual void SetLocalPosition(ruVector3 pos) override;
	virtual void SetLocalRotation(ruQuaternion rot) override;
	virtual shared_ptr<ruSceneNode> GetParent() override;
	virtual void SetBlurAmount(float blurAmount) override;
	virtual float GetBlurAmount() override;
	virtual int GetMeshCount() const override;
	virtual void SetCollisionEnabled(bool state) override;
	virtual bool IsCollisionEnabled() const override;
	virtual shared_ptr<ruSceneNode> FindChild(const string & name) override;
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

