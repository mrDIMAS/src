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
#include "Precompiled.h"
#include "Physics.h"
#include "ParticleSystem.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Texture.h"
#include "Vertex.h"
#include "Renderer.h"
#include "SceneFactory.h"
#include "DirectionalLight.h"
#include "Engine.h"

void SceneNode::AutoName() {
	static unsigned int uniqueNum = 0;
	mName = StringBuilder("Unnamed") << uniqueNum++;
}

SceneNode::SceneNode(SceneFactory * factory) :
	mFactory(factory),
	mStatic(false),
	mInFrustum(false),
	mTotalFrameCount(0),
	mIsSkinned(false),
	mVisible(true),
	mContactCount(0),
	mTexCoordFlow(0.0f, 0.0f),
	mFrozen(false),
	mIsBone(false),
	mDepthHack(0.0f),
	mCollisionEnabled(true),
	mAlbedo(0.0f),
	mCurrentAnimation(nullptr),
	mLastAnimation(nullptr),
	mTransitionAnimation(nullptr),
	mBlurAmount(0.0f),
	mTwoSidedLighting(false),
	mIsMoving(false),
	mShadowCastEnabled(true),
	mVegetation(false),
	mAnimationOverride(false),
	mTransitionFrames(10),
	mCurrentTransitionFrame(0),
	mAnimationBlendingEnabled(true) {
	AutoName();
	mLocalTransform = btTransform(btQuaternion(0, 0, 0), btVector3(0, 0, 0));
	mGlobalTransform = mLocalTransform;
}

bool SceneNode::IsMoving() const {
	return mIsMoving;
}

shared_ptr<SceneNode> SceneNode::FindChildInNode(shared_ptr<SceneNode> node, const string & name) {
	if(node->GetName() == name) {
		return node;
	}
	for(auto & child : node->mChildren) {
		shared_ptr<SceneNode> & lookup = FindChildInNode(child, name);
		if(lookup) {
			return lookup;
		}
	}
	return nullptr;
}

shared_ptr<SceneNode> SceneNode::FindChildInNodeNonRecursive(const shared_ptr<SceneNode> parent, string childName) {
	for(auto & child : parent->mChildren) {
		if(child->mName == childName) {
			return child;
		}
	}
	return nullptr;
}

SceneNode::~SceneNode() {
	for(auto & trimesh : mTrimeshList) {
		delete trimesh;
	}
	for(auto & body : mBodyList) {
		if(body->getCollisionShape()) {
			delete body->getCollisionShape();
		}
		if(body->getMotionState()) {
			delete body->getMotionState();
		}
		mFactory->GetEngine()->GetPhysics()->mpDynamicsWorld->removeRigidBody(body);
		delete body;
	}
}

SceneFactory * const SceneNode::GetFactory() const {
	return mFactory;
}

KeyFrame SceneNode::GetKeyFrame(int n) const {
	KeyFrame kf;
	if(n >= 0 && n < mKeyframeList.size()) {
		auto & bkf = mKeyframeList[n];
		kf.mPosition = Vector3(bkf->getOrigin().x(), bkf->getOrigin().y(), bkf->getOrigin().z());
		kf.mRotation = Quaternion(bkf->getRotation().x(), bkf->getRotation().y(), bkf->getRotation().z(), bkf->getRotation().w());
	} else {
		throw runtime_error("Invalid key frame number!");
	}
	return kf;
}

void SceneNode::SetConvexBody() {
	if(mMeshList.size() == 0) {
		return;
	}

	btConvexHullShape * convex = new btConvexHullShape();

	int vertexCount = 0;
	for(auto mesh : mMeshList) {
		for(auto & vertex : mesh->GetVertices()) {
			convex->addPoint(btVector3(vertex.mPosition.x, vertex.mPosition.y, vertex.mPosition.z));
			++vertexCount;
		}
	}

	if(vertexCount > 0) {
		btVector3 inertia(0.0f, 0.0f, 0.0f);
		convex->calculateLocalInertia(1, inertia);
		SetBody(new btRigidBody(1, static_cast<btMotionState*>(new btDefaultMotionState()), static_cast<btCollisionShape*>(convex), inertia));
	} else {
		delete convex;
	}
}

void SceneNode::SetCapsuleBody(float height, float radius) {
	btCollisionShape * shape = new btCapsuleShape(radius, height);
	btVector3 inertia;
	shape->calculateLocalInertia(1, inertia);
	SetBody(new btRigidBody(1, static_cast<btMotionState*>(new btDefaultMotionState()), shape, inertia));
}

void SceneNode::SetBoxBody() {
	Vector3 halfExtents = (GetAABBMax() - GetAABBMin()) / 2.0f;
	btCollisionShape * shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
	btVector3 inertia;
	shape->calculateLocalInertia(1, inertia);
	SetBody(new btRigidBody(1, static_cast<btMotionState*>(new btDefaultMotionState()), shape, inertia));
}

void SceneNode::SetCylinderBody() {
	Vector3 halfExtents = (GetAABBMax() - GetAABBMin()) / 2.0f;
	btCollisionShape * shape = new btCylinderShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
	btVector3 inertia;
	shape->calculateLocalInertia(1, inertia);
	SetBody(new btRigidBody(1, static_cast<btMotionState*>(new btDefaultMotionState()), shape, inertia));
}

void SceneNode::SetSphereBody() {
	float radius = (GetAABBMax() - GetAABBMin()).Length() / 2.0f;
	btCollisionShape * shape = new btSphereShape(radius);
	btVector3 inertia;
	shape->calculateLocalInertia(1, inertia);
	SetBody(new btRigidBody(1, static_cast<btMotionState*>(new btDefaultMotionState()), shape, inertia));
}

void SceneNode::SetAngularFactor(Vector3 fact) {
	for(auto & body : mBodyList) {
		body->setAngularFactor(btVector3(fact.x, fact.y, fact.z));
	}
}

void SceneNode::SetTrimeshBody() {
	if(mMeshList.size()) {
		int meshNum = 0;
		mStatic = true;
		for(auto mesh : mMeshList) {
			if(mesh->GetTriangles().size()) {
				btTriangleMesh * trimesh = new btTriangleMesh();
				for(auto triangle : mesh->GetTriangles()) {
					Vector3 & a = mesh->GetVertices()[triangle.mA].mPosition;
					Vector3 & b = mesh->GetVertices()[triangle.mB].mPosition;
					Vector3 & c = mesh->GetVertices()[triangle.mC].mPosition;
					trimesh->addTriangle(btVector3(a.x, a.y, a.z), btVector3(b.x, b.y, b.z), btVector3(c.x, c.y, c.z), false);
				};
				btMotionState * motionState = new btDefaultMotionState();
				btCollisionShape * shape = new btBvhTriangleMeshShape(trimesh, true, true);
				btRigidBody * body = new btRigidBody(0.0f, motionState, shape);
				body->setWorldTransform(mGlobalTransform);
				body->setFriction(1.0f);
				body->setUserPointer(this);
				body->setUserIndex(meshNum);
				body->setRestitution(0.0f);
				body->setDeactivationTime(0.1f);
				body->setSleepingThresholds(1.0f, 1.0f);
				body->getCollisionShape()->setMargin(0.02);
				mBodyList.push_back(body);
				mFactory->GetEngine()->GetPhysics()->mpDynamicsWorld->addRigidBody(body);
				mTrimeshList.push_back(trimesh);
			}
			meshNum++;
		}
	}
}

void SceneNode::Attach(const shared_ptr<ISceneNode> & parent) {
	shared_ptr<SceneNode> & pParent = std::dynamic_pointer_cast<SceneNode>(parent);

	mParent = pParent;
	pParent->mChildren.push_back(shared_from_this());

	// for correct attaching, we must recalculate local transform of this node
	pParent->CalculateGlobalTransform();
	CalculateGlobalTransform();
	// local transform becomes transform relative to parent
	mLocalTransform = pParent->mGlobalTransform.inverse() * mGlobalTransform;
}

void SceneNode::Detach() {
	mParent.reset();
}

btTransform & SceneNode::CalculateGlobalTransform() {
	if(mBodyList.size()) {
		if(mParent.use_count()) {
			shared_ptr<SceneNode> & pParent = mParent.lock();
			if(mFrozen) { // only frozen bodies can be parented
				mGlobalTransform = pParent->CalculateGlobalTransform() * mLocalTransform;
				mBodyList[0]->setWorldTransform(mGlobalTransform);
				mBodyList[0]->setLinearVelocity(btVector3(0, 0, 0));
				mBodyList[0]->setAngularVelocity(btVector3(0, 0, 0));
			} else {
				mGlobalTransform = mBodyList[0]->getWorldTransform();
			}
		} else { // no parent
			mGlobalTransform = mBodyList[0]->getWorldTransform();
		}
	} else { // no body
		if(mParent.use_count()) {
			shared_ptr<SceneNode> & pParent = mParent.lock();
			mGlobalTransform = pParent->CalculateGlobalTransform() * mLocalTransform;
		} else {
			mGlobalTransform = mLocalTransform;
		}
	}
	if((mLastPosition - GetPosition()).Length2() < 0.0001) {
		mIsMoving = false;
	} else {
		mIsMoving = true;
	}

	mLastPosition = GetPosition();

	return mGlobalTransform;
}

Vector3 SceneNode::GetAABBMin() {
	Vector3 min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

	for(auto mesh : mMeshList) {
		AABB aabb = mesh->GetBoundingBox();
		if(aabb.mMin.x < min.x) {
			min.x = aabb.mMin.x;
		}
		if(aabb.mMin.y < min.y) {
			min.y = aabb.mMin.y;
		}
		if(aabb.mMin.z < min.z) {
			min.z = aabb.mMin.z;
		}
	}
	return min;
}

Vector3 SceneNode::GetAABBMax() {
	Vector3 max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(auto mesh : mMeshList) {
		AABB aabb = mesh->GetBoundingBox();

		if(aabb.mMax.x > max.x) {
			max.x = aabb.mMax.x;
		}
		if(aabb.mMax.y > max.y) {
			max.y = aabb.mMax.y;
		}
		if(aabb.mMax.z > max.z) {
			max.z = aabb.mMax.z;
		}
	}
	return max;
}




void SceneNode::PerformAnimation() {
	if(!mIsSkinned) {
		if(mCurrentAnimation) {
			if(mKeyframeList.size() && !mAnimationOverride) {
				if(mTransitionAnimation && mAnimationBlendingEnabled) {
					// do transition					
					float t = (float)mCurrentTransitionFrame / (float)mTransitionFrames;

					auto lastRotation = mKeyframeList[mTransitionAnimation->GetCurrentFrame()]->getRotation().slerp(mKeyframeList[mTransitionAnimation->GetNextFrame()]->getRotation(), mTransitionAnimation->GetInterpolator());
					auto lastPosition = mKeyframeList[mTransitionAnimation->GetCurrentFrame()]->getOrigin().lerp(mKeyframeList[mTransitionAnimation->GetNextFrame()]->getOrigin(), mTransitionAnimation->GetInterpolator());

					auto currRotation = mKeyframeList[mCurrentAnimation->GetCurrentFrame()]->getRotation().slerp(mKeyframeList[mCurrentAnimation->GetNextFrame()]->getRotation(), mCurrentAnimation->GetInterpolator());
					auto currPosition = mKeyframeList[mCurrentAnimation->GetCurrentFrame()]->getOrigin().lerp(mKeyframeList[mCurrentAnimation->GetNextFrame()]->getOrigin(), mCurrentAnimation->GetInterpolator());

					mLocalTransform.setRotation(lastRotation.slerp(currRotation, t));
					mLocalTransform.setOrigin(lastPosition.lerp(currPosition, t));

					if(mCurrentTransitionFrame == mTransitionFrames) {
						mTransitionAnimation = nullptr;
					}
					++mCurrentTransitionFrame;
				} else {
					mLocalTransform.setRotation(mKeyframeList[mCurrentAnimation->GetCurrentFrame()]->getRotation().slerp(mKeyframeList[mCurrentAnimation->GetNextFrame()]->getRotation(), mCurrentAnimation->GetInterpolator()));
					mLocalTransform.setOrigin(mKeyframeList[mCurrentAnimation->GetCurrentFrame()]->getOrigin().lerp(mKeyframeList[mCurrentAnimation->GetNextFrame()]->getOrigin(), mCurrentAnimation->GetInterpolator()));
				}
			}
		}
	};
}

void SceneNode::Freeze() {
	mFrozen = true;
	for(auto & body : mBodyList) {
		body->setAngularFactor(0);
		body->setLinearFactor(btVector3(0, 0, 0));
		body->setAngularVelocity(btVector3(0, 0, 0));
		body->setLinearVelocity(btVector3(0, 0, 0));
		body->setGravity(btVector3(0, 0, 0));
	}
}

void SceneNode::Unfreeze() {
	mFrozen = false;
	for(auto & body : mBodyList) {
		body->activate(true);
		body->setAngularFactor(1);
		body->setLinearFactor(btVector3(1, 1, 1));
		body->setGravity(mFactory->GetEngine()->GetPhysics()->mpDynamicsWorld->getGravity());
	}
}

bool SceneNode::IsVisible() {
	bool trulyVisible = mVisible;

	if(mParent.use_count()) {
		shared_ptr<SceneNode> & parent = mParent.lock();
		trulyVisible &= parent->IsVisible();
	}

	return trulyVisible;
}

void SceneNode::Hide() {
	mVisible = false;
}

void SceneNode::Show() {
	mVisible = true;
}

std::string SceneNode::GetProperty(string propName) {
	auto propIter = mProperties.find(propName);

	if(propIter != mProperties.end()) {
		return propIter->second;
	}

	return string("null");
}

void SceneNode::UpdateSounds() {
	btVector3 pos = mGlobalTransform.getOrigin();

	for(auto & sound : mSoundList) {
		sound->SetPosition(Vector3(pos.x(), pos.y(), pos.z()));
	}
	if(mIdleSound) {
		mIdleSound->Play();
	}
}



void SceneNode::SetLinearFactor(Vector3 lin) {
	for(auto & body : mBodyList) {
		body->setLinearFactor(btVector3(lin.x, lin.y, lin.z));
	}
}

Vector3 SceneNode::GetPosition() const {
	btVector3 pos = mGlobalTransform.getOrigin();
	return Vector3(pos.x(), pos.y(), pos.z());
}

int SceneNode::GetContactCount() {
	return mContactCount;
}

Contact SceneNode::GetContact(int num) {
	return mContactList[num];
}

void SceneNode::ApplyProperties() {
	for(auto prop : mProperties) {
		string pname = prop.first;
		string value = prop.second;

		if(pname == "body") {
			if(value == "static") {
				SetTrimeshBody();
			}
			if(value == "convex") {
				SetConvexBody();
			}
			if(value == "box") {
				SetBoxBody();
			}
			if(value == "sphere") {
				SetSphereBody();
			}
			if(value == "cylinder") {
				SetCylinderBody();
			}
		};

		if(pname == "albedo") {
			mAlbedo = atof(value.c_str());
			if(mAlbedo > 0.9) {
				SetShadowCastEnabled(false); // HAAAX 
			}
		}

		if(pname == "vegetation") {
			SetVegetation(atoi(value.c_str()));
		}

		if(pname == "visible") {
			mVisible = atoi(value.c_str());
		}

		if(pname == "tag") {
			SetTag(value);
		}

		if(pname == "drawFlare") {
			auto light = dynamic_pointer_cast<Light>(shared_from_this());
			if(light) {
				light->SetDrawFlare(atoi(value.c_str()));
			}
		}

		if(pname == "shadowCast") {
			SetShadowCastEnabled(atoi(value.c_str()));
		}

		if(pname == "frozen") {
			int frozen = atoi(value.c_str());

			if(frozen) {
				Freeze();
			}
		}

		if(pname == "mass") {
			if(!IsStatic()) {
				SetMass(atof(value.c_str()));
			}
		}

		if(pname == "twoSidedLighting") {
			mTwoSidedLighting = atoi(value.c_str());
		}

		if(pname == "friction") {
			for(auto & body : mBodyList) {
				body->setFriction(atof(value.c_str()));
			}
		}

		if(pname == "hitSound") {
			mHitSound = mFactory->GetEngine()->GetSoundSystem()->LoadSound3D(value.c_str());
			AttachSound(mHitSound);
		};

		if(pname == "idleSound") {
			mIdleSound = mFactory->GetEngine()->GetSoundSystem()->LoadSound3D(value.c_str());
			AttachSound(mIdleSound);
		};
	};
}


void SceneNode::AttachSound(const shared_ptr<ISound> & sound) {
	mSoundList.push_back(sound);
}

bool SceneNode::IsInsideNode(shared_ptr<ISceneNode> n) {
	shared_ptr<SceneNode> & node = std::dynamic_pointer_cast<SceneNode>(n);

	if(!node) {
		return 0;
	}

	btVector3 pos(mGlobalTransform.getOrigin());

	Vector3 point(pos.x(), pos.y(), pos.z());

	int result = 0;

	btVector3 n2Pos = node->mGlobalTransform.getOrigin();
	for(auto mesh : node->mMeshList) {
		AABB aabb = mesh->GetBoundingBox();

		aabb.mMax.x += n2Pos.x();
		aabb.mMax.y += n2Pos.y();
		aabb.mMax.z += n2Pos.z();

		aabb.mMin.x += n2Pos.x();
		aabb.mMin.y += n2Pos.y();
		aabb.mMin.z += n2Pos.z();

		if(point.x > aabb.mMin.x && point.x < aabb.mMax.x &&
			point.y > aabb.mMin.y && point.y < aabb.mMax.y &&
			point.z > aabb.mMin.z && point.z < aabb.mMax.z) {
			result++;
		}
	}

	for(auto & child : node->mChildren) {
		result += child->IsInsideNode(shared_from_this());
	}

	return result;
}

shared_ptr<ISceneNode> SceneNode::GetChild(int i) {
	return mChildren[i];
}

int SceneNode::GetCountChildren() {
	return mChildren.size();
}


void SceneNode::SetFriction(float friction) {
	for(auto & body : mBodyList) {
		body->setFriction(friction);
	}
}

void SceneNode::SetDepthHack(float depthHack) {
	mDepthHack = depthHack;
	for(auto & node : mChildren) {
		if(node) {
			node->SetDepthHack(depthHack);
		}
	}
}

void SceneNode::SetAnisotropicFriction(Vector3 aniso) {
	for(auto & body : mBodyList) {
		body->setAnisotropicFriction(btVector3(aniso.x, aniso.y, aniso.z));
	}
}

void SceneNode::Move(Vector3 speed) {
	for(auto & body : mBodyList) {
		body->activate(true);
		body->setLinearVelocity(btVector3(speed.x, speed.y, speed.z));
	};
	mLocalTransform.setOrigin(mLocalTransform.getOrigin() + btVector3(speed.x, speed.y, speed.z));
}

void SceneNode::SetVelocity(Vector3 velocity) {
	for(auto & body : mBodyList) {
		body->activate(true);
		body->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	}
}

void SceneNode::SetPosition(Vector3 position) {
	mLocalTransform.setOrigin(btVector3(position.x, position.y, position.z));
	for(auto & body : mBodyList) {
		body->activate(true);
		body->getWorldTransform().setOrigin(btVector3(position.x, position.y, position.z));
	};
	CalculateGlobalTransform();
}

float SceneNode::GetMass() {
	if(mBodyList.size()) {
		return 1.0f / mBodyList[0]->getInvMass();
	}

	return 0.0f;
}

bool SceneNode::IsFrozen() {
	return mFrozen;
}

void SceneNode::SetRotation(Quaternion rotation) {
	for(auto & body : mBodyList) {
		body->activate(true);
		body->getWorldTransform().getBasis().setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	}
	mLocalTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
}

void SceneNode::SetLocalScale(Vector3 scale) {
	for(auto & body : mBodyList) {
		body->getCollisionShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
	}
}

Vector3 SceneNode::GetLookVector() const {
	btVector3 look = mLocalTransform.getBasis().getColumn(2);
	return Vector3(look.x(), look.y(), look.z());
}

Vector3 SceneNode::GetAbsoluteLookVector() {
	btVector3 look = mGlobalTransform.getBasis().getColumn(2);
	return Vector3(look.x(), look.y(), look.z());
}

const string SceneNode::GetName() {
	return mName;
}

Vector3 SceneNode::GetRightVector() const {
	btVector3 right = mLocalTransform.getBasis().getColumn(0);
	return Vector3(right.x(), right.y(), right.z());
}

Vector3 SceneNode::GetUpVector() const {
	btVector3 up = mLocalTransform.getBasis().getColumn(1);
	return Vector3(up.x(), up.y(), up.z());
}

btTransform SceneNode::GetGlobalTransform() const {
	return mGlobalTransform;
}

Vector3 SceneNode::GetLocalPosition() {
	btTransform transform = mLocalTransform;

	if(mBodyList.size() && !mFrozen) {
		transform = mBodyList[0]->getWorldTransform();
	}

	Vector3 lp;

	lp.x = transform.getOrigin().x();
	lp.y = transform.getOrigin().y();
	lp.z = transform.getOrigin().z();

	return lp;
}


void SceneNode::SetShadowCastEnabled(bool state) {
	mShadowCastEnabled = state;
}

bool SceneNode::IsShadowCastEnabled() const {
	return mShadowCastEnabled;
}

void SceneNode::SetVegetation(bool state) {
	mVegetation = state;
}

bool SceneNode::IsVegetation() const {
	return mVegetation;
}

void SceneNode::SetAnimationOverride(bool state) {
	mAnimationOverride = state;
}

bool SceneNode::IsAnimationOverride() const {
	return mAnimationOverride;
}

void SceneNode::SetAnimationBlendingEnabled(bool state) {
	mAnimationBlendingEnabled = state;

	for(auto & child : mChildren) {
		child->SetAnimationBlendingEnabled(state);
	}
}

bool SceneNode::IsAnimationBlendingEnabled(bool state) const {
	return mAnimationBlendingEnabled;
}

void SceneNode::SetOpacity(float opacity) {
	for(auto & mesh : mMeshList) {
		mesh->SetOpacity(opacity);
	}
}

shared_ptr<ISceneNode> SceneNode::FindChild(const string & name) {
	return FindChildInNode(shared_from_this(), name);
}

void SceneNode::SetAngularVelocity(Vector3 velocity) {
	for(auto & body : mBodyList) {
		body->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	}
}

Vector3 SceneNode::GetEulerAngles() {
	float y, p, r;

	mGlobalTransform.getBasis().getEulerYPR(y, p, r);

	y *= 180.0 / 3.14159f;
	p *= 180.0 / 3.14159f;
	r *= 180.0 / 3.14159f;

	return Vector3(p, y, r);
}

Quaternion SceneNode::GetLocalRotation() {
	btTransform transform = mLocalTransform;
	if(mBodyList.size()) {
		transform = mBodyList[0]->getWorldTransform();
	}
	return Quaternion(transform.getRotation().x(), transform.getRotation().y(), transform.getRotation().z(), transform.getRotation().w());
}

void SceneNode::SetDamping(float linearDamping, float angularDamping) {
	for(auto & body : mBodyList) {
		body->setDamping(linearDamping, angularDamping);
	}
}

void SceneNode::SetGravity(const Vector3 & gravity) {
	btVector3 g(gravity.x, gravity.y, gravity.z);

	for(auto & body : mBodyList) {
		body->setGravity(g);
	}
}

void SceneNode::SetMass(float mass) {
	for(auto & body : mBodyList) {
		btVector3 inertia;
		body->getCollisionShape()->calculateLocalInertia(mass, inertia);
		body->setMassProps(mass, inertia);
	}
}

void SceneNode::SetBody(btRigidBody * theBody) {
	theBody->setWorldTransform(mGlobalTransform);
	theBody->setFriction(1.0f);
	theBody->setUserPointer(this);
	theBody->setUserIndex(mMeshList.size() > 0 ? 0 : -1);
	theBody->setRestitution(0.0f);
	theBody->setDeactivationTime(0.1f);
	theBody->setCcdMotionThreshold(0.75f);
	theBody->setCcdSweptSphereRadius(0.2f);
	theBody->setSleepingThresholds(1.0f, 1.0f);
	theBody->getCollisionShape()->setMargin(0.02);
	mFactory->GetEngine()->GetPhysics()->mpDynamicsWorld->addRigidBody(theBody);
	mBodyList.push_back(theBody);
}

void SceneNode::SetAnimation(Animation * newAnim, bool dontAffectChilds) {
	if(newAnim) {
		/*
		if (newAnim->GetBeginFrame() < 0 || newAnim->GetBeginFrame() >= mKeyframeList.size()) {
			Log::Write(StringBuilder("WARNING! Unable to set animation. Begin frame index is out of range! Node: ") << mName << ", index: " << newAnim->GetBeginFrame() << ", range is [0;" << mKeyframeList.size() << "]");
			return;
		}
		if (newAnim->GetEndFrame() < 0 || newAnim->GetEndFrame() >= mKeyframeList.size()) {
			Log::Write(StringBuilder("WARNING! Unable to set animation. End frame index is out of range! Node: ") << mName << ", index: " << newAnim->GetEndFrame() << ", range is [0;" << mKeyframeList.size() << "]");
			return;
		}*/

		if(newAnim->GetBeginFrame() >= 0 && newAnim->GetBeginFrame() < mKeyframeList.size() &&
			newAnim->GetEndFrame() >= 0 && newAnim->GetEndFrame() < mKeyframeList.size()) {
			if(mCurrentAnimation != mLastAnimation) {
				mTransitionAnimation = mLastAnimation;
				mCurrentTransitionFrame = 0;
				mLastAnimation = newAnim;
			}
			mCurrentAnimation = newAnim;
		}
	}



	if(!dontAffectChilds) {
		for(auto & child : mChildren) {
			child->SetAnimation(newAnim, false);
		}
	}
}

BodyType SceneNode::GetBodyType() const {
	BodyType bodyType = BodyType::None;
	if(mBodyList.size()) {
		btCollisionShape * shape = mBodyList[0]->getCollisionShape();
		if(dynamic_cast<btSphereShape*>(shape)) {
			bodyType = BodyType::Sphere;
		}
		if(dynamic_cast<btBvhTriangleMeshShape*>(shape)) {
			bodyType = BodyType::Trimesh;
		}
		if(dynamic_cast<btCylinderShape*>(shape)) {
			bodyType = BodyType::Cylinder;
		}
		if(dynamic_cast<btConvexHullShape*>(shape)) {
			bodyType = BodyType::Convex;
		}
		if(dynamic_cast<btBoxShape*>(shape)) {
			bodyType = BodyType::Box;
		}
		if(dynamic_cast<btCapsuleShape*>(shape)) {
			bodyType = BodyType::Capsule;
		}
	}
	return bodyType;
}

Vector3 SceneNode::GetTotalForce() {
	if(mBodyList.size()) {
		return Vector3(mBodyList[0]->getTotalForce().x(), mBodyList[0]->getTotalForce().y(), mBodyList[0]->getTotalForce().z());
	} else {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
}

Animation * SceneNode::GetCurrentAnimation() {
	return mCurrentAnimation;
}

Vector3 SceneNode::GetLinearVelocity() {
	Vector3 vel;
	if(mBodyList.size()) {
		vel.x = mBodyList[0]->getLinearVelocity().x();
		vel.y = mBodyList[0]->getLinearVelocity().y();
		vel.z = mBodyList[0]->getLinearVelocity().z();
	}
	return vel;
}

float SceneNode::GetRotationAngle() {
	return mLocalTransform.getRotation().getAngle() * 180.0f / 3.14159;
}

Vector3 SceneNode::GetRotationAxis() {
	btVector3 axis = mLocalTransform.getRotation().getAxis();
	return Vector3(axis.x(), axis.y(), axis.z());
}

void SceneNode::AddTorque(Vector3 torque) {
	for(auto & body : mBodyList) {
		body->applyTorque(btVector3(torque.x, torque.y, torque.z));
	}
}

void SceneNode::AddForceAtPoint(Vector3 force, Vector3 point) {
	for(auto & body : mBodyList) {
		body->applyForce(btVector3(force.x, force.y, force.z), btVector3(point.x, point.y, point.z));
	}
}

void SceneNode::AddForce(Vector3 force) {
	for(auto & body : mBodyList) {
		body->applyCentralForce(btVector3(force.x, force.y, force.z));
	}
}

void SceneNode::SetLocalPosition(Vector3 pos) {
	mLocalTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	for(auto & body : mBodyList) {
		body->getWorldTransform().setOrigin(btVector3(pos.x, pos.y, pos.z));
	}
	CalculateGlobalTransform();
}

void  SceneNode::SetLocalRotation(Quaternion rot) {
	mLocalTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	for(auto & body : mBodyList) {
		body->getWorldTransform().setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	}
	CalculateGlobalTransform();
}

shared_ptr<ISceneNode> SceneNode::GetParent() {
	return mParent.lock();
}

bool SceneNode::IsStatic() {
	return mStatic;
}

bool SceneNode::IsDynamic() {
	if(mBodyList.size()) {
		if(dynamic_cast<btBvhTriangleMeshShape*>(mBodyList[0]->getCollisionShape()) == nullptr) {
			return true;
		}
	}
	return false;
}

shared_ptr<ITexture> SceneNode::GetTexture(int n) {
	if(n < 0 || n >= mMeshList.size()) {
		return nullptr;
	} else {
		return mMeshList[n]->mDiffuseTexture;
	}
}

int SceneNode::GetTextureCount() {
	return mMeshList.size();
}

float SceneNode::GetBlurAmount() {
	return mBlurAmount;
}

void SceneNode::SetBlurAmount(float blurAmount) {
	mBlurAmount = blurAmount;
	for(auto child : mChildren) {
		child->SetBlurAmount(blurAmount);
	}
}

void SceneNode::SetAlbedo(float albedo) {
	mAlbedo = albedo;
}

bool SceneNode::IsInFrustum() {
	return mInFrustum;
}

void SceneNode::SetName(const string & name) {
	mName = name;
}

int SceneNode::GetTotalAnimationFrameCount() {
	return mTotalFrameCount;
}

void SceneNode::AddMesh(const shared_ptr<Mesh> & mesh) {
	mMeshList.push_back(mesh);
}

void SceneNode::CheckFrustum(Camera * pCamera) {
	mInFrustum = false;
	for(auto & mesh : mMeshList) {
		mInFrustum |= pCamera->mFrustum.IsAABBInside(mesh->GetBoundingBox(), GetPosition(), GetWorldMatrix());
	}
}

btTransform SceneNode::GetRelativeTransform() {
	return mGlobalTransform * mInverseBindTransform;
}

btTransform SceneNode::GetInverseBindTransform() {
	return mInverseBindTransform;
}

btTransform SceneNode::GetLocalTransform() {
	return mLocalTransform;
}

int SceneNode::GetMeshCount() const {
	return mMeshList.size();
}

void SceneNode::MakeBone() {
	mIsBone = true;
}

D3DXMATRIX SceneNode::GetWorldMatrix() {
	D3DXMATRIX outMatrix;
	btVector3 R = mGlobalTransform.getBasis().getColumn(0);
	btVector3 U = mGlobalTransform.getBasis().getColumn(1);
	btVector3 L = mGlobalTransform.getBasis().getColumn(2);
	btVector3 P = mGlobalTransform.getOrigin();
	outMatrix._11 = R.x();
	outMatrix._12 = R.y();
	outMatrix._13 = R.z();
	outMatrix._14 = 0.0f;
	outMatrix._21 = U.x();
	outMatrix._22 = U.y();
	outMatrix._23 = U.z();
	outMatrix._24 = 0.0f;
	outMatrix._31 = L.x();
	outMatrix._32 = L.y();
	outMatrix._33 = L.z();
	outMatrix._34 = 0.0f;
	outMatrix._41 = P.x();
	outMatrix._42 = P.y();
	outMatrix._43 = P.z();
	outMatrix._44 = 1.0f;
	return outMatrix;
}

bool SceneNode::IsBone() const {
	return mIsBone;
}

float SceneNode::GetAlbedo() const {
	return mAlbedo;
}

float SceneNode::GetDepthHack() const {
	return mDepthHack;
}

bool SceneNode::IsSkinned() const {
	return mIsSkinned;
}

string SceneNode::GetTag() const {
	return mTag;
}

void SceneNode::SetTag(const string & tag) {
	mTag = tag;
}

shared_ptr<Mesh> SceneNode::GetMesh(int n) {
	try {
		return mMeshList.at(n);
	} catch(std::out_of_range) {
		return nullptr;
	}
}

btRigidBody * SceneNode::GetDynamicBody() {
	if(mBodyList.size()) {
		return mBodyList[0];
	}
	return nullptr;

}

bool SceneNode::IsCollisionEnabled() const {
	return mCollisionEnabled;
}

void SceneNode::SetCollisionEnabled(bool state) {
	mCollisionEnabled = state;
}

Vector2 SceneNode::GetTexCoordFlow() const {
	return mTexCoordFlow;
}

void SceneNode::SetTexCoordFlow(const Vector2 & flow) {
	mTexCoordFlow = flow;
}



ISceneNode::~ISceneNode() {

}