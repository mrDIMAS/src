#include "Precompiled.h"

#include "LightAnimator.h"

LightAnimator::LightAnimator( ruLight * lit, float as, float onRange, float peakRangeMult ) {
    mLight = lit;
    mAnimSpeed = as;
    mOnRange = onRange;
    mPeakRangeMult = peakRangeMult;
    mAnimType = AnimationType::On;
    mRange = onRange;
    mRangeDest = mRange;
    mLight->SetRange( mRange );
}

void LightAnimator::Update() {
    mRange = mRange + ( mRangeDest - mRange ) * mAnimSpeed;

    mLight->SetRange( mRange );

    if( mAnimType == AnimationType::On ) {
        mRangeDest = mOnRange;
    }

    if( mAnimType == AnimationType::Off ) {
        mRangeDest = 0;
    }
}

void LightAnimator::SetAnimationType( AnimationType lat ) {
    mAnimType = lat;
}

LightAnimator::AnimationType LightAnimator::GetAnimationType() {
    return mAnimType;
}
