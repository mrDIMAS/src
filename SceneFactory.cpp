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
#include "SceneFactory.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include "DirectionalLight.h"
#include "Fog.h"

vector<weak_ptr<SceneNode>> SceneFactory::msNodeList;
vector<weak_ptr<SpotLight>> SceneFactory::msSpotLightList;
vector<weak_ptr<PointLight>> SceneFactory::msPointLightList;
vector<weak_ptr<DirectionalLight>> SceneFactory::msDirectionalLightList;
vector<weak_ptr<ParticleSystem>> SceneFactory::msParticleEmitters;
vector<weak_ptr<Fog>> SceneFactory::msFogList;

shared_ptr<SceneNode> SceneFactory::CreateSceneNode() {
	auto sceneNode = make_shared<SceneNode>();
	msNodeList.push_back(sceneNode);
	return sceneNode;
}

shared_ptr<SceneNode> SceneFactory::CreateSceneNodeDuplicate(shared_ptr<SceneNode> source) {
	shared_ptr<SceneNode> & duplicate = CreateSceneNode();
	duplicate->mInFrustum = source->mInFrustum;
	duplicate->mTotalFrameCount = 0;
	duplicate->mIsSkinned = false;
	duplicate->mVisible = true;
	duplicate->mLocalTransform = source->mLocalTransform;
	duplicate->mGlobalTransform = duplicate->mLocalTransform;
	duplicate->mContactCount = source->mContactCount;
	duplicate->mFrozen = source->mFrozen;
	duplicate->mDepthHack = source->mDepthHack;
	duplicate->mAlbedo = source->mAlbedo;
	duplicate->mCurrentAnimation = nullptr;

	// copy surfaces
	for (auto pMesh : source->mMeshList) {
		pMesh->LinkTo(duplicate);
		duplicate->mMeshList.push_back(pMesh);
	}

	// create body
	switch (source->GetBodyType()) {
	case BodyType::Box:
		duplicate->SetBoxBody();
		break;
	case BodyType::Convex:
		duplicate->SetConvexBody();
		break;
	case BodyType::Sphere:
		duplicate->SetSphereBody();
		break;
	case BodyType::Trimesh:
		duplicate->SetTrimeshBody();
		break;
	}

	if (duplicate->mFrozen) {
		duplicate->Freeze();
	}

	// copy childs
	for (auto & pChild : source->mChildren) {
		shared_ptr<SceneNode> & pNewChild = SceneFactory::CreateSceneNodeDuplicate(pChild);
		pNewChild->mParent = duplicate;
		duplicate->mChildren.push_back(pNewChild);
	}
	return duplicate;
}

shared_ptr<PointLight> SceneFactory::CreatePointLight() {
	auto pointLight = make_shared<PointLight>();
	msNodeList.push_back(pointLight);
	msPointLightList.push_back(pointLight);
	return pointLight;
}

shared_ptr<SpotLight> SceneFactory::CreateSpotLight() {
	auto spotLight = make_shared<SpotLight>();
	msNodeList.push_back(spotLight);
	msSpotLightList.push_back(spotLight);
	return spotLight;
}

shared_ptr<DirectionalLight> SceneFactory::CreateDirectionalLight() {
	auto dirLight = make_shared<DirectionalLight>();
	msNodeList.push_back(dirLight);
	msDirectionalLightList.push_back(dirLight);
	return dirLight;
}

vector<weak_ptr<PointLight>> & SceneFactory::GetPointLightList() {
	RemoveUnreferenced(msPointLightList);
	return msPointLightList;
}

vector<weak_ptr<DirectionalLight>>& SceneFactory::GetDirectionalLightList() {
	RemoveUnreferenced(msDirectionalLightList);
	return msDirectionalLightList;
}

vector<weak_ptr<SpotLight>> & SceneFactory::GetSpotLightList() {
	RemoveUnreferenced(msSpotLightList);
	return msSpotLightList;
}

vector<weak_ptr<SceneNode>> & SceneFactory::GetNodeList() {
	RemoveUnreferenced(msNodeList);
	return msNodeList;
}

shared_ptr<Camera> SceneFactory::CreateCamera(float fov) {
	auto camera = make_shared<Camera>(fov);
	msNodeList.push_back(camera);
	Camera::msCurrentCamera = camera;
	return camera;
}

shared_ptr<ParticleSystem> SceneFactory::CreateParticleSystem(int particleCount) {
	auto particleSystem = make_shared<ParticleSystem>(particleCount);
	msNodeList.push_back(particleSystem);
	msParticleEmitters.push_back(particleSystem);
	return particleSystem;
}

shared_ptr<Fog> SceneFactory::CreateFog(const ruVector3 & min, const ruVector3 & max, const ruVector3 & color, float density) {
	auto fog = make_shared<Fog>(min, max, color, density);
	msNodeList.push_back(fog);
	msFogList.push_back(fog);
	return fog;
}

template<typename Type>
void SceneFactory::RemoveUnreferenced(vector<weak_ptr<Type>> & objList) {
	for (auto iter = objList.begin(); iter != objList.end(); ) {
		if ((*iter).use_count()) {
			++iter;
		} else {
			iter = objList.erase(iter);
		}
	}
}

vector<weak_ptr<ParticleSystem>> & SceneFactory::GetParticleSystemList() {
	RemoveUnreferenced(msParticleEmitters);
	return msParticleEmitters;
}

vector<weak_ptr<Fog>>& SceneFactory::GetFogList() {
	RemoveUnreferenced(msFogList);
	return msFogList;
}
