#pragma once

#include "AABB.h"
#include "SceneNode.h"

class Fog : public IFog, public SceneNode {
private:
	Vector3 mColor;
	Vector3 mMin;
	Vector3 mMax;
	float mDensity;
	AABB mAABB;
	Vector3 mSpeed;
	Vector3 mOffset;
public:
	Fog(SceneFactory * factory, const Vector3 & min, const Vector3 & max, const Vector3 & color, float density);

	AABB & GetAABB() {
		return mAABB;
	}

	void Update() {
		mOffset += mSpeed;
	}

	Vector3 GetOffset() const {
		return mOffset;
	}

	// API Methods
	virtual void SetSize(const Vector3 & min, const Vector3 & max) override {
		mMax = max;
		mMin = min;
	}
	virtual Vector3 GetMin() const override {
		return mMin;
	}
	virtual Vector3 GetMax() const override {
		return mMax;
	}

	virtual void SetSpeed(const Vector3 & speed) override {
		mSpeed = speed;
	}
	virtual Vector3 GetSpeed() const override {
		return mSpeed;
	}

	virtual void SetColor(const Vector3 & color) override {
		mColor = color;
	}
	virtual Vector3 GetColor() const override {
		return mColor;
	}

	virtual void SetDensity(float density) override {
		mDensity = density;
	}
	virtual float GetDensity() const override {
		return mDensity;
	}
};