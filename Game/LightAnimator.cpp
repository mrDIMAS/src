#include "Precompiled.h"

#include "LightAnimator.h"

LightAnimator::LightAnimator( ruNodeHandle lit, float as, float onRange, float peakRangeMult ) {
    if( !ruIsLight( lit )) {
        RaiseError( "Node is not a light!" );
    }
    mLight = lit;
    mAnimSpeed = as;
    mOnRange = onRange;
    mPeakRangeMult = peakRangeMult;
    mAnimType = AnimationType::On;
    mRange = onRange;
    mRangeDest = mRange;
    ruSetLightRange( mLight, mRange );
}

void LightAnimator::Update() {
    mRange = mRange + ( mRangeDest - mRange ) * mAnimSpeed;

    ruSetLightRange( mLight, mRange );

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
