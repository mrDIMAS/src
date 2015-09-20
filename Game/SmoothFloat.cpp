#include "Precompiled.h"

#include "SmoothFloat.h"

void SmoothFloat::Deserialize( SaveFile & in ) {
    mValue = in.ReadFloat();
    mChase = in.ReadFloat();
    mMin = in.ReadFloat();
    mMax = in.ReadFloat();
}

void SmoothFloat::Serialize( SaveFile & out ) {
    out.WriteFloat( mValue );
    out.WriteFloat( mChase );
    out.WriteFloat( mMin );
    out.WriteFloat( mMax );
}

SmoothFloat::operator const float() {
    return mValue;
}

float SmoothFloat::ChaseTarget( float chaseSpeed ) {
    if( mChase < mMin ) {
        mChase = mMin;
    } else if( mChase > mMax ) {
        mChase = mMax;
    }
    mValue += ( mChase - mValue ) * chaseSpeed;
    return mValue;
}

float SmoothFloat::GetTarget() {
    return mChase;
}

void SmoothFloat::SetTarget( float chaseValue ) {
    mChase = chaseValue;
}

bool SmoothFloat::IsReachBegin() {
    return abs( mValue - mMax ) < 0.01f;
}

bool SmoothFloat::IsReachEnd() {
    return abs( mValue - mMax ) < 0.01f;
}

void SmoothFloat::SetMax( float maxValue ) {
    mMax = maxValue;
}

void SmoothFloat::SetMin( float minValue ) {
    mMin = minValue;
}

void SmoothFloat::Set( float newValue ) {
    mValue = newValue;
}

SmoothFloat::SmoothFloat( float newValue, float minValue, float maxValue ) : mMin( minValue ), mMax( maxValue ), mChase( newValue ) {
    Set( newValue );
}

SmoothFloat::SmoothFloat( float newValue ) : mMin( -FLT_MAX ), mMax( FLT_MAX ), mChase( newValue ) {
    Set( newValue );
}

SmoothFloat::SmoothFloat() : mMin( -FLT_MAX ), mMax( FLT_MAX ), mChase( 0.0f ) {
    mValue = 0.0f;
}

float SmoothFloat::GetMax() {
    return mMax;
}

float SmoothFloat::GetMin() {
    return mMin;
}
