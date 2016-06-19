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
#include "Renderer.h"
#include "ParticleSystem.h"
#include "Camera.h"
#include "Texture.h"
#include "Utility.h"
#include "SceneFactory.h"

ParticleSystem::~ParticleSystem() {
	OnLostDevice();
	delete mVertices;
	delete mFaces;
}

void ParticleSystem::Update() {
	if (!mFirstTimeUpdate) {
		ResurrectParticles();
		mFirstTimeUpdate = true;
	}

	mAliveParticleCount = 0;

	int vertexNum = 0, faceNum = 0;

	for (auto & p : mParticles) {
		p.mPosition += p.mSpeed;

		float inside = 1.0f;
		if (mType == ruParticleSystem::Type::Box) {
			inside = (p.mPosition).Length2() / (mBoundingBoxMax - mBoundingBoxMin).Length2();
		} else {
			inside = (p.mPosition).Length2() / mBoundingRadius;
		}

		if (inside > 1.0f) {
			inside = 1.0f;
		}

		p.mColor = mColorBegin.Lerp(mColorEnd, inside);
		float fakeOpacity = 255.0f * (1.0f - inside);
		p.mOpacity = fakeOpacity + mAlphaOffset;
		p.mSize += mScaleFactor;

		if (p.mOpacity <= 1.0f) {
			if (mAutoResurrectDeadParticles) {
				ResurrectParticle(p);
			}
		} else {
			short v1 = vertexNum + 1;
			short v2 = vertexNum + 2;
			short v3 = vertexNum + 3;

			mVertices[vertexNum].mPosition = p.mPosition;
			mVertices[vertexNum].mTexCoord.x = 0.0f;
			mVertices[vertexNum].mTexCoord.y = 0.0f;
			mVertices[vertexNum].mBoneIndices = ruVector4(p.mColor / 255.0f, p.mOpacity / 255.0f);
			mVertices[vertexNum].mBoneWeights.x = -1.0f;
			mVertices[vertexNum].mBoneWeights.y = -1.0f;
			mVertices[vertexNum].mBoneWeights.z = p.mSize;

			mVertices[v1].mPosition = p.mPosition;
			mVertices[v1].mTexCoord.x = 1.0f;
			mVertices[v1].mTexCoord.y = 0.0f;
			mVertices[v1].mBoneIndices = ruVector4(p.mColor / 255.0f, p.mOpacity / 255.0f);
			mVertices[v1].mBoneWeights.x = -1.0f;
			mVertices[v1].mBoneWeights.y = 1.0f;
			mVertices[v1].mBoneWeights.z = p.mSize;

			mVertices[v2].mPosition = p.mPosition;
			mVertices[v2].mTexCoord.x = 1.0f;
			mVertices[v2].mTexCoord.y = 1.0f;
			mVertices[v2].mBoneIndices = ruVector4(p.mColor / 255.0f, p.mOpacity / 255.0f);
			mVertices[v2].mBoneWeights.x = 1.0f;
			mVertices[v2].mBoneWeights.y = 1.0f;
			mVertices[v2].mBoneWeights.z = p.mSize;

			mVertices[v3].mPosition = p.mPosition;
			mVertices[v3].mTexCoord.x = 0.0f;
			mVertices[v3].mTexCoord.y = 1.0f;
			mVertices[v3].mBoneIndices = ruVector4(p.mColor / 255.0f, p.mOpacity / 255.0f);
			mVertices[v3].mBoneWeights.x = 1.0f;
			mVertices[v3].mBoneWeights.y = -1.0f;
			mVertices[v3].mBoneWeights.z = p.mSize;

			// indices
			mFaces[faceNum].mA = vertexNum;
			mFaces[faceNum].mB = v1;
			mFaces[faceNum].mC = v3;

			int f1 = faceNum + 1;

			mFaces[f1].mA = v1;
			mFaces[f1].mB = v2;
			mFaces[f1].mC = v3;

			++mAliveParticleCount;
			vertexNum += 4;
			faceNum += 2;
		}
	}

	void * data = 0;
	mVertexBuffer->Lock(0, 0, &data, D3DLOCK_DISCARD);
	memcpy(data, mVertices, mAliveParticleCount * 4 * sizeof(Vertex));
	mVertexBuffer->Unlock();

	mIndexBuffer->Lock(0, 0, &data, D3DLOCK_DISCARD);
	memcpy(data, mFaces, mAliveParticleCount * 2 * sizeof(Triangle));
	mIndexBuffer->Unlock();
}

bool ParticleSystem::IsLightAffects() {
	return mUseLighting;
}

void ParticleSystem::ResurrectParticle(Particle & p) {
	if (mType == ruParticleSystem::Type::Box) {
		p.mPosition = RandomVector3(mBoundingBoxMin, mBoundingBoxMax);
		p.mSpeed = RandomVector3(mSpeedDeviationMin, mSpeedDeviationMax);
	} else if (mType == ruParticleSystem::Type::Stream) {
		p.mPosition = ruVector3(0, 0, 0);
		p.mSpeed = RandomVector3(mSpeedDeviationMin, mSpeedDeviationMax);
	}

	p.mSize = mPointSize;
	p.mOpacity = 255;
}

ruVector3 ParticleSystem::RandomVector3(ruVector3 & min, ruVector3 & max) {
	return ruVector3(frandom(min.x, max.x), frandom(min.y, max.y), frandom(min.z, max.z));
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
	for (auto & particle : mParticles) {
		ResurrectParticle(particle);
	}
}

ParticleSystem::ParticleSystem(int theParticleCount) :
	mAliveParticleCount(theParticleCount),
	mMaxParticleCount(theParticleCount),
	mType(ruParticleSystem::Type::Box),
	mColorBegin(0, 0, 0),
	mColorEnd(255, 255, 255),
	mSpeedDeviationMin(-1, -1, -1),
	mSpeedDeviationMax(1, 1, 1),
	mBoundingBoxMin(100, 100, 100),
	mBoundingBoxMax(-100, -100, -100),
	mParticleThickness(1.0f),
	mBoundingRadius(1.0f),
	mPointSize(1.0f),
	mScaleFactor(0.0f),
	mAlphaOffset(0.0f),
	mAutoResurrectDeadParticles(true),
	mUseLighting(false),
	mEnabled(true),
	mTexture(nullptr),
	mFirstTimeUpdate(false) {
	OnResetDevice();
	mFaces = new Triangle[theParticleCount * 2];
	mVertices = new Vertex[theParticleCount * 4];
	for (int i = 0; i < theParticleCount; i++) {
		mParticles.push_back(Particle());
	}
	ResurrectParticles();
}

void ParticleSystem::OnLostDevice() {
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();
}

void ParticleSystem::OnResetDevice() {
	pD3D->CreateVertexBuffer(mMaxParticleCount * 4 * sizeof(Vertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mVertexBuffer, 0);
	pD3D->CreateIndexBuffer(mMaxParticleCount * 2 * sizeof(Triangle), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mIndexBuffer, 0);
}

ruVector3 ParticleSystem::GetSpeedDeviationMin() {
	return mSpeedDeviationMin;
}

ruVector3 ParticleSystem::GetSpeedDeviationMax() {
	return mSpeedDeviationMax;
}

void ParticleSystem::SetSpeedDeviation(const ruVector3 & dMin, const ruVector3 & dMax) {
	mSpeedDeviationMax = dMax;
	mSpeedDeviationMin = dMin;
}

ruVector3 ParticleSystem::GetBoundingBoxMin() {
	return mBoundingBoxMin;
}

ruVector3 ParticleSystem::GetBoundingBoxMax() {
	return mBoundingBoxMax;
}

void ParticleSystem::SetBoundingBox(const ruVector3 & bbMin, const ruVector3 & bbMax) {
	mBoundingBoxMax = bbMax;
	mBoundingBoxMin = bbMin;
}

bool ParticleSystem::IsLightingEnabled() {
	return mUseLighting;
}

void ParticleSystem::SetLightingEnabled(bool state) {
	mUseLighting = state;
}

bool ParticleSystem::IsAutoResurrectionEnabled() {
	return mAutoResurrectDeadParticles;
}

void ParticleSystem::SetAutoResurrection(bool state) {
	mAutoResurrectDeadParticles = state;
}

float ParticleSystem::GetParticleThickness() {
	return mParticleThickness;
}

void ParticleSystem::SetParticleThickness(float thickness) {
	mParticleThickness = thickness;
}

float ParticleSystem::GetScaleFactor() {
	return mScaleFactor;
}

void ParticleSystem::SetScaleFactor(float scaleFactor) {
	mScaleFactor = scaleFactor;
}

float ParticleSystem::GetPointSize() {
	return mPointSize;
}

void ParticleSystem::SetPointSize(float size) {
	mPointSize = size;
}

void ParticleSystem::SetEnabled(bool state) {
	mEnabled = state;
}

int ParticleSystem::GetAliveParticles() {
	return mAliveParticleCount;
}

shared_ptr<ruTexture> ParticleSystem::GetTexture() {
	return mTexture;
}

void ParticleSystem::SetTexture(const shared_ptr<ruTexture> & texture) {
	mTexture = std::dynamic_pointer_cast<Texture>(texture);
}

float ParticleSystem::GetBoundingRadius() {
	return mBoundingRadius;
}

void ParticleSystem::SetBoundingRadius(float radius) {
	mBoundingRadius = radius;
}

ruParticleSystem::Type ParticleSystem::GetType() {
	return mType;
}

void ParticleSystem::SetType(ruParticleSystem::Type type) {
	mType = type;
}

ruVector3 ParticleSystem::GetColorMax() {
	return mColorBegin;
}

ruVector3 ParticleSystem::GetColorMin() {
	return mColorEnd;
}

void ParticleSystem::SetColorRange(const ruVector3 & cMin, const ruVector3 & cMax) {
	mColorEnd = cMin;
	mColorBegin = cMax;
}

void ParticleSystem::SetAlphaOffset(float alphaOffset) {
	mAlphaOffset = alphaOffset;
}

float ParticleSystem::GetAlphaOffset() {
	return mAlphaOffset;
}

Particle::Particle() :
	mPosition(0, 0, 0),
	mSpeed(0, 0, 0),
	mColor(255, 255, 255),
	mOpacity(255),
	mSize(1.0f) {

}

Particle::Particle(const ruVector3 & thePosition, const ruVector3 & theSpeed, const ruVector3 & theColor, float theTranslucency, float theSize) :
	mPosition(thePosition),
	mSpeed(theSpeed),
	mColor(theColor),
	mOpacity(theTranslucency),
	mSize(theSize) {

}

shared_ptr<ruParticleSystem> ruParticleSystem::Create(int particleNum) {
	return SceneFactory::CreateParticleSystem(particleNum);
}