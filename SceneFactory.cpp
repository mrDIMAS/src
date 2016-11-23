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
#include "FastReader.h"
#include "Engine.h"
#include "CubeTexture.h"


// API Methods

IEngine * const SceneFactory::GetEngineInterface() const {
	return mEngine;
}

shared_ptr<ISceneNode> SceneFactory::CreateSceneNode() {
	auto sceneNode = make_shared<SceneNode>(this);
	msNodeList.push_back(sceneNode);
	return sceneNode;
}

shared_ptr<ISceneNode> SceneFactory::CreateSceneNodeDuplicate(shared_ptr<ISceneNode> src) {
	auto source = dynamic_pointer_cast<SceneNode>(src);
	auto duplicate = dynamic_pointer_cast<SceneNode>(CreateSceneNode());
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
	for(auto pMesh : source->mMeshList) {
		pMesh->LinkTo(duplicate);
		duplicate->mMeshList.push_back(pMesh);
	}

	// create body
	switch(source->GetBodyType()) {
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

	if(duplicate->mFrozen) {
		duplicate->Freeze();
	}

	// copy childs
	for(auto & pChild : source->mChildren) {
		shared_ptr<SceneNode> & pNewChild = dynamic_pointer_cast<SceneNode>(SceneFactory::CreateSceneNodeDuplicate(pChild));
		pNewChild->mParent = duplicate;
		duplicate->mChildren.push_back(pNewChild);
	}
	return duplicate;
}

shared_ptr<IPointLight> SceneFactory::CreatePointLight() {
	auto pointLight = make_shared<PointLight>(this);
	msNodeList.push_back(pointLight);
	msPointLightList.push_back(pointLight);
	return pointLight;
}

shared_ptr<ISpotLight> SceneFactory::CreateSpotLight() {
	auto spotLight = make_shared<SpotLight>(this);
	msNodeList.push_back(spotLight);
	msSpotLightList.push_back(spotLight);
	return spotLight;
}

shared_ptr<IDirectionalLight> SceneFactory::CreateDirectionalLight() {
	auto dirLight = make_shared<DirectionalLight>(this);
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

SceneFactory::SceneFactory(Engine * engine) : mEngine(engine) {

}

Engine * SceneFactory::GetEngine() const {
	return mEngine;
}

vector<weak_ptr<SceneNode>> & SceneFactory::GetNodeList() {
	RemoveUnreferenced(msNodeList);
	return msNodeList;
}

shared_ptr<ICamera> SceneFactory::CreateCamera(float fov) {
	auto camera = make_shared<Camera>(this, fov);
	msNodeList.push_back(camera);
	Camera::msCurrentCamera = camera;
	return camera;
}

shared_ptr<IParticleSystem> SceneFactory::CreateParticleSystem(int particleCount) {
	auto particleSystem = make_shared<ParticleSystem>(this, particleCount);
	msNodeList.push_back(particleSystem);
	msParticleEmitters.push_back(particleSystem);
	return particleSystem;
}

shared_ptr<IFog> SceneFactory::CreateFog(const Vector3 & min, const Vector3 & max, const Vector3 & color, float density) {
	auto fog = make_shared<Fog>(this, min, max, color, density);
	msNodeList.push_back(fog);
	msFogList.push_back(fog);
	return fog;
}

shared_ptr<ISceneNode> SceneFactory::FindByName(const string & name) {
	for(auto & pWeak : msNodeList) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if(node) {
			if(node->mName == name) {
				return node;
			}
		}
	}
	return nullptr;
}

shared_ptr<ISceneNode> SceneFactory::LoadScene(const string & file) {
	FastReader reader;

	if(!reader.ReadFile(file)) {
		Log::Error(StringBuilder("Unable to load '") << file << "' scene!");
	}

	int numObjects = reader.GetInteger();
	int numMeshes = reader.GetInteger();
	int numLights = reader.GetInteger();
	int framesCount = reader.GetInteger();

	auto scene = dynamic_pointer_cast<SceneNode>(CreateSceneNode());

	scene->mTotalFrameCount = framesCount;

	for(int meshObjectNum = 0; meshObjectNum < numMeshes; meshObjectNum++) {
		auto node = dynamic_pointer_cast<SceneNode>(CreateSceneNode());

		node->mLocalTransform.setOrigin(reader.GetVector());
		node->mLocalTransform.setRotation(reader.GetQuaternion());
		node->mGlobalTransform = node->mLocalTransform;

		int hasAnimation = reader.GetInteger();
		int isSkinned = reader.GetInteger();
		int meshCount = reader.GetInteger();
		int keyframeCount = reader.GetInteger();
		node->mIsSkinned = isSkinned;
		ParseString(reader.GetString(), node->mProperties);
		node->mName = reader.GetString();
		for(int i = 0; i < keyframeCount; i++) {
			auto keyframe = make_unique<btTransform>();
			keyframe->setOrigin(reader.GetVector());
			keyframe->setRotation(reader.GetQuaternion());
			node->mKeyframeList.push_back(std::move(keyframe));
		}

		if(keyframeCount) {
			node->mLocalTransform = *node->mKeyframeList[0];
		}

		node->mTotalFrameCount = framesCount - 1;

		for(int i = 0; i < meshCount; i++) {
			shared_ptr<Mesh> & mesh = make_shared<Mesh>();
			mesh->LinkTo(node);

			int vertexCount = reader.GetInteger();
			int indexCount = reader.GetInteger();

			Vector3 aabbMin = reader.GetBareVector();
			Vector3 aabbMax = reader.GetBareVector();
			Vector3 aabbCenter = reader.GetBareVector(); // odd
			float aabbRadius = reader.GetFloat(); // odd

			string diffuse = reader.GetString();
			string normal = reader.GetString();
			mesh->SetOpacity(reader.GetFloat() / 100.0f);

			for(int vertexNum = 0; vertexNum < vertexCount; vertexNum++) {
				Vertex v;

				v.mPosition = reader.GetBareVector();
				v.mNormal = reader.GetBareVector();
				v.mTexCoord = reader.GetBareVector2();
				Vector2 tc2 = reader.GetBareVector2(); // odd
				v.mTangent = reader.GetBareVector();

				mesh->AddVertex(v);
			}

			for(int indexNum = 0; indexNum < indexCount; indexNum += 3) {
				unsigned short a = reader.GetShort();
				unsigned short b = reader.GetShort();
				unsigned short c = reader.GetShort();

				mesh->AddTriangle(Triangle(a, b, c));
			}

			mesh->mDiffuseTexture = dynamic_pointer_cast<Texture>(mEngine->GetRenderer()->GetTexture(mEngine->GetRenderer()->GetTextureStoragePath() + diffuse));
			if(mesh->GetOpacity() > 0.95f) {
				mesh->mNormalTexture = dynamic_pointer_cast<Texture>(mEngine->GetRenderer()->GetTexture(mEngine->GetRenderer()->GetTextureStoragePath() + normal));
				// try to load height map
				string height = diffuse.substr(0, diffuse.find_first_of('.')) + "_height" + diffuse.substr(diffuse.find_first_of('.'));
				if(FileExist(mEngine->GetRenderer()->GetTextureStoragePath() + height)) {
					mesh->mHeightTexture = dynamic_pointer_cast<Texture>(mEngine->GetRenderer()->GetTexture(mEngine->GetRenderer()->GetTextureStoragePath() + height));
				}
			}
			node->AddMesh(mesh);

			if(node->mIsSkinned) {
				for(int k = 0; k < vertexCount; k++) {
					Mesh::BoneGroup boneGroup;
					boneGroup.mBoneCount = reader.GetInteger();
					for(int j = 0; j < boneGroup.mBoneCount; j++) {
						// number of scene node represents bone in the scene
						boneGroup.mBone[j].mID = reader.GetInteger();
						boneGroup.mBone[j].mWeight = reader.GetFloat();
						boneGroup.mBone[j].mRealBone = nullptr;
					}
					mesh->AddBoneGroup(boneGroup);
				}
			}

			if(vertexCount != 0) {
				if(!mesh->IsSkinned()) {
					mesh->CreateHardwareBuffers();
				}
				mEngine->GetRenderer()->AddMesh(mesh);
			}
		}

		node->mParent = scene;
		scene->mChildren.push_back(node);

		node->mScene = scene;
		node->ApplyProperties();
	}

	// remap bone id's to real scene nodes 
	for(auto & child : scene->mChildren) {
		if(child->mIsSkinned) {
			for(auto pMesh : child->mMeshList) {
				for(auto & w : pMesh->GetBoneTable()) {
					for(int i = 0; i < w.mBoneCount; ++i) {
						shared_ptr<SceneNode> & bone = scene->mChildren[w.mBone[i].mID];
						if(bone) {
							w.mBone[i].mRealBone = pMesh->AddBone(bone);
						}
					}
				}
				pMesh->CreateHardwareBuffers();
			}
		}
	}

	for(int lightObjectNum = 0; lightObjectNum < numLights; lightObjectNum++) {
		shared_ptr<Light> light;
		string name = reader.GetString();
		int type = reader.GetInteger();
		if(type == 0) {
			light = dynamic_pointer_cast<Light>(CreatePointLight());
		} else if(type == 1) {
			light = dynamic_pointer_cast<Light>(CreateSpotLight());
		} else if(type == 2) {
			light = dynamic_pointer_cast<Light>(CreateDirectionalLight());
		}
		light->mName = name;
		light->SetColor(reader.GetBareVector());
		light->SetRange(reader.GetFloat());
		float brightness = reader.GetFloat();
		light->mLocalTransform.setOrigin(reader.GetVector());
		light->mScene = scene;
		light->mParent = scene;
		scene->mChildren.push_back(light);
		if(type == 1) { // spot
			shared_ptr<SpotLight> & spot = std::dynamic_pointer_cast<SpotLight>(light);
			float in = reader.GetFloat();
			float out = reader.GetFloat();
			spot->SetConeAngles(in, out);
			light->mLocalTransform.setRotation(reader.GetQuaternion());
		} else if(type == 2) { // directional
			light->mLocalTransform.setRotation(reader.GetQuaternion());
		}
	}

	for(auto child : scene->mChildren) {
		string objectName = reader.GetString();
		string parentName = reader.GetString();

		auto object = SceneNode::FindChildInNodeNonRecursive(scene, objectName);
		auto parent = SceneNode::FindChildInNodeNonRecursive(scene, parentName);

		if(parent) {
			parent->mChildren.push_back(object);
			object->mParent = parent;
		}
	}

	for(auto & node : scene->mChildren) {
		node->mInverseBindTransform = node->CalculateGlobalTransform().inverse();
	}

	return scene;
}

int SceneFactory::GetNodeCount() {
	return GetNodeList().size();
}

int SceneFactory::GetSpotLightCount() {
	return GetSpotLightList().size();
}

shared_ptr<ISpotLight> SceneFactory::GetSpotLight(int n) {
	return GetSpotLightList()[n].lock();
}

int SceneFactory::GetPointLightCount() {
	return GetPointLightList().size();
}

shared_ptr<IPointLight> SceneFactory::GetPointLight(int n) {
	return GetPointLightList()[n].lock();
}

int SceneFactory::GetDirectionalLightCount() {
	return GetDirectionalLightList().size();
}

shared_ptr<IDirectionalLight> SceneFactory::GetDirectionalLight(int n) {
	return GetDirectionalLightList()[n].lock();
}

void SceneFactory::SetPointLightDefaultTexture(const shared_ptr<ICubeTexture>& defaultPointTexture) {
	mDefaultPointCubeTexture = dynamic_pointer_cast<CubeTexture>(defaultPointTexture);
}

shared_ptr<ICubeTexture> SceneFactory::GetPointLightDefaultTexture() const {
	return mDefaultPointCubeTexture;
}

void SceneFactory::SetSpotLightDefaultTexture(const shared_ptr<ITexture>& defaultSpotTexture) {
	mDefaultSpotTexture = dynamic_pointer_cast<Texture>(defaultSpotTexture);
}

shared_ptr<ITexture> SceneFactory::GetSpotLightDefaultTexture() const {
	return mDefaultSpotTexture;
}

shared_ptr<ISceneNode> SceneFactory::GetNode(int i) {
	return GetNodeList()[i].lock();
}

vector<shared_ptr<ISceneNode>> SceneFactory::GetTaggedObjects(const string & tag) {
	vector<shared_ptr<ISceneNode>> tagged;
	for(auto weakNode : GetNodeList()) {
		auto node = weakNode.lock();
		if(node->GetTag() == tag) {
			tagged.push_back(node);
		}
	}
	return tagged;
}

template<typename Type>
void SceneFactory::RemoveUnreferenced(vector<weak_ptr<Type>> & objList) {
	for(auto iter = objList.begin(); iter != objList.end(); ) {
		if((*iter).use_count()) {
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
