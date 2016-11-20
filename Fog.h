#pragma once

#include "AABB.h"
#include "SceneNode.h"

class Fog : public ruFog, public SceneNode {
private:
	ruVector3 mColor;
	ruVector3 mMin;
	ruVector3 mMax;
	float mDensity;
	AABB mAABB;
	ruVector3 mSpeed;
	ruVector3 mOffset;
public:
	Fog(SceneFactory * factory, const ruVector3 & min, const ruVector3 & max, const ruVector3 & color, float density);

	AABB & GetAABB() {
		return mAABB;
	}

	void Update() {
		mOffset += mSpeed;
	}

	ruVector3 GetOffset() const {
		return mOffset;
	}

	// API Methods
	virtual void SetSize(const ruVector3 & min, const ruVector3 & max) override {
		mMax = max;
		mMin = min;
	}
	virtual ruVector3 GetMin() const override {
		return mMin;
	}
	virtual ruVector3 GetMax() const override {
		return mMax;
	}

	virtual void SetSpeed(const ruVector3 & speed) override {
		mSpeed = speed;
	}
	virtual ruVector3 GetSpeed() const override {
		return mSpeed;
	}

	virtual void SetColor(const ruVector3 & color) override {
		mColor = color;
	}
	virtual ruVector3 GetColor() const override {
		return mColor;
	}

	virtual void SetDensity(float density) override {
		mDensity = density;
	}
	virtual float GetDensity() const override {
		return mDensity;
	}
};