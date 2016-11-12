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

class SceneFactory {
private:
	static vector<weak_ptr<SceneNode>> msNodeList;
	static vector<weak_ptr<SpotLight>> msSpotLightList;
	static vector<weak_ptr<PointLight>> msPointLightList;
	static vector<weak_ptr<DirectionalLight>> msDirectionalLightList;
	static vector<weak_ptr<ParticleSystem>> msParticleEmitters;
	template<typename Type> 
	static void RemoveUnreferenced( vector<weak_ptr<Type>> & objList );
public:
	// All 'Get***' Methods guarantee to return weak_ptr's to existing objects 
	static vector<weak_ptr<SceneNode>> & GetNodeList();
	static vector<weak_ptr<SpotLight>> & GetSpotLightList();
	static vector<weak_ptr<PointLight>> & GetPointLightList();
	static vector<weak_ptr<DirectionalLight>> & GetDirectionalLightList();
	static vector<weak_ptr<ParticleSystem>> & GetParticleSystemList();
	static shared_ptr<SceneNode> CreateSceneNode( );
	static shared_ptr<SceneNode> CreateSceneNodeDuplicate( shared_ptr<SceneNode> source );
	static shared_ptr<PointLight> CreatePointLight();
	static shared_ptr<SpotLight> CreateSpotLight();
	static shared_ptr<DirectionalLight> CreateDirectionalLight();
	static shared_ptr<Camera> CreateCamera( float fov );
	static shared_ptr<ParticleSystem> CreateParticleSystem( int particleCount );
};