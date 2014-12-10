#include "Common.h"

ruQuaternion::ruQuaternion( )
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
};

ruQuaternion::ruQuaternion( float x, float y, float z, float w )
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
};

ruQuaternion::ruQuaternion( float pitch, float yaw, float roll )
{
    pitch *= DEG2RAD;
    yaw *= DEG2RAD;
    roll *= DEG2RAD;

    float halfYaw = yaw * 0.5f;
    float halfPitch = pitch * 0.5f;
    float halfRoll = roll * 0.5f;
    float cosYaw = cosf(halfYaw);
    float sinYaw = sinf(halfYaw);
    float cosPitch = cosf(halfPitch);
    float sinPitch = sinf(halfPitch);
    float cosRoll = cosf(halfRoll);
    float sinRoll = sinf(halfRoll);
    x = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
    y = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
    z = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
    w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
};

ruQuaternion::ruQuaternion( const ruVector3 & axis, float angle )
{
    angle *= DEG2RAD;

    float d = axis.Length();
    float s = sinf( angle * 0.5f ) / d;
    x = axis.x * s;
    y = axis.y * s;
    z = axis.z * s;
    w = cosf( angle * 0.5f );
};