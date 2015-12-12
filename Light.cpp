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
#include "Light.h"
#include "SceneNode.h"
#include "Utility.h"

vector< Light* > Light::msPointLightList;
vector< Light* > Light::msSpotLightList;
vector<Light*> Light::msLightList;
Texture * Light::msDefaultSpotTexture = nullptr;
CubeTexture * Light::msDefaultPointCubeTexture = nullptr;

Light * Light::GetLightByHandle( ruSceneNode handle ) {
    SceneNode * n = SceneNode::CastHandle( handle );
    Light * light = dynamic_cast< Light* >( n );
    if( !light ) {
        MessageBoxA( 0, (StringBuilder( "Node " ) << n->mName << " is not a light!").ToCStr(), "Error", MB_OK );
        exit( -1 );
    }
    return light;
};

bool ruIsLightHandeValid( ruSceneNode handle ) {
	auto pointIter = find( Light::msPointLightList.begin(), Light::msPointLightList.end(), Light::GetLightByHandle( handle ) );
	if( pointIter != Light::msPointLightList.end() ) {
		return true;
	}
	auto spotIter = find( Light::msSpotLightList.begin(), Light::msSpotLightList.end(), Light::GetLightByHandle( handle ) );
	if( spotIter != Light::msSpotLightList.end() ) {
		return true;
	}
	return false;
}

Light::Light( ruLight::Type type ) : mGreyScaleFactor( 0.0f ) {
    mColor = ruVector3( 1.0f, 1.0f, 1.0f );
    mRadius = 1.0f;
    mPointTexture = nullptr;
    mSpotTexture = nullptr;
	mQueryDone = true;
    this->mType = type;
    if( type == ruLight::Type::Point ) {
        Light::msPointLightList.push_back( this );
        if( msDefaultPointCubeTexture ) {
            mPointTexture = msDefaultPointCubeTexture;
        }
    }
    if( type == ruLight::Type::Spot ) {
        Light::msSpotLightList.push_back( this );
        if( msDefaultSpotTexture ) {
            mSpotTexture = msDefaultSpotTexture;
        }
    }
    SetConeAngles( 45.0f, 80.0f );
	OnResetDevice();
	mInFrustum = false;
}

void Light::SetColor( const ruVector3 & theColor ) {
    mColor.x = theColor.x / 255.0f;
    mColor.y = theColor.y / 255.0f;
    mColor.z = theColor.z / 255.0f;
}

ruVector3 Light::GetColor() const {
    return mColor;
}

void Light::SetRange( const float & theRadius ) {
    mRadius = theRadius;
}

float Light::GetRange() const {
    return mRadius;
}

float Light::GetInnerAngle() const {
    return mInnerAngle;
}

float Light::GetOuterAngle() const {
    return mOuterAngle;
}

void Light::SetConeAngles( float theInner, float theOuter ) {
    mInnerAngle = theInner;
    mOuterAngle = theOuter;

    mCosHalfInnerAngle = cosf( ( mInnerAngle / 2 ) * SIMD_PI / 180.0f );
    mCosHalfOuterAngle = cosf( ( mOuterAngle / 2 ) * SIMD_PI / 180.0f );
}

float Light::GetCosHalfInnerAngle( ) {
    return mCosHalfInnerAngle;
}

float Light::GetCosHalfOuterAngle( ) {
    return mCosHalfOuterAngle;
}

Light::~Light() {
    auto pointLight = find( Light::msPointLightList.begin(), Light::msPointLightList.end(), this );
    if( pointLight != Light::msPointLightList.end() ) {
        Light::msPointLightList.erase( pointLight );
    }

    auto spotLight = find( Light::msSpotLightList.begin(), Light::msSpotLightList.end(), this );
    if( spotLight != Light::msSpotLightList.end() ) {
        Light::msSpotLightList.erase( spotLight );
    }

	if( Camera::msCurrentCamera ) {
		if( Camera::msCurrentCamera->mNearestPathPoint ) {
			auto & litList = Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList;
			auto iter = find( litList.begin(), litList.end(), this );
			if( iter != litList.end() ) {
				litList.erase( iter );
			}
		}		
	}
	pQuery->Release();
}

void Light::BuildSpotProjectionMatrixAndFrustum() {
	ruVector3 position = GetPosition();
    btVector3 bEye = btVector3( position.x, position.y, position.z );
    btVector3 bLookAt = bEye + mGlobalTransform.getBasis() * btVector3( 0, -1, 0 );
    btVector3 bUp = mGlobalTransform.getBasis() * btVector3( 1, 0, 0 );

    D3DXVECTOR3 dxEye( bEye.x(), bEye.y(), bEye.z() );
    D3DXVECTOR3 dxLookAt( bLookAt.x(), bLookAt.y(), bLookAt.z() );
    D3DXVECTOR3 dxUp( bUp.x(), bUp.y(), bUp.z() );

    D3DXMATRIX mView, mProj;
    D3DXMatrixLookAtRH( &mView, &dxEye, &dxLookAt, &dxUp );
    D3DXMatrixPerspectiveFovRH( &mProj, mOuterAngle * SIMD_PI / 180.0f, 1.0f, 0.1f, 1000.0f );
    D3DXMatrixMultiply( &mSpotViewProjectionMatrix, &mView, &mProj );
    mFrustum.Build( mSpotViewProjectionMatrix );
}

void Light::SetSpotTexture( Texture * tex ) {
    mSpotTexture = tex;
}

void Light::SetPointTexture( CubeTexture * ctex ) {
    mPointTexture = ctex;
}



void Light::OnResetDevice()
{
	Engine::Instance().GetDevice()->CreateQuery( D3DQUERYTYPE_OCCLUSION, &pQuery );
	mQueryDone = true;
}

void Light::OnLostDevice()
{
	pQuery->Release();
}

void Light::SetGreyScaleFactor( float greyScaleFactor )
{
	mGreyScaleFactor = greyScaleFactor;
}

float Light::GetGreyScaleFactor()
{
	return mGreyScaleFactor;
}

bool Light::IsSeePoint( const ruVector3 & point )
{
	if( mType == ruLight::Type::Spot ) {
		bool inFrustum = mFrustum.IsPointInside( point );
		if( inFrustum ) {
			return ( ruVector3( mGlobalTransform.getOrigin().m_floats ) - point ).Length2() < mRadius * mRadius * 4;
		}
	} else {
		return (ruVector3( mGlobalTransform.getOrigin().m_floats ) - point ).Length2() < mRadius * mRadius * 4;
	}
	return false;
}
