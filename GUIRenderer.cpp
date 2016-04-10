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

#include "GUIRenderer.h"
#include "Camera.h"
#include "Texture.h"
#include "Cursor.h"
#include "Vertex.h"
#include "TextRenderer.h"
#include "BitmapFont.h"
#include "Engine.h"
#include "GUIFactory.h"

GUIRenderer::GUIRenderer() {
   Initialize();
   mVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/gui.vso" ));
   mPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/gui.pso" ));
}

GUIRenderer::~GUIRenderer() {
    OnLostDevice();
}

void GUIRenderer::Render() {
	pD3D->SetRenderState( D3DRS_ZENABLE, FALSE );

    mPixelShader->Bind();
    mVertexShader->Bind();

    pD3D->SetVertexDeclaration( mVertexDeclaration );

	pD3D->SetVertexShaderConstantF( 0, &mOrthoMatrix.m[0][0], 4 );

    pD3D->SetVertexDeclaration( mVertexDeclaration );
    pD3D->SetStreamSource( 0, mVertexBuffer, 0, sizeof( Vertex2D ));

	auto & nodes = GUIFactory::GetNodeList();
	for( auto & nWeak : nodes ) {
		shared_ptr<GUINode> & pNode = nWeak.lock();
		if( pNode ) {
			pNode->DoActions();
		}
	}

	auto & rects = GUIFactory::GetRectList();
    for( auto & rWeak : rects ) {
		shared_ptr<GUIRect> & pRect = rWeak.lock();
		if( pRect ) {
			if( pRect->IsVisible() ) {
				RenderRect( pRect );
			}
        }
    }

	auto & texts = GUIFactory::GetTextList();
    for( auto tWeak : texts ) {
		shared_ptr<GUIText> & pText = tWeak.lock();
		if( pText ) {
			if( pText->IsVisible() ) {
				pEngine->GetTextRenderer()->RenderText( pText );
			}
		}
    }
	
    if( pEngine->GetCursor() ) {
        pD3D->SetStreamSource( 0, mVertexBuffer, 0, sizeof( Vertex2D ));
        if( pEngine->GetCursor()->IsVisible() ) {
            pEngine->GetCursor()->SetPosition( 
				ruInput::GetMouseX() / pEngine->GetGUIWidthScaleFactor(), 
				ruInput::GetMouseY() / pEngine->GetGUIHeightScaleFactor());
            RenderRect( pEngine->GetCursor() );
        }
    }
}


void GUIRenderer::RenderRect( const shared_ptr<GUIRect> & r ) {
	if( !r->GetTexture() )  {
		return;
	}
    void * data = nullptr;
    Vertex2D vertices[6];
	r->CalculateTransform();
    r->GetSixVertices( vertices );
    mVertexBuffer->Lock( 0, 0, &data, D3DLOCK_DISCARD );
    memcpy( data, vertices, mSizeOfRectBytes );
    mVertexBuffer->Unlock( );
    pD3D->SetTexture( 0, std::dynamic_pointer_cast<Texture>(r->GetTexture())->GetInterface() );
    pEngine->RegisterDIP();
    pD3D->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
}

void GUIRenderer::OnLostDevice() {
	mVertexBuffer.Reset();
	mVertexDeclaration.Reset();
}

void GUIRenderer::OnResetDevice() {
	Initialize();
}

void GUIRenderer::Initialize() {
	mSizeOfRectBytes = 6 * sizeof( Vertex2D  );
	pD3D->CreateVertexBuffer( mSizeOfRectBytes, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_DEFAULT, &mVertexBuffer, 0 );

	D3DVERTEXELEMENT9 guivd[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0  },
		D3DDECL_END()
	};

	pD3D->CreateVertexDeclaration( guivd, &mVertexDeclaration );

	D3DVIEWPORT9 vp;
	pD3D->GetViewport( &vp );
	D3DXMatrixOrthoOffCenterLH ( &mOrthoMatrix, 0, vp.Width, vp.Height, 0, 0.0f, 1024.0f );
}

void Face::Set( unsigned short i1, unsigned short i2, unsigned short i3, unsigned short i4, unsigned short i5, unsigned short i6 ) {
	mIndex[0] = i1;
	mIndex[1] = i2;
	mIndex[2] = i3;
	mIndex[3] = i4;
	mIndex[4] = i5;
	mIndex[5] = i6;
}

void TextVertex::Set( float posX, float posY, ruVector2 & texCoord, DWORD color ) {
	mPosition.x = posX;
	mPosition.y = posY;
	mPosition.z = 0.0f;
	mTexCoord = texCoord;
	mColor = color;
}
