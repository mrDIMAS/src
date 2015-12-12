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

void ruEngine::Create( int width, int height, int fullscreen, char vSync ) {
	Engine::Instance().Initialize( width, height, fullscreen, vSync ) ;
}
void ruEngine::Free( ) {
	Engine::Instance().Shutdown();
}
void ruEngine::RenderWorld( ) {
	Engine::Instance().RenderWorld();
}
int ruEngine::GetResolutionWidth( ) {
	return Engine::Instance().GetResolutionWidth();
}
int ruEngine::GetResolutionHeight( ) {
	return Engine::Instance().GetResolutionHeight();
}
void ruEngine::HideCursor( ) {
	if( Cursor::msCurrentCursor ) {
		Cursor::msCurrentCursor->Hide();
	} else {
		::ShowCursor( 0 );
		Engine::Instance().GetDevice()->ShowCursor( 0 );
	}
}
void ruEngine::ShowCursor( ) {
	if( Cursor::msCurrentCursor ) {
		Cursor::msCurrentCursor->Show();
	} else {
		::ShowCursor( 1 );
		Engine::Instance().GetDevice()->ShowCursor( 1 );
	}
}

void ruEngine::SetCursorSettings( ruTextureHandle texture, int w, int h ) {
	if( !Cursor::msCurrentCursor ) {
		Cursor::msCurrentCursor = new Cursor( w, h, (Texture*)texture.pointer );
	}	
}
int ruEngine::GetDIPs( ) {
	return Engine::Instance().GetDIPCount();
}
int ruEngine::GetMaxAnisotropy() {
	D3DCAPS9 caps;
	Engine::Instance().GetDevice()->GetDeviceCaps( &caps );

	return caps.MaxAnisotropy;
}
int ruEngine::GetTextureUsedPerFrame( ) {
	return Engine::Instance().GetTextureChangeCount();
}
void ruEngine::SetAmbientColor( ruVector3 color ) {
	Engine::Instance().SetAmbientColor( color );
}
int ruEngine::GetAvailableTextureMemory() {
	return Engine::Instance().GetDevice()->GetAvailableTextureMem();
}
void ruEngine::EnableShadows( bool state ) {
	Engine::Instance().SetSpotLightShadowsEnabled( state );
	Engine::Instance().SetPointLightShadowsEnabled( state );
}
void ruEngine::UpdateWorld() {
	// build view and projection matrices, frustum, also attach sound listener to camera
	if( Camera::msCurrentCamera ) {
		Camera::msCurrentCamera->Update();
	}
	for( auto node : SceneNode::msNodeList ) {
		node->CalculateGlobalTransform();
		// update all sounds attached to node, and physical interaction sounds( roll, hit )
		node->UpdateSounds();
		if( !node->mIsSkinned ) {
			node->PerformAnimation();
		}
	}

	// skinned animation is based on transforms of other nodes, so skin meshes in the end of all
	for( auto node : SceneNode::msNodeList ) {
		if( node->mIsSkinned ) {
			node->PerformAnimation();
		}
	}
}
void ruEngine::SetAnisotropicTextureFiltration( bool state ) {
	Engine::Instance().SetAnisotropicTextureFiltration( state );
}
void ruEngine::SetFXAAEnabled( bool state ) {
	Engine::Instance().SetFXAAEnabled( state );
}
bool ruEngine::IsFXAAEnabled() {
	return Engine::Instance().IsFXAAEnabled();
}
void ruEngine::ChangeVideomode( int width, int height, int fullscreen, char vSync ) {
	Engine::Instance().ChangeVideomode( width, height, fullscreen, vSync );
}
void ruEngine::SetHDREnabled( bool state ) {
	Engine::Instance().SetHDREnabled( state );
}
bool ruEngine::IsHDREnabled( ) {
	return Engine::Instance().IsHDREnabled();
}
void ruEngine::SetSpotLightShadowMapSize( int size ) {
	Engine::Instance().SetSpotLightShadowMapSize( size );
}
void ruEngine::EnableSpotLightShadows( bool state ) {
	Engine::Instance().SetSpotLightShadowsEnabled( state );
}
bool ruEngine::IsSpotLightShadowsEnabled() {
	return Engine::Instance().IsSpotLightShadowsEnabled();
}
void ruEngine::SetParallaxEnabled( bool state ) {
	Engine::Instance().SetParallaxEnabled( state );
}
bool ruEngine::IsParallaxEnabled() {
	return Engine::Instance().IsParallaxEnabled();
}
