#pragma once

#include <math.h>
#include <limits.h>
#include "SaveFile.h"

class SmoothFloat {
protected:
	float mValue;
	float mMin;
	float mMax;
	float mChase;
public:
	explicit SmoothFloat();
	explicit SmoothFloat(float newValue);
	explicit SmoothFloat(float newValue, float minValue, float maxValue);
	void Set(float newValue);
	void SetMin(float minValue);
	void SetMax(float maxValue);
	float GetMin();
	float GetMax();
	bool IsReachEnd();
	bool IsReachBegin();
	void SetTarget(float chaseValue);
	float GetTarget();
	float ChaseTarget(float chaseSpeed = 0.01f);
	operator const float();
	void operator = (float f) {
		mValue = f;
	}
	void Serialize(SaveFile & out);
};
