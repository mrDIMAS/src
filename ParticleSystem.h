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

#pragma once

#include "SceneNode.h"
#include "Texture.h"
#include "Vertex.h"

class Particle {
public:
	ruVector3 mPosition;
	ruVector3 mColor;
	ruVector3 mSpeed;
	float mOpacity;
	float mSize;

	explicit Particle( const ruVector3 & thePosition, const ruVector3 & theSpeed, const ruVector3 & theColor, float theTranslucency, float theSize );
	Particle( );
};

class ParticleSystem : public virtual ruParticleSystem, public SceneNode {
public:
    vector< Particle > mParticles;
    Vertex * mVertices;
    Triangle * mFaces;
	int mAliveParticleCount;
	int mMaxParticleCount;
	D3DXMATRIX mWorldTransform;
    bool mFirstTimeUpdate;
	ruParticleSystem::Type mType;
	ruVector3 mColorBegin;
	ruVector3 mColorEnd;
	ruVector3 mSpeedDeviationMin;
	ruVector3 mSpeedDeviationMax;
	ruVector3 mBoundingBoxMin;
	ruVector3 mBoundingBoxMax;
	float mParticleThickness;
	float mBoundingRadius;
	float mPointSize;
	float mScaleFactor;
	bool mAutoResurrectDeadParticles;
	bool mUseLighting;
	bool mEnabled;
	float mAlphaOffset;	
    ruVector3 RandomVector3( ruVector3 & min, ruVector3 & max );
public:
	// Data for renderer
	shared_ptr<Texture> mTexture;
	COMPtr<IDirect3DVertexBuffer9> mVertexBuffer;
	COMPtr<IDirect3DIndexBuffer9> mIndexBuffer;

public:
    ParticleSystem( int theParticleCount );
    ~ParticleSystem();
	void OnResetDevice();
	void OnLostDevice();
    virtual void ResurrectParticles();
    float GetThickness( );
    bool HasAliveParticles();
    void ResurrectParticle( Particle & p );
    bool IsLightAffects( );
    void Update( );
	virtual int GetAliveParticles();
	virtual bool IsEnabled();
	virtual void SetEnabled( bool state );
	virtual void SetPointSize( float size );
	virtual float GetPointSize();
	virtual void SetScaleFactor( float scaleFactor );
	virtual float GetScaleFactor( );
	virtual void SetParticleThickness( float thickness );
	virtual float GetParticleThickness( );
	virtual void SetAutoResurrection( bool state );
	virtual bool IsAutoResurrectionEnabled();
	virtual void SetLightingEnabled( bool state );
	virtual bool IsLightingEnabled( );
	virtual void SetBoundingBox( const ruVector3 & bbMin, const ruVector3 & bbMax );
	virtual ruVector3 GetBoundingBoxMax( );
	virtual ruVector3 GetBoundingBoxMin( );
	virtual void SetSpeedDeviation( const ruVector3 & dMin, const ruVector3 & dMax );
	virtual ruVector3 GetSpeedDeviationMax();
	virtual ruVector3 GetSpeedDeviationMin();
	virtual void SetTexture( const shared_ptr<ruTexture> & texture );
	virtual shared_ptr<ruTexture> GetTexture( );
	virtual void SetType( ruParticleSystem::Type type );
	virtual ruParticleSystem::Type GetType();
	virtual void SetBoundingRadius( float radius );
	virtual float GetBoundingRadius( );
	virtual void SetColorRange( const ruVector3 & cMin, const ruVector3 & cMax );
	virtual ruVector3 GetColorMin( );
	virtual ruVector3 GetColorMax( );
	virtual float GetAlphaOffset( );
	virtual void SetAlphaOffset( float alphaOffset );
};