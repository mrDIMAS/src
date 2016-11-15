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
#include "RutheniumAPI.h"
#include "Renderer.h"
#include "Cursor.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Physics.h"
#include "SceneFactory.h"

void ruEngine::Create( int width, int height, int fullscreen, char vSync ) {
	pEngine = unique_ptr<Renderer>( new Renderer( width, height, fullscreen, vSync ));
}

void ruEngine::Free( ) {
	pEngine->Shutdown();
	pfSystemDestroy();
}

void ruEngine::RenderWorld( ) {
	pEngine->RenderWorld();
}

int ruEngine::GetResolutionWidth( ) {
	return pEngine->GetResolutionWidth();
}

int ruEngine::GetResolutionHeight( ) {
	return pEngine->GetResolutionHeight();
}

void ruEngine::HideCursor( ) {
	pEngine->SetCursorVisible( false );
}

void ruEngine::ShowCursor( ) {
	pEngine->SetCursorVisible( true );
}

void ruEngine::SetCursorSettings( shared_ptr<ruTexture> texture, int w, int h ) {
	pEngine->SetCursor( texture, w, h );
}

int ruEngine::GetDIPs( ) {
	return pEngine->GetDIPCount();
}

int ruEngine::GetMaxIsotropyDegree() {
	return pEngine->GetMaxIsotropyDegree();
}

void ruEngine::SetIsotropyDegree(int degree) {
	pEngine->SetIsotropyDegree(degree);
}

void ruEngine::SetDirectionalLightDynamicShadows(bool state) {
	pEngine->SetDirectionalLightDynamicShadows(state);
}

bool ruEngine::IsDirectionalLightDynamicShadowsEnabled() {
	return pEngine->IsDirectionalLightDynamicShadowsEnabled();
}

bool ruEngine::IsAnisotropicTextureFiltrationEnabled() {
	return pEngine->IsAnisotropicFilteringEnabled();
}

void ruEngine::SetVolumetricFogEnabled(bool state) {
	pEngine->SetVolumetricFogEnabled(state);
}

void ruEngine::SetDirectionalLightShadowMapSize(int size) {
	pEngine->SetDirectionalLightShadowMapSize(size);
}

int ruEngine::GetDirectionalLightShadowMapSize() {
	return pEngine->GetDirectionalLightShadowMapSize();
}

bool ruEngine::IsVolumetricFogEnabled() {
	return pEngine->IsVolumetricFogEnabled();
}

vector<ruVideomode> ruEngine::GetVideoModeList() {
	return pEngine->GetVideoModeList();
}

int ruEngine::GetTextureUsedPerFrame( ) {
	return pEngine->GetTextureChangeCount();
}

int ruEngine::GetShaderCountChangedPerFrame( ) {
	return pEngine->GetShaderChangeCount();
}

int ruEngine::GetRenderedTriangles() {
	return pEngine->GetRenderedTriangles();
}

void ruEngine::SetAmbientColor( ruVector3 color ) {
	pEngine->SetAmbientColor( color );
}

int ruEngine::GetAvailableTextureMemory() {
	return pD3D->GetAvailableTextureMem();
}

void ruEngine::EnableShadows( bool state ) {
	pEngine->SetSpotLightShadowsEnabled( state );
	pEngine->SetPointLightShadowsEnabled( state );
}

void ruEngine::UpdateWorld() {
	pEngine->UpdateWorld();
}

void ruEngine::SetAnisotropicTextureFiltration( bool state ) {
	pEngine->SetAnisotropicTextureFiltration( state );
}

void ruEngine::SetFXAAEnabled( bool state ) {
	pEngine->SetFXAAEnabled( state );
}

bool ruEngine::IsFXAAEnabled() {
	return pEngine->IsFXAAEnabled();
}

void ruEngine::ChangeVideomode( int width, int height, int fullscreen, char vSync ) {
	pEngine->ChangeVideomode( width, height, fullscreen, vSync );
}

void ruEngine::SetHDREnabled( bool state ) {
	pEngine->SetHDREnabled( state );
}

bool ruEngine::IsHDREnabled( ) {
	return pEngine->IsHDREnabled();
}

void ruEngine::SetSpotLightShadowMapSize( int size ) {
	pEngine->SetSpotLightShadowMapSize( size );
}

void ruEngine::EnableSpotLightShadows( bool state ) {
	pEngine->SetSpotLightShadowsEnabled( state );
}

bool ruEngine::IsSpotLightShadowsEnabled() {
	return pEngine->IsSpotLightShadowsEnabled();
}

void ruEngine::SetParallaxEnabled( bool state ) {
	pEngine->SetParallaxEnabled( state );
}

bool ruEngine::IsParallaxEnabled() {
	return pEngine->IsParallaxEnabled();
}


void ruEngine::SetPointLightShadowMapSize( int size ) {
	
}

void ruEngine::EnablePointLightShadows( bool state ) {
	pEngine->SetPointLightShadowsEnabled( state );
}

bool ruEngine::IsPointLightShadowsEnabled() {
	return pEngine->IsPointLightShadowsEnabled();
}

void ruEngine::LoadColorGradingMap(const char * fileName) {
	pEngine->LoadColorGradingMap(fileName);
}