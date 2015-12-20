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
    Engine::I().GetDevice()->SetRenderTarget( 0, mBackSurface );
    Engine::I().GetDevice()->SetRenderTarget( 1, 0 );
    Engine::I().GetDevice()->SetRenderTarget( 2, 0 );
    FreeRenderTargets();
}

void GBuffer::BindRenderTargets() {
    UnbindTextures();

    Engine::I().GetDevice()->SetRenderTarget( 0, mDepthSurface );
    Engine::I().GetDevice()->SetRenderTarget( 1, mNormalSurface );
    Engine::I().GetDevice()->SetRenderTarget( 2, mDiffuseSurface );
}

void GBuffer::BindNormalMapAsRT() {
    Engine::I().GetDevice()->SetRenderTarget( 0, mNormalSurface );
};

void GBuffer::BindDiffuseMapAsRT() {
    Engine::I().GetDevice()->SetRenderTarget( 0, mDiffuseSurface );
};

void GBuffer::BindDepthMapAsRT() {
    Engine::I().GetDevice()->SetRenderTarget( 0, mDepthSurface );
};

void GBuffer::UnbindTextures() {
    Engine::I().GetDevice()->SetTexture( 0, 0 );
    Engine::I().GetDevice()->SetTexture( 1, 0 );
    Engine::I().GetDevice()->SetTexture( 2, 0 );
};

void GBuffer::UnbindRenderTargets() {
    Engine::I().GetDevice()->SetRenderTarget( 0, mBackSurface );
    Engine::I().GetDevice()->SetRenderTarget( 1, 0 );
    Engine::I().GetDevice()->SetRenderTarget( 2, 0 );
}

void GBuffer::BindTextures() {
    Engine::I().GetDevice()->SetTexture( 0, mDepthMap );
    Engine::I().GetDevice()->SetTexture( 1, mNormalMap );
    Engine::I().GetDevice()->SetTexture( 2, mDiffuseMap );
}

void GBuffer::BindDepthMap( int layer ) {
    Engine::I().GetDevice()->SetTexture( layer, mDepthMap );
}

void GBuffer::FreeRenderTargets() {
	mDepthSurface.Reset();
	mNormalSurface.Reset();
	mDiffuseSurface.Reset();
	mDepthMap.Reset();
	mNormalMap.Reset();
	mDiffuseMap.Reset();
}

void GBuffer::CreateRenderTargets() {
	int width = Engine::I().GetResolutionWidth();
	int height = Engine::I().GetResolutionHeight();
	if( !Engine::I().IsNonPowerOfTwoTexturesSupport()) {
		width = FloorPow2( Engine::I().GetResolutionWidth() );
		height = FloorPow2( Engine::I().GetResolutionHeight() );
	}

	Engine::I().GetDevice()->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mDepthMap, nullptr );
	Engine::I().GetDevice()->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mNormalMap, nullptr );
	Engine::I().GetDevice()->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mDiffuseMap, nullptr );
	mDepthMap->GetSurfaceLevel( 0, &mDepthSurface );
	mNormalMap->GetSurfaceLevel( 0, &mNormalSurface );
	mDiffuseMap->GetSurfaceLevel( 0, &mDiffuseSurface );
	mBackSurface = Engine::I().GetBackBuffer();
}

void GBuffer::OnLostDevice() {
	FreeRenderTargets();
}

void GBuffer::OnResetDevice() {
	CreateRenderTargets();
}
