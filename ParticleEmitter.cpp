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
#include "ParticleEmitter.h"
#include "Camera.h"
#include "Texture.h"
#include "Utility.h"

vector<ParticleSystem*> ParticleSystem::msParticleEmitters;

ParticleSystem::~ParticleSystem() {
	OnLostDevice();

    delete mVertices;
    delete mFaces;

    ParticleSystem::msParticleEmitters.erase( find( ParticleSystem::msParticleEmitters.begin(), ParticleSystem::msParticleEmitters.end(), this ));
}

void ParticleSystem::Render() {
    Engine::I().RegisterDIP();
    Engine::I().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, mAliveParticleCount * 4, 0, mAliveParticleCount * 2 );
}

void ParticleSystem::Bind() {
    if( mTexture ) {
        mTexture->Bind( 0 );
    }
    Engine::I().GetDevice()->SetStreamSource( 0, mVertexBuffer, 0, sizeof( ParticleVertex ));
    Engine::I().GetDevice()->SetIndices( mIndexBuffer );
}

void ParticleSystem::Update() {
    if( !mFirstTimeUpdate ) {
        ResurrectParticles();
        mFirstTimeUpdate = true;
    }

    mAliveParticleCount = 0;

    mGlobalTransform.getBasis().setEulerYPR( 0, 0, 0 );
    GetD3DMatrixFromBulletTransform( mGlobalTransform, mWorldTransform );

    D3DXMATRIX view = Camera::msCurrentCamera->mView;

    ruVector3 rightVect = ruVector3( view._11, view._21, view._31 ).Normalize();
    ruVector3 upVect = ruVector3( view._12, view._22, view._32 ).Normalize();

    ruVector3 leftTop = upVect - rightVect;
    ruVector3 rightTop = upVect + rightVect;
    ruVector3 rightBottom = rightVect - upVect;
    ruVector3 leftBottom = -( rightVect + upVect );

    int vertexNum = 0, faceNum = 0;

    for( auto & p : mParticles ) {
        p.mPosition += p.mSpeed;

        float insideCoeff = 1.0f;
        if( mType == ruParticleSystem::Type::Box ) {
            insideCoeff = ( p.mPosition ).Length2() / ( mBoundingBoxMax - mBoundingBoxMin ).Length2();
        } else {
            insideCoeff = ( p.mPosition ).Length2() / mBoundingRadius;
        }
        if( insideCoeff > 1.0f ) {
            insideCoeff = 1.0f;
        }
        p.mColor = mColorBegin.Lerp( mColorEnd, insideCoeff );
		float fakeOpacity = 255.0f * ( 1.0f - insideCoeff );
        p.mOpacity = fakeOpacity + mAlphaOffset;
        p.mSize += mScaleFactor;

		
        if( p.mOpacity <= 1.0f  ) {
            if( mAutoResurrectDeadParticles ) {
                ResurrectParticle( p );
            }
        } else {
			int color = RGBAToInt( p.mColor, p.mOpacity );

			short v1 = vertexNum + 1;
			short v2 = vertexNum + 2;
			short v3 = vertexNum + 3;

            mVertices[ vertexNum ].mPosition = p.mPosition + leftTop * p.mSize;
			mVertices[ vertexNum ].mTexCoord.x = 0.0f;
			mVertices[ vertexNum ].mTexCoord.y = 0.0f; 
			mVertices[ vertexNum ].mColor = color;

            mVertices[ v1 ].mPosition = p.mPosition + rightTop * p.mSize;
			mVertices[ v1 ].mTexCoord.x = 1.0f;
			mVertices[ v1 ].mTexCoord.y = 0.0f;
			mVertices[ v1 ].mColor = color;

            mVertices[ v2 ].mPosition = p.mPosition + rightBottom * p.mSize;
			mVertices[ v2 ].mTexCoord.x = 1.0f;
			mVertices[ v2 ].mTexCoord.y = 1.0f;
			mVertices[ v2 ].mColor = color;

            mVertices[ v3 ].mPosition = p.mPosition + leftBottom * p.mSize;
			mVertices[ v3 ].mTexCoord.x = 0.0f;
			mVertices[ v3 ].mTexCoord.y = 1.0f;
			mVertices[ v3 ].mColor = color;

            // indices
            mFaces[ faceNum ].v1 = vertexNum;
			mFaces[ faceNum ].v2 = v1;
			mFaces[ faceNum ].v3 = v3;

			int f1 = faceNum + 1;

            mFaces[ f1 ].v1 = v1;
			mFaces[ f1 ].v2 = v2;
			mFaces[ f1 ].v3 = v3;

            ++mAliveParticleCount;
            vertexNum += 4;
            faceNum += 2;
        }
    }

    void * data = 0;
    mVertexBuffer->Lock( 0, 0, &data,  D3DLOCK_DISCARD );
    memcpy( data, mVertices, mAliveParticleCount * 4 * sizeof( ParticleVertex ));
    mVertexBuffer->Unlock();

    mIndexBuffer->Lock( 0, 0, &data,  D3DLOCK_DISCARD );
    memcpy( data, mFaces, mAliveParticleCount * 2 * sizeof( ParticleFace ));
    mIndexBuffer->Unlock();
}

bool ParticleSystem::IsLightAffects() {
    return mUseLighting;
}

int ParticleSystem::RGBAToInt( ruVector3 color, int alpha ) {
    return D3DCOLOR_ARGB( alpha, (int)color.x, (int)color.y, (int)color.z );
}

void ParticleSystem::ResurrectParticle( Particle & p ) {
    if( mType ==  ruParticleSystem::Type::Box ) {
        p.mPosition = RandomVector3( mBoundingBoxMin, mBoundingBoxMax );
        p.mSpeed = RandomVector3( mSpeedDeviationMin, mSpeedDeviationMax );
    } else if( mType ==  ruParticleSystem::Type::Stream ) {
        p.mPosition = ruVector3( 0, 0, 0 );
        p.mSpeed = RandomVector3( mSpeedDeviationMin, mSpeedDeviationMax );
    }

    p.mSize = mPointSize;
    p.mOpacity = 255;
}

ruVector3 ParticleSystem::RandomVector3( ruVector3 & min, ruVector3 & max ) {
    return ruVector3( frandom( min.x, max.x ), frandom( min.y, max.y ), frandom( min.z, max.z ) );
}

bool ParticleSystem::HasAliveParticles() {
    return mAliveParticleCount > 0;
}

bool ParticleSystem::IsEnabled() {
    return mEnabled;
}

float ParticleSystem::GetThickness() {
    return mParticleThickness;
}

void ParticleSystem::ResurrectParticles() {
    for( auto & particle : mParticles ) {
        ResurrectParticle( particle );
    }
}

ParticleSystem::ParticleSystem( int theParticleCount ) {
    mAliveParticleCount = theParticleCount;
	mMaxParticleCount = theParticleCount;
	OnResetDevice();
    mFaces = new ParticleFace[ theParticleCount * 2 ];
    mVertices = new ParticleVertex[ theParticleCount * 4 ];
    for( int i = 0; i < theParticleCount; i++ ) {
        mParticles.push_back( Particle());
    }
	mType =  ruParticleSystem::Type::Box;

	mColorBegin = ruVector3( 0, 0, 0 );
	mColorEnd = ruVector3( 255, 255, 255 );

	mSpeedDeviationMin = ruVector3( -1, -1, -1 );
	mSpeedDeviationMax = ruVector3( 1, 1, 1 );

	mBoundingBoxMin = ruVector3( 100, 100, 100 );
	mBoundingBoxMax = ruVector3( -100, -100, -100 );

	mParticleThickness = 1.0f;
	mBoundingRadius = 1.0f;
	mPointSize = 1.0f;
	mScaleFactor = 0.0f;

	mDepthHack = 0.0f;
	mAlphaOffset = 0.0f;

	mAutoResurrectDeadParticles = true;
	mUseLighting = false;

	mEnabled = true;

	mTexture = nullptr;
    mFirstTimeUpdate = false;
    ParticleSystem::msParticleEmitters.push_back( this );
    ResurrectParticles();
}

void ParticleSystem::OnLostDevice()
{
	mVertexBuffer->Release();
	mIndexBuffer->Release();
}

void ParticleSystem::OnResetDevice()
{
	Engine::I().GetDevice()->CreateVertexBuffer( mMaxParticleCount * 4 * sizeof( ParticleVertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mVertexBuffer, 0 );
	Engine::I().GetDevice()->CreateIndexBuffer( mMaxParticleCount * 2 * sizeof( ParticleFace ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mIndexBuffer, 0 );
}

ruVector3 ParticleSystem::GetSpeedDeviationMin()
{
	return mSpeedDeviationMin;
}

ruVector3 ParticleSystem::GetSpeedDeviationMax()
{
	return mSpeedDeviationMax;
}

void ParticleSystem::SetSpeedDeviation( const ruVector3 & dMin, const ruVector3 & dMax )
{
	mSpeedDeviationMax = dMax;
	mSpeedDeviationMin = dMin;
}

ruVector3 ParticleSystem::GetBoundingBoxMin()
{
	return mBoundingBoxMin;
}

ruVector3 ParticleSystem::GetBoundingBoxMax()
{
	return mBoundingBoxMax;
}

void ParticleSystem::SetBoundingBox( const ruVector3 & bbMin, const ruVector3 & bbMax )
{
	mBoundingBoxMax = bbMax;
	mBoundingBoxMin = bbMin;
}

bool ParticleSystem::IsLightingEnabled()
{
	return mUseLighting ;
}

void ParticleSystem::SetLightingEnabled( bool state )
{
	mUseLighting = state;
}

bool ParticleSystem::IsAutoResurrectionEnabled()
{
	return mAutoResurrectDeadParticles;
}

void ParticleSystem::SetAutoResurrection( bool state )
{
	mAutoResurrectDeadParticles = state;
}

float ParticleSystem::GetParticleThickness()
{
	return mParticleThickness;
}

void ParticleSystem::SetParticleThickness( float thickness )
{
	mParticleThickness = thickness;
}

float ParticleSystem::GetScaleFactor()
{
	return mScaleFactor;
}

void ParticleSystem::SetScaleFactor( float scaleFactor )
{
	mScaleFactor = scaleFactor;
}

float ParticleSystem::GetPointSize()
{
	return mPointSize;
}

void ParticleSystem::SetPointSize( float size )
{
	mPointSize = size;
}

void ParticleSystem::SetEnabled( bool state ){
	mEnabled = state;
}

int ParticleSystem::GetAliveParticles()
{
	return mAliveParticleCount;
}

shared_ptr<ruTexture> ParticleSystem::GetTexture() {
	return mTexture;
}

void ParticleSystem::SetTexture( shared_ptr<ruTexture> texture ) {
	mTexture = std::dynamic_pointer_cast<Texture>( texture );
}

float ParticleSystem::GetBoundingRadius()
{
	return mBoundingRadius;
}

void ParticleSystem::SetBoundingRadius( float radius )
{
	mBoundingRadius = radius;
}

ruParticleSystem::Type ParticleSystem::GetType()
{
	return mType;
}

void ParticleSystem::SetType( ruParticleSystem::Type type )
{
	mType = type;
}

ruVector3 ParticleSystem::GetColorMax()
{
	return mColorBegin;
}

ruVector3 ParticleSystem::GetColorMin()
{
	return mColorEnd;
}

void ParticleSystem::SetColorRange( const ruVector3 & cMin, const ruVector3 & cMax )
{
	mColorEnd = cMin;
	mColorBegin = cMax;
}

void ParticleSystem::SetAlphaOffset( float alphaOffset )
{
	mAlphaOffset = alphaOffset;
}

float ParticleSystem::GetAlphaOffset()
{
	return mAlphaOffset;
}

Particle::Particle() {
    mPosition = ruVector3( 0, 0, 0 );
    mSpeed = ruVector3( 0, 0, 0 );
    mColor = ruVector3( 255, 255, 255 );
    mOpacity = 255;
    mSize = 1.0f;
}

Particle::Particle( const ruVector3 & thePosition, const ruVector3 & theSpeed, const ruVector3 & theColor, float theTranslucency, float theSize ) {
    mPosition = thePosition;
    mSpeed = theSpeed;
    mColor = theColor;
    mOpacity = theTranslucency;
    mSize = theSize;
}

ruParticleSystem * ruParticleSystem::Create( int particleNum ) {
	return new ParticleSystem( particleNum );
}