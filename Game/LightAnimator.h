#pragma once

#include "Game.h"

class LightAnimator {
public:
	enum class AnimationType {
		PeakFalloff = 1,
		PeakNormal,
		Off,
		On,
	};
private:
	float mRangeDest;
	float mRange;
	float mPeakRangeMult;
	float mOnRange;
	shared_ptr<ruLight>mLight;
	AnimationType mAnimType;
	float mAnimSpeed;
public:
	explicit LightAnimator(shared_ptr<ruLight>lit, float as, float onRange, float peakRangeMult);
	void Update();
	void SetAnimationType(AnimationType lat);
	AnimationType GetAnimationType();
	static void UpdateAll();
};