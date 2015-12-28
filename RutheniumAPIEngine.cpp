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
#include "Engine.h"
#include "Cursor.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Physics.h"
#include "SceneFactory.h"

void ruEngine::Create( int width, int height, int fullscreen, char vSync ) {
	Engine::I().Initialize( width, height, fullscreen, vSync ) ;
}
void ruEngine::Free( ) {
	Engine::I().Shutdown();
}
void ruEngine::RenderWorld( ) {
	Engine::I().RenderWorld();
}
int ruEngine::GetResolutionWidth( ) {
	return Engine::I().GetResolutionWidth();
}
int ruEngine::GetResolutionHeight( ) {
	return Engine::I().GetResolutionHeight();
}
void ruEngine::HideCursor( ) {
	Engine::I().SetCursorVisible( false );
}
void ruEngine::ShowCursor( ) {
	Engine::I().SetCursorVisible( true );
}

void ruEngine::SetCursorSettings( shared_ptr<ruTexture> texture, int w, int h ) {
	Engine::I().SetCursor( texture, w, h );
}

int ruEngine::GetDIPs( ) {
	return Engine::I().GetDIPCount();
}
int ruEngine::GetMaxAnisotropy() {
	D3DCAPS9 caps;
	Engine::I().GetDevice()->GetDeviceCaps( &caps );

	return caps.MaxAnisotropy;
}
int ruEngine::GetTextureUsedPerFrame( ) {
	return Engine::I().GetTextureChangeCount();
}
void ruEngine::SetAmbientColor( ruVector3 color ) {
	Engine::I().SetAmbientColor( color );
}
int ruEngine::GetAvailableTextureMemory() {
	return Engine::I().GetDevice()->GetAvailableTextureMem();
}
void ruEngine::EnableShadows( bool state ) {
	Engine::I().SetSpotLightShadowsEnabled( state );
	Engine::I().SetPointLightShadowsEnabled( state );
}
void ruEngine::UpdateWorld() {
	// build view and projection matrices, frustum, also attach sound listener to camera
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		camera->Update();
	}

	auto & nodes = SceneFactory::GetNodeList();

	for( auto & pWeak : nodes ) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if( node ) {
			node->CalculateGlobalTransform();
			// update all sounds attached to node, and physical interaction sounds( roll, hit )
			node->UpdateSounds();
			if( !node->IsSkinned() ) {
				node->PerformAnimation();
			}
		}
	}

	// skinned animation is based on transforms of other nodes, so skin meshes in the end of all
	for( auto & pWeak : nodes ) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if( node ) {
			if( node->IsSkinned() ) {
				node->PerformAnimation();
			}
		} 
	}
}
void ruEngine::SetAnisotropicTextureFiltration( bool state ) {
	Engine::I().SetAnisotropicTextureFiltration( state );
}
void ruEngine::SetFXAAEnabled( bool state ) {
	Engine::I().SetFXAAEnabled( state );
}
bool ruEngine::IsFXAAEnabled() {
	return Engine::I().IsFXAAEnabled();
}
void ruEngine::ChangeVideomode( int width, int height, int fullscreen, char vSync ) {
	Engine::I().ChangeVideomode( width, height, fullscreen, vSync );
}
void ruEngine::SetHDREnabled( bool state ) {
	Engine::I().SetHDREnabled( state );
}
bool ruEngine::IsHDREnabled( ) {
	return Engine::I().IsHDREnabled();
}
void ruEngine::SetSpotLightShadowMapSize( int size ) {
	Engine::I().SetSpotLightShadowMapSize( size );
}
void ruEngine::EnableSpotLightShadows( bool state ) {
	Engine::I().SetSpotLightShadowsEnabled( state );
}
bool ruEngine::IsSpotLightShadowsEnabled() {
	return Engine::I().IsSpotLightShadowsEnabled();
}
void ruEngine::SetParallaxEnabled( bool state ) {
	Engine::I().SetParallaxEnabled( state );
}
bool ruEngine::IsParallaxEnabled() {
	return Engine::I().IsParallaxEnabled();
}
