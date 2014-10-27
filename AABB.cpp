#include "AABB.h"

void AABB::Visualize( Vector3 offset ) {
    Vector3 color = Vector3( 0, 255, 0 );
    Draw3DLine( LinePoint( Vector3( min.x,min.y,min.z ) + offset, color ), LinePoint( Vector3( min.x,min.y,max.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( min.x,min.y,max.z ) + offset, color ), LinePoint( Vector3( max.x,min.y,max.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( max.x,min.y,max.z ) + offset, color ), LinePoint( Vector3( max.x,min.y,min.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( max.x,min.y,min.z ) + offset, color ), LinePoint( Vector3( min.x,min.y,min.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( min.x,max.y,min.z ) + offset, color ), LinePoint( Vector3( min.x,max.y,max.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( min.x,max.y,max.z ) + offset, color ), LinePoint( Vector3( max.x,max.y,max.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( max.x,max.y,max.z ) + offset, color ), LinePoint( Vector3( max.x,max.y,min.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( max.x,max.y,min.z ) + offset, color ), LinePoint( Vector3( min.x,max.y,min.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( min.x,min.y,min.z ) + offset, color ), LinePoint( Vector3( min.x,max.y,min.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( min.x,min.y,max.z ) + offset, color ), LinePoint( Vector3( min.x,max.y,max.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( max.x,min.y,max.z ) + offset, color ), LinePoint( Vector3( max.x,max.y,max.z ) + offset, color ));
    Draw3DLine( LinePoint( Vector3( max.x,min.y,min.z ) + offset, color ), LinePoint( Vector3( max.x,max.y,min.z ) + offset, color ));
}

bool AABB::IsTriangleInside( const Vector3 & v1, const Vector3 & v2, const Vector3 & v3 ) {
    return IsPointInside( v1 ) || IsPointInside( v2 ) || IsPointInside( v3 );
}

bool AABB::IsPointInside( const Vector3 & v ) {
    return  v.x >= min.x && v.x <= max.x &&
            v.y >= min.y && v.y <= max.y &&
            v.z >= min.z && v.z <= max.z ;
}

AABB::AABB( const Vector3 & min, const Vector3 & max ) {
    this->min = min;
    this->max = max;

    CalculateVerticesRadiusCenter();
}

AABB::AABB() {
    radius = 0.0f;
    CalculateVerticesRadiusCenter();
}

AABB::AABB( const vector< Vector3 > & points ) {
    max = Vector3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
    min = Vector3( FLT_MAX, FLT_MAX, FLT_MAX );
    for( auto & v : vertices ) {
        if( v.x > max.x ) {
            max.x = v.x;
        } else if( v.x < min.x ) {
            min.x = v.x;
        }
        if( v.y > max.y ) {
            max.y = v.y;
        } else if( v.y < min.y ) {
            min.y = v.y;
        }
        if( v.z > max.z ) {
            max.z = v.z;
        } else if( v.z < min.z ) {
            min.z = v.z;
        }           
    }
    CalculateVerticesRadiusCenter();
}

AABB::AABB( const vector< Vertex > & vertices )
{
    max = Vector3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
    min = Vector3( FLT_MAX, FLT_MAX, FLT_MAX );
    for( auto & vert : vertices ) {
        auto & v = vert.coords;
        if( v.x > max.x ) {
            max.x = v.x;
        } else if( v.x < min.x ) {
            min.x = v.x;
        }
        if( v.y > max.y ) {
            max.y = v.y;
        } else if( v.y < min.y ) {
            min.y = v.y;
        }
        if( v.z > max.z ) {
            max.z = v.z;
        } else if( v.z < min.z ) {
            min.z = v.z;
        }           
    }
    CalculateVerticesRadiusCenter();
}

bool AABB::IsIntersects( const AABB & aabb ) {
    for( int i = 0; i < 8; i++ ) {
        if( IsPointInside( aabb.vertices[i])) {
            return true;
        }
    }
    return false;
}

void AABB::CalculateVerticesRadiusCenter() {
    
    vertices[0] = min;
    vertices[1] = Vector3( min.x, min.y, max.z );
    vertices[2] = Vector3( max.x, min.y, max.z );
    vertices[3] = Vector3( max.x, min.y, min.z );

    vertices[4] = Vector3( min.x, max.y, min.z );
    vertices[5] = Vector3( min.x, max.y, max.z );
    vertices[6] = max;
    vertices[7] = Vector3( max.x, max.y, min.z );
    
    /*
    vertices[0] = Vector3( min.x, min.y, min.z );
    vertices[1] = Vector3( max.x, min.y, min.z );
    vertices[2] = Vector3( max.x, min.y, max.z );
    vertices[3] = Vector3( min.x, min.y, max.z );

    vertices[4] = Vector3( min.x, max.y, min.z );
    vertices[5] = Vector3( max.x, max.y, min.z );
    vertices[6] = Vector3( max.x, max.y, max.z );
    vertices[7] = Vector3( min.x, max.y, max.z );*/
    radius = ( max - min ).Length();

    center = ( min + max ) / 2;
}
