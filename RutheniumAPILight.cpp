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

#include "Precompiled.h"
#include "RutheniumAPI.h"
#include "SceneNode.h"
#include "Light.h"

ruLight::ruLight() {
	pointer = nullptr;
}

ruLight::ruLight( const ruSceneNode & node ) {
	*this = node;
}

bool ruLight::operator == ( const ruLight & node ) {
	return pointer == node.pointer;
}

void ruLight::operator = ( const ruSceneNode & node ) {
	if( node.IsValid() ) {
		SceneNode * sNode = static_cast<SceneNode*>( node.pointer );
		if( dynamic_cast<Light*>( sNode )) {
			pointer = sNode;
		} else {
			pointer = nullptr;
		}
	} else {
		pointer = nullptr;
	}
}

ruLight ruLight::Create( ruLight::Type type ) {
	ruLight light;
	light.pointer = new Light( type );
	return light;
}

int ruLight::GetWorldSpotLightCount() {
	return Light::msSpotLightList.size();
}

ruLight ruLight::GetWorldSpotLight( int n ) {
	ruLight handle;
	if( n >= Light::msSpotLightList.size() || n < 0 ) {
		return handle;
	} else {
		handle.pointer = Light::msSpotLightList[n];
		return handle;
	}
}


int ruLight::GetWorldPointLightCount() {
	return Light::msPointLightList.size();
}

ruLight ruLight::GetWorldPointLight( int n ) {
	ruLight handle;
	if( n >= Light::msPointLightList.size() || n < 0 ) {
		return handle;
	} else {
		handle.pointer = Light::msPointLightList[n];
		return handle;
	}
}

void ruLight::SetRange( float range ) {
	if( IsValid() ) {
		static_cast<Light*>( pointer )->SetRange( range );
	}
}

float ruLight::GetRange() {
	if( IsValid() ) {
		return static_cast<Light*>( pointer )->GetRange();
	} else {
		return 0.0f;
	}
}

bool ruLight::IsSeePoint( const ruVector3 & point ) {
	if( IsValid() ) {
		return static_cast<Light*>( pointer )->IsSeePoint( point );
	} else {
		return false;
	}
}

void ruLight::SetColor( ruVector3 clr ) {
	if( IsValid() ) {
		static_cast<Light*>( pointer )->SetColor( clr );
	}
}

void ruLight::SetConeAngles( float innerAngle, float outerAngle ) {
	if( IsValid() ) {
		static_cast<Light*>( pointer )->SetConeAngles( innerAngle, outerAngle );
	}
}

void ruLight::SetSpotTexture( ruTextureHandle texture ) {
	if( IsValid() ) {
		static_cast<Light*>( pointer )->SetSpotTexture( static_cast<Texture*>( texture.pointer ));
	}
}

void ruLight::SetSpotDefaultTexture( ruTextureHandle defaultSpotTexture ) {
	Light::msDefaultSpotTexture = static_cast<Texture*>( defaultSpotTexture.pointer );
	for( auto spot : Light::msSpotLightList ) {
		if( !spot->mSpotTexture ) {
			spot->mSpotTexture = Light::msDefaultSpotTexture;
		}
	}	
}

void ruLight::SetPointTexture( ruCubeTextureHandle cubeTexture ) {
	if( IsValid() ) {
		static_cast<Light*>( pointer )->SetPointTexture( static_cast<CubeTexture*>( cubeTexture.pointer ) );
	}
}

void ruLight::SetPointDefaultTexture( ruCubeTextureHandle defaultPointTexture ) {
	Light::msDefaultPointCubeTexture = static_cast<CubeTexture*>( defaultPointTexture.pointer );
	for( auto point : Light::msPointLightList ) {
		if( !point->mPointTexture ) {
			point->mPointTexture = Light::msDefaultPointCubeTexture;
		}
	}
}

void ruLight::SetGreyscaleFactor( float factor ) {
	if( IsValid() ) {
		static_cast<Light*>( pointer )->SetGreyScaleFactor( factor );
	}
}