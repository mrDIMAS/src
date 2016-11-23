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

#pragma once

#include "SceneNode.h"
#include "Texture.h"
#include "Vertex.h"

class Particle {
public:
	Vector3 mPosition;
	Vector3 mColor;
	Vector3 mSpeed;
	float mOpacity;
	float mSize;

	explicit Particle(const Vector3 & thePosition, const Vector3 & theSpeed, const Vector3 & theColor, float theTranslucency, float theSize);
	Particle();
};

class ParticleSystem : public virtual IParticleSystem, public SceneNode {
public:
	vector< Particle > mParticles;
	Vertex * mVertices;
	Triangle * mFaces;
	int mAliveParticleCount;
	int mMaxParticleCount;
	D3DXMATRIX mWorldTransform;
	bool mFirstTimeUpdate;
	IParticleSystem::Type mType;
	Vector3 mColorBegin;
	Vector3 mColorEnd;
	Vector3 mSpeedDeviationMin;
	Vector3 mSpeedDeviationMax;
	Vector3 mBoundingBoxMin;
	Vector3 mBoundingBoxMax;
	float mParticleThickness;
	float mBoundingRadius;
	float mPointSize;
	float mScaleFactor;
	bool mAutoResurrectDeadParticles;
	bool mUseLighting;
	bool mEnabled;
	float mAlphaOffset;
	Vector3 RandomVector3(Vector3 & min, Vector3 & max);
public:
	// Data for renderer
	shared_ptr<Texture> mTexture;
	COMPtr<IDirect3DVertexBuffer9> mVertexBuffer;
	COMPtr<IDirect3DIndexBuffer9> mIndexBuffer;

public:
	ParticleSystem(SceneFactory * factory, int theParticleCount);
	~ParticleSystem();
	void OnResetDevice();
	void OnLostDevice();
	virtual void ResurrectParticles();
	float GetThickness();
	bool HasAliveParticles();
	void ResurrectParticle(Particle & p);
	bool IsLightAffects();
	void Update();
	virtual int GetAliveParticles();
	virtual bool IsEnabled();
	virtual void SetEnabled(bool state);
	virtual void SetPointSize(float size);
	virtual float GetPointSize();
	virtual void SetScaleFactor(float scaleFactor);
	virtual float GetScaleFactor();
	virtual void SetParticleThickness(float thickness);
	virtual float GetParticleThickness();
	virtual void SetAutoResurrection(bool state);
	virtual bool IsAutoResurrectionEnabled();
	virtual void SetLightingEnabled(bool state);
	virtual bool IsLightingEnabled();
	virtual void SetBoundingBox(const Vector3 & bbMin, const Vector3 & bbMax);
	virtual Vector3 GetBoundingBoxMax();
	virtual Vector3 GetBoundingBoxMin();
	virtual void SetSpeedDeviation(const Vector3 & dMin, const Vector3 & dMax);
	virtual Vector3 GetSpeedDeviationMax();
	virtual Vector3 GetSpeedDeviationMin();
	virtual void SetTexture(const shared_ptr<ITexture> & texture);
	virtual shared_ptr<ITexture> GetTexture();
	virtual void SetType(IParticleSystem::Type type);
	virtual IParticleSystem::Type GetType();
	virtual void SetBoundingRadius(float radius);
	virtual float GetBoundingRadius();
	virtual void SetColorRange(const Vector3 & cMin, const Vector3 & cMax);
	virtual Vector3 GetColorMin();
	virtual Vector3 GetColorMax();
	virtual float GetAlphaOffset();
	virtual void SetAlphaOffset(float alphaOffset);
};