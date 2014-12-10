#include "SmoothFloat.h"

void SmoothFloat::Deserialize( TextFileStream & in )
{
    value = in.ReadFloat();
    chase = in.ReadFloat();
    min = in.ReadFloat();
    max = in.ReadFloat();
}

void SmoothFloat::Serialize( TextFileStream & out )
{
    out.WriteFloat( value );
    out.WriteFloat( chase );
    out.WriteFloat( min );
    out.WriteFloat( max );
}

SmoothFloat::operator const float()
{
    return value;
}

float SmoothFloat::ChaseTarget( float chaseSpeed )
{
    if( chase < min )
        chase = min;
    else if( chase > max )
        chase = max;
    value += ( chase - value ) * chaseSpeed;
    return value;
}

float SmoothFloat::GetTarget()
{
    return chase;
}

void SmoothFloat::SetTarget( float chaseValue )
{
    chase = chaseValue;
}

bool SmoothFloat::IsReachBegin()
{
    return abs( value - max ) < 0.01f;
}

bool SmoothFloat::IsReachEnd()
{
    return abs( value - max ) < 0.01f;
}

void SmoothFloat::SetMax( float maxValue )
{
    max = maxValue;
}

void SmoothFloat::SetMin( float minValue )
{
    min = minValue;
}

void SmoothFloat::Set( float newValue )
{
    value = newValue;
}

SmoothFloat::SmoothFloat( float newValue, float minValue, float maxValue ) : min( minValue ), max( maxValue ), chase( newValue )
{
    Set( newValue );
}

SmoothFloat::SmoothFloat( float newValue ) : min( -FLT_MAX ), max( FLT_MAX ), chase( newValue )
{
    Set( newValue );
}

SmoothFloat::SmoothFloat() : min( -FLT_MAX ), max( FLT_MAX ), chase( 0.0f )
{
    value = 0.0f;
}

float SmoothFloat::GetMax()
{
    return max;
}

float SmoothFloat::GetMin()
{
    return min;
}
