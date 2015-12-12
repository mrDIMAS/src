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
#include "Engine.h"
#include "GBuffer.h"
#include "Utility.h"

GBuffer::GBuffer() {
	CreateRenderTargets();
}

GBuffer::~GBuffer() {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, backSurface );
    Engine::Instance().GetDevice()->SetRenderTarget( 1, 0 );
    Engine::Instance().GetDevice()->SetRenderTarget( 2, 0 );
    FreeRenderTargets();
}

void GBuffer::BindRenderTargets() {
    UnbindTextures();

    Engine::Instance().GetDevice()->SetRenderTarget( 0, depthSurface );
    Engine::Instance().GetDevice()->SetRenderTarget( 1, normalSurface );
    Engine::Instance().GetDevice()->SetRenderTarget( 2, diffuseSurface );
}

void GBuffer::BindNormalMapAsRT() {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, normalSurface );
};

void GBuffer::BindDiffuseMapAsRT() {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, diffuseSurface );
};

void GBuffer::BindDepthMapAsRT() {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, depthSurface );
};

void GBuffer::UnbindTextures() {
    Engine::Instance().GetDevice()->SetTexture( 0, 0 );
    Engine::Instance().GetDevice()->SetTexture( 1, 0 );
    Engine::Instance().GetDevice()->SetTexture( 2, 0 );
};

void GBuffer::UnbindRenderTargets() {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, backSurface );
    Engine::Instance().GetDevice()->SetRenderTarget( 1, 0 );
    Engine::Instance().GetDevice()->SetRenderTarget( 2, 0 );
}

void GBuffer::BindBackSurfaceAsRT() {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, backSurface );
};

void GBuffer::BindTextures() {
    Engine::Instance().GetDevice()->SetTexture( 0, depthMap );
    Engine::Instance().GetDevice()->SetTexture( 1, normalMap );
    Engine::Instance().GetDevice()->SetTexture( 2, diffuseMap );
}

void GBuffer::BindDepthMap( int layer ) {
    Engine::Instance().GetDevice()->SetTexture( layer, depthMap );
}


void GBuffer::FreeRenderTargets() {
	depthSurface->Release();
	normalSurface->Release();
	diffuseSurface->Release();
	while( depthMap->Release());
	while( normalMap->Release());
	while( diffuseMap->Release());
}

void GBuffer::CreateRenderTargets() {
	int width = Engine::Instance().GetResolutionWidth();
	int height = Engine::Instance().GetResolutionHeight();
	if( !Engine::Instance().IsNonPowerOfTwoTexturesSupport()) {
		width = FloorPow2( Engine::Instance().GetResolutionWidth() );
		height = FloorPow2( Engine::Instance().GetResolutionHeight() );
	}

	Engine::Instance().GetDevice()->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &depthMap, nullptr );
	Engine::Instance().GetDevice()->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &normalMap, nullptr );
	Engine::Instance().GetDevice()->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &diffuseMap, nullptr );
	depthMap->GetSurfaceLevel( 0, &depthSurface );
	normalMap->GetSurfaceLevel( 0, &normalSurface );
	diffuseMap->GetSurfaceLevel( 0, &diffuseSurface );
	backSurface = Engine::Instance().GetBackBuffer();
}

void GBuffer::OnLostDevice()
{
	FreeRenderTargets();
}

void GBuffer::OnResetDevice()
{
	CreateRenderTargets();
}
