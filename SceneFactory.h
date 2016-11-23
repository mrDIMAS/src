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

class SceneNode;
class PointLight;
class SpotLight;
class Camera;
class ParticleSystem;
class DirectionalLight;
class Fog;
class Engine;

#include "Texture.h"
#include "CubeTexture.h"

class SceneFactory : public ISceneFactory {
private:
	Engine * const mEngine;
	vector<weak_ptr<SceneNode>> msNodeList;
	vector<weak_ptr<SpotLight>> msSpotLightList;
	vector<weak_ptr<PointLight>> msPointLightList;
	vector<weak_ptr<DirectionalLight>> msDirectionalLightList;
	vector<weak_ptr<ParticleSystem>> msParticleEmitters;
	vector<weak_ptr<Fog>> msFogList;
	shared_ptr<Texture> mDefaultSpotTexture;
	shared_ptr<CubeTexture> mDefaultPointCubeTexture;
	template<typename Type>
	void RemoveUnreferenced(vector<weak_ptr<Type>> & objList);
public:
	SceneFactory(Engine * engine);
	~SceneFactory() {}
	virtual Engine * GetEngine() const;


	// All 'Get***' Methods guarantee to return weak_ptr's to existing objects 
	vector<weak_ptr<SceneNode>> & GetNodeList();
	vector<weak_ptr<SpotLight>> & GetSpotLightList();
	vector<weak_ptr<PointLight>> & GetPointLightList();
	vector<weak_ptr<DirectionalLight>> & GetDirectionalLightList();
	vector<weak_ptr<ParticleSystem>> & GetParticleSystemList();
	vector<weak_ptr<Fog>> & GetFogList();

	// API Methods
	virtual IEngine * const GetEngineInterface() const override final;
	virtual shared_ptr<ISceneNode> CreateSceneNode() override final;
	virtual shared_ptr<ISceneNode> CreateSceneNodeDuplicate(shared_ptr<ISceneNode> src) override final;
	virtual shared_ptr<IPointLight> CreatePointLight() override final;
	virtual shared_ptr<ISpotLight> CreateSpotLight() override final;
	virtual shared_ptr<IDirectionalLight> CreateDirectionalLight() override final;
	virtual shared_ptr<ICamera> CreateCamera(float fov) override final;
	virtual shared_ptr<IParticleSystem> CreateParticleSystem(int particleCount) override final;
	virtual shared_ptr<IFog> CreateFog(const Vector3 & min, const Vector3 & max, const Vector3 & color, float density) override final;

	virtual shared_ptr<ISceneNode> FindByName(const string & name) override final;
	virtual shared_ptr<ISceneNode> LoadScene(const string & file) override final;
	virtual vector<shared_ptr<ISceneNode>> GetTaggedObjects(const string & tag) override final;
	virtual shared_ptr<ISceneNode> GetNode(int i) override final;
	virtual int GetNodeCount() override final;

	virtual int GetSpotLightCount() override final;
	virtual shared_ptr<ISpotLight> GetSpotLight(int n) override final;

	virtual int GetPointLightCount() override final;
	virtual shared_ptr<IPointLight> GetPointLight(int n) override final;

	virtual int GetDirectionalLightCount() override final;
	virtual shared_ptr<IDirectionalLight> GetDirectionalLight(int n) override final;

	virtual void SetPointLightDefaultTexture(const shared_ptr<ICubeTexture> & defaultPointTexture) override final;
	virtual shared_ptr<ICubeTexture> GetPointLightDefaultTexture() const override final;
	virtual void SetSpotLightDefaultTexture(const shared_ptr<ITexture> & defaultSpotTexture) override final;
	virtual shared_ptr<ITexture> GetSpotLightDefaultTexture() const override final;
};