#include "Common.h"

ruVector3::ruVector3( ) : x( 0.0f ), y( 0.0f ), z( 0.0f ) {
};

ruVector3::ruVector3( float x, float y, float z ) : x( x ), y( y ), z( z ) {
};

ruVector3::ruVector3( const ruVector3 & v ) {
    x = v.x;
    y = v.y;
    z = v.z;
};

ruVector3::ruVector3( float * v ) {
    x = v[ 0 ];
    y = v[ 1 ];
    z = v[ 2 ];
}

ruVector3 ruVector3::operator + ( const ruVector3 & v ) const {
    return ruVector3( x + v.x, y + v.y, z + v.z );
}

ruVector3 ruVector3::operator - ( const ruVector3 & v ) const {
    return ruVector3( x - v.x, y - v.y, z - v.z );
}

ruVector3 ruVector3::operator * ( const ruVector3 & v ) const {
    return ruVector3( x * v.x, y * v.y, z * v.z );
}

ruVector3 ruVector3::operator * ( const float & f ) const {
    return ruVector3( x * f, y * f, z * f );
}

ruVector3 ruVector3::operator / ( const ruVector3 & v ) const {
    return ruVector3( x / v.x, y / v.y, z / v.z );
}

ruVector3 ruVector3::operator / ( const float & f ) const {
    return ruVector3( x / f, y / f, z / f );
}

void ruVector3::operator *= ( const ruVector3 & v ) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
}

float ruVector3::Angle( const ruVector3 & v ) {
    btScalar s = btSqrt( Length2() * v.Length2());
    return acosf( Dot(v) / s );
}

void ruVector3::operator /= ( const ruVector3 & v ) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
}

void ruVector3::operator += ( const ruVector3 & v ) {
    x += v.x;
    y += v.y;
    z += v.z;
}

void ruVector3::operator -= ( const ruVector3 & v ) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

void ruVector3::operator = ( const ruVector3 & v ) {
    x = v.x;
    y = v.y;
    z = v.z;
}

bool ruVector3::operator == ( const ruVector3 & v ) {
    float dx = abs( x - v.x );
    float dy = abs( y - v.y );
    float dz = abs( z - v.z );

    return dx > 0.0001f && dy > 0.0001f && dz > 0.0001f;
}

float ruVector3::Length( ) const {
    return sqrt( x * x + y * y + z * z );
}

float ruVector3::Length2( ) const {
    return x * x + y * y + z * z;
}

ruVector3 ruVector3::Normalize( ) {
    float l = 1.0f / Length();

    x *= l;
    y *= l;
    z *= l;

    return *this;
}

ruVector3 ruVector3::Normalized() const {
    float l = 1.0f / Length();

    return ruVector3( x * l, y * l, z * l );
}

ruVector3 ruVector3::Cross( const ruVector3 & v ) const {
    return ruVector3( y * v.z - z * v.x, z * v.x - x * v.z, x * v.y - y * v.x );
}

float ruVector3::Dot( const ruVector3 & v ) const {
    return x * v.x + y * v.y + z * v.z;
}

ruVector3 ruVector3::Rotate( const ruVector3 & axis, float angle ) {
    angle *= DEG2RAD;

    ruVector3 o = axis * axis.Dot( *this );
    ruVector3 x = *this - o;
    ruVector3 y;

    y = axis.Cross( *this );

    return ( o + x * cosf( angle ) + y * sinf( angle ) );
}

ruVector3 ruVector3::Lerp( const ruVector3 & v, float t ) const {
    return ruVector3( x + ( v.x - x ) * t, y + ( v.y - y ) * t, z + ( v.z - z ) * t );
}